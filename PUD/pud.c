
/**
  *********************************************************************************************
  * @file           : pud.c
  * @brief          : PUD application functions
  * @author         : Artur Petrosyan, Home hand-made, Ltd.
  * @date           : 10/05/2024
  * @date           : 
  *
  * @note 
  *********************************************************************************************
*/

#include "main.h"
#include "spi.h"
#include "tim.h"
#include "fdcan.h"
#include "pud.h"
#include "tle7230.h"
#include "modbus.h"

FDCAN_TxHeaderTypeDef CAN_TxHeader;
FDCAN_RxHeaderTypeDef CAN_RxHeader;
volatile uint8_t can_flg;
uint8_t rx_data[32];


/**
 *********************************************************************************************
 * @brief ������� ������������� ������ ������
 *
 * @param [ton] ��������� �� ������ � ��������� ��������
 * @param [adc_buf] ��������� �� ����� ������������ ������
 *
 * @retval [st] ������ �������������
 *
 * @note  
 *
 *********************************************************************************************
*/
uint8_t TON4980_Start (uint16_t *ton)
{
    HAL_StatusTypeDef st;
 
    /* �������� ��������� ������� 4980 ��. ������������ ���������� NSS */
    st = HAL_SPI_Transmit_DMA(&DAC_SPI, (uint8_t *)ton, TON4980_POINT_NUM);
    /* ��������� ������ ������������� ���������� � ��� */
    st |= HAL_TIM_Base_Start(&htim12);
    st |= HAL_TIM_Base_Start_IT(&htim7);
    st |= HAL_TIM_Base_Start_IT(&htim16);

    if (st != HAL_OK) {
        return PUD_ERR;
    }
    
    return PUD_OK;
}

/**
 *********************************************************************************************
 * @brief ������� ��������� ������
 *
 * @param [ton] ��������� �� ������ � ��������� ��������
 * @param [adc_buf] ��������� �� ����� ������������ ������
 *
 * @retval [st] ������ �������������
 *
 * @note  
 *
 *********************************************************************************************
*/
uint8_t TON4980_Stop (void)
{
    HAL_StatusTypeDef st;
    
    /* ������������� ������ ������������� ���������� � ��� */
    st = HAL_TIM_Base_Stop(&htim12);
    st |= HAL_TIM_Base_Stop(&htim16);
    st |= HAL_TIM_Base_Stop(&htim7);

    if (st != HAL_OK) {
        return PUD_ERR;
    }
    
    return PUD_OK;
}

