/* USER CODE BEGIN Header */
/**
  *********************************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @author         : Artur Petrosyan
  * @date           : 10/04/2024
  * @date           : 
  *
  * @note
  *
  * Программа системы ILS
  *
  *********************************************************************************************
  */
      
/**
  *********************************************************************************************
  * @History
  * @date     :
  *
  *********************************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fdcan.h"
#include "i2c.h"
#include "memorymap.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pud.h"
#include "tle7230.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DMA_BUFFER uint16_t TON4980[TON4980_POINT_NUM];
//static const uint8_t adc_tx_data[3] = {0b00000110, 0b11000000, 0};
static const uint8_t adc_tx_data[3] = {0b00000111, 0b00000000, 0};
DMA_BUFFER uint8_t adc_data[ADC_BUF_SIZE];
uint32_t milk_counter;
uint32_t ton[TON4980_POINT_NUM];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
static void TON4980_Calc (uint16_t *dest, uint16_t point_num);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_UART7_Init();
  MX_UART8_Init();
  MX_USART2_UART_Init();
  MX_SPI4_Init();
  MX_I2C1_Init();
  MX_TIM16_Init();
  MX_FDCAN1_Init();
  MX_TIM12_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  TON4980_Calc(TON4980, TON4980_POINT_NUM);
  PUD_Init();
  TLE7230_Init();
  BTS716G(BTS716G_7, BTS716G_OUT_1, BTS716G_OFF);
  BTS716G(BTS716G_7, BTS716G_OUT_2, BTS716G_OFF);
  BTS716G(BTS716G_7, BTS716G_OUT_4, BTS716G_OFF);
  BTS716G(BTS716G_8, BTS716G_OUT_1, BTS716G_OFF);
  BTS716G(BTS716G_8, BTS716G_OUT_2, BTS716G_OFF);
  BTS716G(BTS716G_8, BTS716G_OUT_3, BTS716G_OFF);
  
  HAL_FDCAN_ConfigInterruptLines (&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0);
  HAL_FDCAN_ActivateNotification (&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  
  HAL_FDCAN_Start(&hfdcan1);
  CAN_SelfTest();
  
  AO2_EN();
  TON4980_Start(TON4980, &milk_counter);
  
  CS_SW3_EN();
  
  // Включаем второй зонд. Первый выключаем.
//  CS_SW1_DIS();
//  CS_SW2_EN();
  
  // Включаем первый зонд. Второй выключаем.
  CS_SW1_EN();
  CS_SW2_DIS();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_GPIO_TogglePin(LED_ISPRAVNOST_GPIO_Port, LED_ISPRAVNOST_Pin);
      HAL_Delay(300);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_SPI4;
  PeriphClkInitStruct.PLL3.PLL3M = 2;
  PeriphClkInitStruct.PLL3.PLL3N = 12;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_PLL3;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
 *********************************************************************************************
 * @brief Callback функция. Переключает ключ в нуле каждой полуволны 4980 Гц
 *
 * @param [] - None
 * @retval [] - None
 *
 * @note
 *
 *********************************************************************************************
*/
void HAL_SPI_TxHalfCpltCallback (SPI_HandleTypeDef *hspi) 
{
    /* Переключаем ключ в нуле каждой полуволны 4980 Гц */
    if (hspi->Instance == SPI3) {
        TOGGLE_SW4();
    }
}

/**
 *********************************************************************************************
 * @brief Callback функция. После оцифровки ADC_BUF_SIZE сигнала проточного датчика молока
 *        вызывает обработчик сигнала. 
 *
 * @param [] - None
 * @retval [] - None
 *
 * @note
 *
 *********************************************************************************************
*/
void HAL_SPI_TxCpltCallback (SPI_HandleTypeDef *hspi) 
{
    /* Переключаем ключ в нуле каждой полуволны 4980 Гц */
    if (hspi->Instance == SPI3) {
        TOGGLE_SW4();
    }
}

/**
 *********************************************************************************************
 * @brief Callback функция. После оцифровки ADC_BUF_SIZE сигнала проточного датчика молока
 *        вызывает обработчик сигнала. 
 *
 * @param [] - None
 * @retval [] - None
 *
 * @note
 *
 *********************************************************************************************
*/
void HAL_SPI_RxCpltCallback (SPI_HandleTypeDef *hspi) 
{
    
    if (hspi->Instance == SPI4) {
        
    }
}

void HAL_SPI_TxRxCpltCallback (SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI4) {
        /* Здесь включаем модуль SPI4, т.к. HAL его выключает после приема/передачи, а мы используем аппаратный NSS */
      __HAL_SPI_ENABLE(&ADC_SPI);
    }
}

/**
 *********************************************************************************************
 * @brief Callback функция. Запускает преобразование внешнего АЦП
 *
 * @param [] - None
 * @retval [] - None
 *
 * @note
 *
 *********************************************************************************************
*/
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
    static uint32_t i = 0;
    static uint32_t j = 0;
    HAL_StatusTypeDef st;
    
    // Петриод переключения электродов датчика. Равен EPERIOD * 0.01s. Где 0.01s период тамера TIM16
    const uint32_t EPERIOD = 100;
    
    if (htim->Instance == TIM16) {
        /* Включаем АЦП. Используется аппаратный NSS */
        if (i > (ADC_BUF_SIZE - 3)) i = 0;
        st = HAL_SPI_TransmitReceive_DMA(&ADC_SPI, (uint8_t *)&adc_tx_data[0], (uint8_t *)&adc_data[i], 3);
        if (st == HAL_OK) {
            i += 3;
        }
        
        if (j >= EPERIOD) {
//            TOGGLE_SW1();
//            TOGGLE_SW2();
            /* Обработка оцифрованного сигнала проточного дататчика молока */
            MilkSensor_Process(adc_data, &milk_counter);
            j = 0;
        } else {
            j++;
        }
    } else if (htim->Instance == TIM7) {
//        TOGGLE_SW4();
    }
}

/**
 *********************************************************************************************
 * @brief Сервисная функция генерации отсчетов тонального сигнала частотой 4980 Гц
 *
 * @param [dest]      Указатель на массив для сохранения результата
 * @param [point_num] Требуемое число отсчетов на период
 *
 * @retval [] None
 *
 * @note 
 * 
 *********************************************************************************************
*/
static void TON4980_Calc (uint16_t *dest, uint16_t point_num)
{
  /* Это расчет отсчетов тонального сигнала маркерного приемника. В окончательном варианте удалить */
  float k;
  const uint16_t Am = 16384;
  const float Ph = 0.4f;
  uint32_t t;
  
  k = (2* 3.1415926535897932384626433832795f) / point_num;
  for (t = 0; t < point_num; t++) {
      dest[t] = (uint16_t)((Am-1)*(sin(k * t + Ph)) + Am);
  }
} 
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
