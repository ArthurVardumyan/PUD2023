

/**
  ******************************************************************************
  * @file    tle7230.c
  * @author  Artur Petrosyan, Home hand-made, Ltd.
  * @date    03.06.2024
  * @brief   �������������� ������� ���������� TLE7230.
  *
  */

#include "main.h"
#include "spi.h"
#include "pud.h"
#include "tle7230.h"
      
static uint8_t Channel_On_Off (TLE7230_Channel_t ch, TLE7230_Switch_t sw);
static uint16_t rwReg (uint8_t reg_addr, uint8_t cmd, uint8_t data);

/**
 *********************************************************************************************
 * @brief ������� ������/������ �������� TLE7230
 *
 * @param [reg_addr] ����� �������� TLE7230_REG_ADDR
 * @param [cmd]  ��� �������
 * @param [data] ������
 *
 * @retval [spi_data.Word] ��������� ������ ��������. ��� PUD_ERR � ������ ������ ����������
 *
 * @note  ��������� ������� ��� �������� ������ �� ������ ����� ���������� ���, ��� � ����������
 *        � ���� spi_data.Field.Cmd ����� ������� ������ TLE7230_CMD_READ
 *
 *********************************************************************************************
*/
static uint16_t rwReg (uint8_t cmd, uint8_t reg_addr, uint8_t data)
{
    TLE7230_SPI_DATA_t spi_data;
    HAL_StatusTypeDef st;
    
    spi_data.Field.Addr = reg_addr;
    spi_data.Field.Cmd = cmd;
    spi_data.Field.Data = data;
    
    /* ���������� ������� */
    PWRSW_CS_LOW();
    st = HAL_SPI_Transmit(&TLE7230_SPI, (const uint8_t *)&spi_data, 1, 50);
    PWRSW_CS_HIGH();
    HAL_Delay(1);
    if (st != HAL_OK) {
        return PUD_ERR;
    }
    
    if ((cmd == TLE7230_CMD_WRITE) || (cmd == TLE7230_CMD_RST))
        return PUD_OK;
    
    /* ������ ������� */
    PWRSW_CS_LOW();
    st = HAL_SPI_Receive(&TLE7230_SPI, (uint8_t *)&spi_data, 1, 50);
    PWRSW_CS_HIGH();
    if (st != HAL_OK) {
        return PUD_ERR;
    }
    return spi_data.Word;
}

/**
 *********************************************************************************************
 * @brief ������� ���/���� TLE7230
 *
 * @param [reg_addr] ����� �������� TLE7230_REG_ADDR
 * @param [ch] ����� ������
 *
 * @retval [st] ������ ���������� �������: PUD_OK - �������; PUD_ERR - ���������.
 *
 * @note  
 *
 *********************************************************************************************
*/
static uint8_t Channel_On_Off (TLE7230_Channel_t ch, TLE7230_Switch_t sw)
{
    TLE7230_SPI_DATA_t spi_data;
    TLE7230_REG_t tle7230;
    HAL_StatusTypeDef st;
    
    /* ������� ������ ��������� �������. ������ ������� STA */
    spi_data = (TLE7230_SPI_DATA_t)rwReg(TLE7230_REG_STA_ADDR, TLE7230_CMD_READ, 0);
    tle7230.Reg = spi_data.Field.Data;
    
    /* �������� ����� ��� ��������� ����� */
    if (sw == TLE7230_CH_On) {
        tle7230.Reg |= ch;
    } else if (sw == TLE7230_CH_Off) {
        tle7230.Reg &= ~ch;
    }
    spi_data.Field.Data = tle7230.Reg;
    spi_data.Field.Addr = TLE7230_REG_CTL_ADDR;
    spi_data.Field.Cmd = TLE7230_CMD_WRITE;
    HAL_Delay(1);
    PWRSW_CS_LOW();
    st = HAL_SPI_Transmit(&TLE7230_SPI, (const uint8_t *)&spi_data, 1, 50);
    PWRSW_CS_HIGH();
    if (st != HAL_OK) {
        return PUD_ERR;
    }
    return PUD_OK;
}

/**
 *********************************************************************************************
 * @brief ���������� ������� ���/���� ������� TLE7230
 *
 * @param [ch] ����� ������ TLE7230_Channel_t
 * @param [sw] ��������� �������� TLE7230_Switch_t TLE7230_CH_On/TLE7230_CH_Off
 *
 * @retval [st] ������ ���������� �������: PUD_OK - �������; PUD_ERR - ���������.
 *
 * @note  
 *
 *********************************************************************************************
*/
uint8_t TLE7230_Channel_Sw (TLE7230_Channel_t ch, TLE7230_Switch_t sw)
{
    return (Channel_On_Off (ch, sw));
}

/**
 *********************************************************************************************
 * @brief ���������� ������� ������/������ �������� TLE7230
 *
 * @param [reg_addr] ����� �������� TLE7230_REG_ADDR
 * @param [cmd]  ��� �������
 * @param [data] ������
 *
 * @retval [spi_data.Word] ��������� ������ ��������. ��� PUD_ERR � ������ ������ ����������
 *
 * @note  ��� �������� ������ ��������� ������� �� ������ ����� ���������� ���, ��� � ����������
 *        � ���� spi_data.Field.Cmd ����� ������� ������ TLE7230_CMD_READ
 *
 *********************************************************************************************
*/
uint16_t TLE7230_ReadWrite_Reg (uint8_t reg_addr, uint8_t cmd, uint8_t data)
{
    return (rwReg(reg_addr, cmd, data));
}

/**
 *********************************************************************************************
 * @brief ���������� ������� ������������� TLE7230
 *
 * @param [c] None
 *
 * @retval [st] ������ ���������� �������: PUD_OK - �������; PUD_ERR - ���������.
 *
 * @note  
 *
 *********************************************************************************************
*/
uint8_t TLE7230_Init (void)
{
    uint16_t st;
    
    /* ��������� ��� IN �� ���������� �������� */
    st = rwReg(TLE7230_CMD_WRITE, TLE7230_REG_MAP_ADDR, 0);
    /* ��������� ��� ������ */
    st |= rwReg(TLE7230_CMD_WRITE, TLE7230_REG_CTL_ADDR, 0);
    
    if (st != HAL_OK) {
        return PUD_ERR;
    }
    return PUD_OK;
}