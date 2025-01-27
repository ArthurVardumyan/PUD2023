
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
 * @brief Функция инициализации начала доения
 *
 * @param [ton] Указатель на массив с тональным сигналом
 * @param [adc_buf] Указатель на буфер оцифрованных данных
 *
 * @retval [st] Статус инициализации
 *
 * @note  
 *
 *********************************************************************************************
*/
uint8_t TON4980_Start (uint16_t *ton, uint32_t *cnt)
{
    HAL_StatusTypeDef st;
 
    // Сбрасываем счетчик надоенного молока
    *cnt = 0;
    /* Включаем генерацию сигнала 4980 Гц. Используется аппаратный NSS */
    st = HAL_SPI_Transmit_DMA(&DAC_SPI, (uint8_t *)ton, TON4980_POINT_NUM);
    /* Запускаем таймер синхронизации генератора и АЦП */
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
 * @brief Функция окончания доения
 *
 * @param [ton] Указатель на массив с тональным сигналом
 * @param [adc_buf] Указатель на буфер оцифрованных данных
 *
 * @retval [st] Статус инициализации
 *
 * @note  
 *
 *********************************************************************************************
*/
uint8_t TON4980_Stop (void)
{
    HAL_StatusTypeDef st;
    
    /* Останавливаем таймер синхронизации генератора и АЦП */
    st = HAL_TIM_Base_Stop(&htim12);
    st = HAL_TIM_Base_Stop(&htim16);

    if (st != HAL_OK) {
        return PUD_ERR;
    }
    
    return PUD_OK;
}

/**
 *********************************************************************************************
 * @brief Функция обработки оцифрованного сигнала проточного датчика молока
 *
 * @param [data] Указатель на массив с данными
 * @param [res] Указатель на результат обработки
 *
 * @retval [] None
 *
 * @note 
 * 
 *********************************************************************************************
*/
void MilkSensor_Process (uint8_t *adc_buf, uint32_t *cnt)
{
  uint32_t data, i;
  
    data = 0;
    for (i = 0; i < ADC_BUF_SIZE; i += 3) {
        data += (adc_data[i+1] << 8 | adc_data[i+2]) & 0x00000FFF;
    }
    data /= (ADC_BUF_SIZE / 3);
    // надо считать разницу между электродами.
    // Если уровень превысил порог, значит электрод покрылся молоком
    if (data >= MILK_SENSOR_TRS) {
        *cnt++;
    }
}

/**
 *********************************************************************************************
 * @brief Функция управления ключом BTS716G
 *
 * @param [chip] Номер чипа в схеме. Может быть 7, 8 и 9.
 * @param [cmd]  Команда на вкл/выкл - BTS716G_ON/BTS716G_OFF
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
 * @brief Функция передачи параметров по шине CAN
 *
 * @param [adr]  Адрес устройства, которому передается сообщение
 * @param [func] Номер функции согласно протоколу ModBus
 * @param [list_id] Идентификационный номер списка параметров
 * @param [len] Длина буфера данных, байт
 * @param [data] Указатель на буфер с данными
 *
 * @retval [] Статус выполнения
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
        HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &CAN_TxHeader, (const uint8_t *)&data[index]);
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
