

/**
  ******************************************************************************
  * @file    tle7230.h
  * @author  Artur Petrosyan, Home hand-made, Ltd.
  * @date    03.06.2024
  * @brief   Определения и обявления для низкоуровневого драйвера микросхемы TLE7230.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TLE7230_H
#define __TLE7230_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Exported variables ---------------------------------------------------------*/


/* Exported defines -----------------------------------------------------------*/


/* Управление чип-селектом драйвера силовых ключей */
#define PWRSW_CS_HIGH()  HAL_GPIO_WritePin(SPI1_NSS3_GPIO_Port, SPI1_NSS3_Pin, GPIO_PIN_SET);
#define PWRSW_CS_LOW()  HAL_GPIO_WritePin(SPI1_NSS3_GPIO_Port, SPI1_NSS3_Pin, GPIO_PIN_RESET);

/* Коды команд */
enum TLE7230_CMD {
    TLE7230_CMD_DIAG  = 0,
    TLE7230_CMD_READ  = 1,
    TLE7230_CMD_RST   = 2,
    TLE7230_CMD_WRITE = 3
};

enum TLE7230_REG_ADDR {
    TLE7230_REG_MAP_ADDR = 1,
    TLE7230_REG_BOL_ADDR = 2,
    TLE7230_REG_OVL_ADDR = 3,
    TLE7230_REG_OVT_ADDR = 4,
    TLE7230_REG_SLE_ADDR = 5,
    TLE7230_REG_STA_ADDR = 6,
    TLE7230_REG_CTL_ADDR = 7
};

enum TLE7230_DIAG_TYPE {
    TLE7230_OUT_Short   = 0,
    TLE7230_OUT_Off     = 1,   
    TLE7230_OUT_Failure = 2,
    TLE7230_OUT_Normal  = 3
};

/* Private defines -----------------------------------------------------------*/
typedef union TLE7230_REG {
    uint8_t Reg;
    struct {
        unsigned Ch1 : 1;
        unsigned Ch2 : 1;
        unsigned Ch3 : 1;
        unsigned Ch4 : 1;
        unsigned Ch5 : 1;
        unsigned Ch6 : 1;
        unsigned Ch7 : 1;
        unsigned Ch8 : 1;
    } Channel;
} TLE7230_REG_t;

#pragma pack(push, 1)
typedef union TLE7230_SPI_DATA {
    uint16_t Word;
    struct {
        uint8_t Data;
        unsigned Addr : 3;
        unsigned Rsv  : 3;
        unsigned Cmd  : 2;
    } Field;
} TLE7230_SPI_DATA_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union TLE7230_SPI_DIAG {
    uint16_t Word;
    struct {
        unsigned Ch1 : 2;
        unsigned Ch2 : 2;
        unsigned Ch3 : 2;
        unsigned Ch4 : 2;
        unsigned Ch5 : 2;
        unsigned Ch6 : 2;
        unsigned Ch7 : 2;
        unsigned Ch8 : 2;
    } Diag;
} TLE7230_SPI_DIAG_t;
#pragma pack(pop)

typedef enum TLE7230_Channel {
    TLE7230_CH1 = 0x01,
    TLE7230_CH2 = 0x02,
    TLE7230_CH3 = 0x04,
    TLE7230_CH4 = 0x08,
    TLE7230_CH5 = 0x10,
    TLE7230_CH6 = 0x20,
    TLE7230_CH7 = 0x40,
    TLE7230_CH8 = 0x80
} TLE7230_Channel_t;

typedef enum TLE7230_Switch {
    TLE7230_CH_Off = 0,
    TLE7230_CH_On  = 1
} TLE7230_Switch_t;


/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint8_t (*Channel_Sw)(TLE7230_Channel_t ch, TLE7230_Switch_t sw);
    uint16_t (*ReadWrite_Reg)(uint8_t reg_addr, uint8_t cmd, uint8_t data);
} TLE7230_t;


/* Exported macro ------------------------------------------------------------*/




/* Exported functions prototypes ---------------------------------------------*/

uint8_t TLE7230_Init (void);
uint8_t TLE7230_Channel_Sw (TLE7230_Channel_t ch, TLE7230_Switch_t sw);
uint16_t TLE7230_ReadWrite_Reg (uint8_t cmd, uint8_t reg_addr, uint8_t data);


#endif /* __TLE7230_H */