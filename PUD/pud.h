
/**
  ******************************************************************************
  * @file           : pud.h
  * @brief          : Header file for PUD project.
  *                   This file contains the common defines of the application.
  * @author         : Artur Petrosyan, Home hand-made, Ltd.
  *
  * @date           : 10/05/2024
  * @date           : 
  *
  * @note 
  *********************************************************************************************
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PUD_H
#define __PUD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Exported defines -----------------------------------------------------------*/

/* Размер буфера оцифрованного сигнала проточного датчика молока */
#define ADC_BUF_SIZE    512*3
/* Число отсчетов для генерации тона 4980 Гц */
#define TON4980_POINT_NUM  18
/* Пороговый уровень сигнала проточного датчика */
#define MILK_SENSOR_TRS 1000
/* Таймаут дойки, мс */
#define PROCESS_TIMEOUT 5000
#define DAC_SPI         hspi3
#define ADC_SPI         hspi4
#define TLE7230_SPI     hspi1

/* Ключи BTS716G */
typedef enum {
    BTS716G_OFF = 0,
    BTS716G_ON  = 1
} BTS716G_STATE;

typedef enum {
    BTS716G_7 = 7,
    BTS716G_8 = 8,
    BTS716G_9 = 9
} BTS716G_CHIP;

typedef enum {
    BTS716G_OUT_1 = 1,
    BTS716G_OUT_2 = 2,
    BTS716G_OUT_3 = 3,
    BTS716G_OUT_4 = 4
} BTS716G_OUT;
          

/* Exported variables ---------------------------------------------------------*/
extern uint16_t TON4980[TON4980_POINT_NUM];
extern uint8_t adc_data[ADC_BUF_SIZE];


/* Переключение фазы выходного сигнала проточного датчика */
#define TOGGLE_SW4()  HAL_GPIO_TogglePin(CS_SW4_GPIO_Port, CS_SW4_Pin);
#define TOGGLE_SW1()  HAL_GPIO_TogglePin(CS_SW1_GPIO_Port, CS_SW1_Pin);
#define TOGGLE_SW2()  HAL_GPIO_TogglePin(CS_SW2_GPIO_Port, CS_SW2_Pin);
#define ELEC_SW_STATE() HAL_GPIO_ReadPin(CS_SW1_GPIO_Port, CS_SW1_Pin);
#define TOGGLE_SW3()  HAL_GPIO_TogglePin(CS_SW3_GPIO_Port, CS_SW3_Pin);
#define CS_SW1_EN()  HAL_GPIO_WritePin(CS_SW1_GPIO_Port, CS_SW1_Pin, GPIO_PIN_SET);
#define CS_SW1_DIS()  HAL_GPIO_WritePin(CS_SW1_GPIO_Port, CS_SW1_Pin, GPIO_PIN_RESET);
#define CS_SW2_EN()  HAL_GPIO_WritePin(CS_SW2_GPIO_Port, CS_SW2_Pin, GPIO_PIN_SET);
#define CS_SW2_DIS()  HAL_GPIO_WritePin(CS_SW2_GPIO_Port, CS_SW2_Pin, GPIO_PIN_RESET);
#define CS_SW3_EN()   HAL_GPIO_WritePin(CS_SW3_GPIO_Port, CS_SW3_Pin, GPIO_PIN_SET);
#define CS_SW3_DIS()  HAL_GPIO_WritePin(CS_SW3_GPIO_Port, CS_SW3_Pin, GPIO_PIN_RESET);
#define CS_SW4_EN()   HAL_GPIO_WritePin(CS_SW4_GPIO_Port, CS_SW4_Pin, GPIO_PIN_SET);
/* Ключ управления аудиовыходом - АО1 или АО2 */
#define AO2_EN()     HAL_GPIO_WritePin(CS_SW1_GPIO_Port, CS_SW1_Pin, GPIO_PIN_SET);
#define AO2_DIS()    HAL_GPIO_WritePin(CS_SW1_GPIO_Port, CS_SW1_Pin, GPIO_PIN_RESET);




/* Exported types ------------------------------------------------------------*/




/* Exported constants --------------------------------------------------------*/
enum err_code {
    PUD_OK = 0,
    PUD_ERR = 0xFF
};


/* Exported macro ------------------------------------------------------------*/




/* Exported functions prototypes ---------------------------------------------*/
uint8_t PUD_Init (void);
uint8_t TON4980_Start (uint16_t *ton, uint32_t *cnt);
uint8_t TON4980_Stop (void);
uint8_t BTS716G (BTS716G_CHIP chip,  BTS716G_OUT out, BTS716G_STATE state);
void MilkSensor_Process (uint8_t *adc_buf, uint32_t *cnt);
uint8_t CAN_Transmit_Data (uint8_t adr, uint8_t func, uint8_t list_id, uint16_t len, uint8_t *data);
uint32_t CAN_SelfTest (void);


/* Private defines -----------------------------------------------------------*/


#endif /* __PUD_H */