/**
 *********************************************************************************************
 * @brief ������� ��������� ������������� ������� ���������� ������� ������
 *
 * @param [data] ��������� �� ������ � �������
 * @param [res] ��������� �� ��������� ���������
 *
 * @retval [] None
 *
 * @note 
 * 
 *********************************************************************************************
*/
void MilkSensor_Process (uint8_t *adc_buf, float *milk_counter)
{
  enum state {
      MILKING_OFF = 0,
      INTAKE_START,
      INTAKE,
      OUTTAKE_START,
      OUTTAKE
  };
  uint32_t data, i;
  static uint32_t timer;
  static uint8_t state = MILKING_OFF;
  static float milk_portion = 0;
  static int32_t elec1 = 0, elec2 = 0;
  
  
    // �������� ������� ������� ���������, ������� ��������� �� ������� �������
    data = 0;
    for (i = 0; i < ADC_BUF_SIZE; i += 3) {
         data += (adc_buf[i+1] << 8 | adc_buf[i+2]) & 0x00000FFF;
    }
    if (ELEC1_STATE() == 0) {
        elec1 /= (int32_t)(ADC_BUF_SIZE / 3);
    } else {
        elec2 /= (int32_t)(ADC_BUF_SIZE / 3);
    }
  
    // ���� ��� ����������� ����� ����� ������� ������. ��������� ������������� �����������
    if ((PUD.start_milking_btn == 1) && (state == MILKING_OFF)) {
        MILK_OUTTAKE_OFF();
        MILK_INTAKE_ON();
        state = INTAKE_START;
        *milk_counter = 0;
        milk_portion = FIRST_MILK_PORTION;
        timer = HAL_GetTick();
        elec1 = 0;
        elec2 = 0;
        return;
    }
    
    // ���� ����� ������ �������� ��������� �������
    if (state == INTAKE_START) {
        if (elec1 < elec2) {
            state = INTAKE;
            // �������� ������ ������� ���������� �������
            timer = HAL_GetTick();
            return;
        }
    }
    
    // ���� ���������� ������� �� ������� ��������� �� ��������.
    // ���� ����� ������� �������� ��������� �������. ������������ ������� ��������� ����, ��� ��������
    // �.�. �� ������ �������. ����� ������������ ���������� (������ ���� ��������� ������ MILK_SENSOR_TRS,
    // ��� ��������, ��� ������� �������� �������� �������. ��������� �����, ��������� ������.
    if (state == INTAKE) {
        // ���� ������� ������� ����� �����������.
        // ���� ������� �������� �����, ������ �������� �������� �������
        if ((elec2 - elec1) <= MILK_SENSOR_TRS) {
            // ��������� �����, ��������� ������
            MILK_INTAKE_OFF();
            MILK_OUTTAKE_ON();
            state = OUTTAKE_START;
            return;
        }
    }
    
    // ���� ����� ������� ������ ��������� ���� �������� ���������
    if (state == OUTTAKE_START) {
        if (elec2 > elec1) {
            state = OUTTAKE;
        }
        return;
    }
    
    // ��� ���� ����� ������ �� ������������� �������. ���� ����� ������� ������ ���������
    // �� ������� ���������
    if (state == OUTTAKE) {
        if ((elec2 - elec1) <= MILK_SENSOR_TRS) {
        // ��������� ������, ��������� �����
            MILK_OUTTAKE_OFF();
            MILK_INTAKE_ON();
            state = INTAKE_START;
            
            // ���� ��� ������ ����, ����������� ���-�� ������ �� �������� FIRST_MILK_PORTION
            // ����� �� MILK_PORTION. � ��������� �� LAST_MILK_PORTION
            *milk_counter += milk_portion;
            if (milk_portion == FIRST_MILK_PORTION) {
                milk_portion = MILK_PORTION;
            }
        }
        return;
    }
    
    // ���� ����� ���������� ��������� �������, �������� ��������� ���� ������ � ��������� ������
    if (((state == INTAKE_START) || (state == INTAKE)) && ((HAL_GetTick() - timer) >= MILKING_TIMEOUT)) {
        MILK_OUTTAKE_ON();
        *milk_counter += LAST_MILK_PORTION;
        state = MILKING_OFF;
    }
}

/**
 *********************************************************************************************
 * @brief ������� ���������� ������ BTS716G
 *
 * @param [chip] ����� ���� � �����. ����� ���� 7, 8 � 9.
 * @param [cmd]  ������� �� ���/���� - BTS716G_ON/BTS716G_OFF
 *
 * @retval [] None
 *
 * @note 
 * 
 *********************************************************************************************
*/
uint8_t BTS716G (BTS716G_CHIP chip,  BTS716G_OUT out, BTS716G_STATE state)
{
    if (chip == BTS716G_7) {
        if (out == BTS716G_OUT_1) {
            HAL_GPIO_WritePin(DO_PULS1_GPIO_Port, DO_PULS1_Pin, (GPIO_PinState)state);
        } else if (out == BTS716G_OUT_2) {
            HAL_GPIO_WritePin(DO_PULS2_GPIO_Port, DO_PULS2_Pin, (GPIO_PinState)state);
        } else if (out == BTS716G_OUT_3) {
            return PUD_ERR;
        } else if (out == BTS716G_OUT_4) {
            HAL_GPIO_WritePin(DO_CILYNDER_GPIO_Port, DO_CILYNDER_Pin, (GPIO_PinState)state);
        }
    }
    
    if (chip == BTS716G_8) {
        if (out == BTS716G_OUT_1) {
            HAL_GPIO_WritePin(DO_DRAIN_GPIO_Port, DO_DRAIN_Pin, (GPIO_PinState)state);
        } else if (out == BTS716G_OUT_2) {
            HAL_GPIO_WritePin(DO_FLOW_GPIO_Port, DO_FLOW_Pin, (GPIO_PinState)state);
        } else if (out == BTS716G_OUT_3) {
            HAL_GPIO_WritePin(DO_DEFOAMER_UP_GPIO_Port, DO_DEFOAMER_UP_Pin, (GPIO_PinState)state);
        } else if (out == BTS716G_OUT_4) {
            return PUD_ERR;
        }
    }
    
    if (chip == BTS716G_9) {
        return PUD_ERR;
    }
    
    return PUD_OK;
}

