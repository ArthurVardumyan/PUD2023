/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* Определяем DMA буфер в области памяти AXI SRAM, т.к. DMA контроллер не имеет доступа к другой области */
#if defined( __ICCARM__ )
  #define DMA_BUFFER \
      _Pragma("location=\".dma_buffer\"")
#else
  #define DMA_BUFFER \
      __attribute__((section(".dma_buffer")))
#endif
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI4_SCK_AI_Pin GPIO_PIN_2
#define SPI4_SCK_AI_GPIO_Port GPIOE
#define FPGA_D3_Pin GPIO_PIN_3
#define FPGA_D3_GPIO_Port GPIOE
#define SPI4_MOSI_AI_Pin GPIO_PIN_6
#define SPI4_MOSI_AI_GPIO_Port GPIOE
#define FPGA_D4_Pin GPIO_PIN_13
#define FPGA_D4_GPIO_Port GPIOC
#define FPGA_D5_Pin GPIO_PIN_14
#define FPGA_D5_GPIO_Port GPIOC
#define FPGA_D6_Pin GPIO_PIN_15
#define FPGA_D6_GPIO_Port GPIOC
#define FPGA_D7_Pin GPIO_PIN_0
#define FPGA_D7_GPIO_Port GPIOC
#define FPGA_A0_Pin GPIO_PIN_2
#define FPGA_A0_GPIO_Port GPIOC
#define FPGA_A1_Pin GPIO_PIN_3
#define FPGA_A1_GPIO_Port GPIOC
#define UART4_TX_ANLZ_Pin GPIO_PIN_0
#define UART4_TX_ANLZ_GPIO_Port GPIOA
#define USART2_RX_RESERV_Pin GPIO_PIN_3
#define USART2_RX_RESERV_GPIO_Port GPIOA
#define FPGA_A3_Pin GPIO_PIN_0
#define FPGA_A3_GPIO_Port GPIOB
#define FPGA_A4_Pin GPIO_PIN_1
#define FPGA_A4_GPIO_Port GPIOB
#define SPI3_MOSI_DAC_Pin GPIO_PIN_2
#define SPI3_MOSI_DAC_GPIO_Port GPIOB
#define UART7_RX_WIFI_Pin GPIO_PIN_7
#define UART7_RX_WIFI_GPIO_Port GPIOE
#define UART7_TX_WIFI_Pin GPIO_PIN_8
#define UART7_TX_WIFI_GPIO_Port GPIOE
#define FPGA_A5_Pin GPIO_PIN_9
#define FPGA_A5_GPIO_Port GPIOE
#define DI_INDUCT_DET1_Pin GPIO_PIN_10
#define DI_INDUCT_DET1_GPIO_Port GPIOE
#define DI_MAGN_DET2_Pin GPIO_PIN_12
#define DI_MAGN_DET2_GPIO_Port GPIOE
#define DI_FAST_START_Pin GPIO_PIN_13
#define DI_FAST_START_GPIO_Port GPIOE
#define SPI1_NSS3_Pin GPIO_PIN_14
#define SPI1_NSS3_GPIO_Port GPIOE
#define SPI1_NSS_ROM_Pin GPIO_PIN_15
#define SPI1_NSS_ROM_GPIO_Port GPIOE
#define LED_SERVICE_MODE_Pin GPIO_PIN_10
#define LED_SERVICE_MODE_GPIO_Port GPIOB
#define SPI1_NSS_EXPANDER_Pin GPIO_PIN_8
#define SPI1_NSS_EXPANDER_GPIO_Port GPIOD
#define LED_ISPRAVNOST_Pin GPIO_PIN_9
#define LED_ISPRAVNOST_GPIO_Port GPIOD
#define DI_KICK_OFF_Pin GPIO_PIN_10
#define DI_KICK_OFF_GPIO_Port GPIOD
#define CS_SW1_Pin GPIO_PIN_11
#define CS_SW1_GPIO_Port GPIOD
#define CS_SW2_Pin GPIO_PIN_12
#define CS_SW2_GPIO_Port GPIOD
#define CS_SW3_Pin GPIO_PIN_13
#define CS_SW3_GPIO_Port GPIOD
#define CS_SW4_Pin GPIO_PIN_14
#define CS_SW4_GPIO_Port GPIOD
#define DO_DEFOAMER_UP_Pin GPIO_PIN_15
#define DO_DEFOAMER_UP_GPIO_Port GPIOD
#define DO_FLOW_Pin GPIO_PIN_6
#define DO_FLOW_GPIO_Port GPIOC
#define DO_DRAIN_Pin GPIO_PIN_7
#define DO_DRAIN_GPIO_Port GPIOC
#define UART5_DE_RFID_Pin GPIO_PIN_8
#define UART5_DE_RFID_GPIO_Port GPIOC
#define DO_CILYNDER_Pin GPIO_PIN_9
#define DO_CILYNDER_GPIO_Port GPIOC
#define DO_PULS2_Pin GPIO_PIN_8
#define DO_PULS2_GPIO_Port GPIOA
#define DO_PULS1_Pin GPIO_PIN_9
#define DO_PULS1_GPIO_Port GPIOA
#define DI_INDUCT_DET2_Pin GPIO_PIN_10
#define DI_INDUCT_DET2_GPIO_Port GPIOA
#define SWDIO_CAN_EN_Pin GPIO_PIN_13
#define SWDIO_CAN_EN_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SW5_ENTER_Pin GPIO_PIN_15
#define SW5_ENTER_GPIO_Port GPIOA
#define SPI3_SCK_DAC_Pin GPIO_PIN_10
#define SPI3_SCK_DAC_GPIO_Port GPIOC
#define UART5_TX_RFID_Pin GPIO_PIN_12
#define UART5_TX_RFID_GPIO_Port GPIOC
#define UART4_RX_ANLZ_Pin GPIO_PIN_0
#define UART4_RX_ANLZ_GPIO_Port GPIOD
#define SW4_RIGHT_Pin GPIO_PIN_1
#define SW4_RIGHT_GPIO_Port GPIOD
#define UART5_RX_RFID_Pin GPIO_PIN_2
#define UART5_RX_RFID_GPIO_Port GPIOD
#define SW3_LEFT_Pin GPIO_PIN_3
#define SW3_LEFT_GPIO_Port GPIOD
#define SW2_DOWN_Pin GPIO_PIN_4
#define SW2_DOWN_GPIO_Port GPIOD
#define UART2_TX_REZERV_Pin GPIO_PIN_5
#define UART2_TX_REZERV_GPIO_Port GPIOD
#define SW1_UP_Pin GPIO_PIN_6
#define SW1_UP_GPIO_Port GPIOD
#define FPGA_RD_Pin GPIO_PIN_3
#define FPGA_RD_GPIO_Port GPIOB
#define FPGA_WR_Pin GPIO_PIN_4
#define FPGA_WR_GPIO_Port GPIOB
#define FPGA_D0_Pin GPIO_PIN_5
#define FPGA_D0_GPIO_Port GPIOB
#define FPGA_D1_Pin GPIO_PIN_6
#define FPGA_D1_GPIO_Port GPIOB
#define FPGA_D2_Pin GPIO_PIN_9
#define FPGA_D2_GPIO_Port GPIOB
#define UART8_RX_PROT_Pin GPIO_PIN_0
#define UART8_RX_PROT_GPIO_Port GPIOE
#define UART8_TX_PROT_Pin GPIO_PIN_1
#define UART8_TX_PROT_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