uint32_t CAN_SelfTest (void)
{
    FDCAN_FilterTypeDef   CAN_Filter;
    static uint8_t tx_data[32] = {1, 22, 33, 44, 55, 66, 77, 88};
    uint32_t start_time;
    uint16_t len;
    static uint8_t adr = 0x03;
    static uint8_t func = 0x01;
    
    CAN_Filter.IdType = FDCAN_STANDARD_ID;
    CAN_Filter.FilterIndex = 127;
    CAN_Filter.FilterType = FDCAN_FILTER_RANGE;
    CAN_Filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    CAN_Filter.FilterID1 = 0x113;
    CAN_Filter.FilterID2 = 0x115;
    CAN_Filter.RxBufferIndex = 0;
    CAN_Filter.IsCalibrationMsg = 0;
    
//    HAL_FDCAN_ConfigFilter (&hfdcan1, &CAN_Filter);
    
    
    CAN_TxHeader.Identifier = 0x111;
    CAN_TxHeader.IdType = FDCAN_EXTENDED_ID; //FDCAN_STANDARD_ID;
    CAN_TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    CAN_TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    CAN_TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    CAN_TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    CAN_TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    CAN_TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    CAN_TxHeader.MessageMarker = 0;
    
    start_time = HAL_GetTick();
    
    can_flg = 0;
    HAL_FDCAN_AddMessageToTxFifoQ (&hfdcan1, &CAN_TxHeader, &tx_data[0]);
    
    while ((can_flg == 0) && ((HAL_GetTick() - start_time) < 1000)) {}
    if ((HAL_GetTick() - start_time) >= 1000) {
        return PUD_ERR;
    }
    
    MBpack(adr, func, rx_data, 8, tx_data, (int *)&len);
    CAN_Transmit_Data (adr, func, 0, len, tx_data);

//    HAL_FDCAN_GetRxMessage (&hfdcan1, FDCAN_RX_FIFO0, &CAN_RxHeader, &rx_data[0]);
    
    return PUD_OK;
}

/**
 *********************************************************************************************
 * @brief ������� �������� ���������� �� ���� CAN
 *
 * @param [adr]  ����� ����������, �������� ���������� ���������
 * @param [func] ����� ������� �������� ��������� ModBus
 * @param [list_id] ����������������� ����� ������ ����������
 * @param [len] ����� ������ ������, ����
 * @param [data] ��������� �� ����� � �������
 *
 * @retval [] ������ ����������
 *
 * @note 
 * 
 *********************************************************************************************
*/
uint8_t CAN_Transmit_Data (uint8_t adr, uint8_t func, uint8_t list_id, uint16_t len, uint8_t *data)
{
 uint16_t size;
 uint16_t i;
 uint16_t index;
 uint32_t can_id;
 uint8_t frame_num;
 HAL_StatusTypeDef st;
 static FDCAN_TxHeaderTypeDef CAN_TxHeader;
 
    index = 0;
    frame_num = 0;
    
    CAN_TxHeader.IdType = FDCAN_EXTENDED_ID;
    CAN_TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    CAN_TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    CAN_TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    CAN_TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    CAN_TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    CAN_TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    CAN_TxHeader.MessageMarker = 0;
    
    if (len > 8) {
        size = len / 8;
        if ((len % 8) > 0) {
            size++;
        }
    } else {
        size = 1;
    }
    
    can_id = MBpackID (func, list_id, frame_num, adr);
    CAN_TxHeader.Identifier = can_id; 
    
    for (i = 0; i < size; i++)
    {
        st = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &CAN_TxHeader, (const uint8_t *)&data[index]);
        if (st != HAL_OK) {
            st = HAL_OK;
        }
        index += 8;
        if (index >= 64) {
            return PUD_ERR;
        }
    }
    return PUD_OK;
}

void HAL_FDCAN_RxFifo0Callback (FDCAN_HandleTypeDef *hfdcan,  uint32_t RxFifo0ITs)
{
    HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &CAN_RxHeader, &rx_data[0]);
    can_flg++;
}

uint8_t Start_Milking_Button (void)
{
  static uint8_t state = 0, new_state = 0;
  static uint32_t timer = 0;
  
    new_state = HAL_GPIO_ReadPin(DI_FAST_START_GPIO_Port, DI_FAST_START_Pin);
    
    if (new_state == 0) {
        state = 0;
    }
    
    if ((new_state == 1) && (state == 0)) {
        state = 1;
        timer = HAL_GetTick();
    }
    
    if ((new_state == 1) && (state == 1) && ((HAL_GetTick() - timer) >= BUTTUN_TIMEOUT)) {
        return 1;
    }
    
    return 0;
}
