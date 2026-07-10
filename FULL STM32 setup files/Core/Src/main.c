/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
# include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// 1602 I2C address
#define I2C_ADDR 0x27 // I2C address of the PCF8574
// 1602 dimensions
#define LCD_ROWS 2 // Number of rows on the LCD
#define LCD_COLS 16 // Number of columns on the LCD
// 1602 message bit numbers
#define DC_BIT 0 // Data/Command bit (register select bit)
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Backlight bit
#define D4_BIT 4 // Data 4 bit
#define D5_BIT 5 // Data 5 bit
#define D6_BIT 6 // Data 6 bit
#define D7_BIT 7 // Data 7 bit
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void CharLCD_Write_Nibble (uint8_t nibble, uint8_t dc);
void CharLCD_Send_Cmd(uint8_t cmd);
void CharLCD_Send_Data(uint8_t data);
void CharLCD_Init();
void CharLCD_Write_String(char *str);
void CharLCD_Set_Cursor(uint8_t row, uint8_t column);
void CharLCD_Clear(void);
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  CharLCD_Init(); // Initialize the LCD

//  CharLCD_Set_Cursor(0,0); // Set cursor to row 0, column 0
//  CharLCD_Write_String("Hello, ECEN 260!");
//  CharLCD_Set_Cursor(1,0); // Set cursor to row 1, column 0
//  CharLCD_Write_String("Jesus loves you!");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint8_t rx_buffer[33];

	  if (HAL_UART_Receive(&huart2, rx_buffer, 32, HAL_MAX_DELAY) == HAL_OK)
	    {
	      rx_buffer[32] = '\0';
	      char line1[17];
	      char line2[17];

	      strncpy(line1, (char*)rx_buffer, 16);
	      line1[16] = '\0';
	      strncpy(line2, (char*)rx_buffer + 16, 16);
	      line2[16] = '\0';

	      CharLCD_Clear();
	      CharLCD_Set_Cursor(0, 0);
	      CharLCD_Write_String(line1);
	      CharLCD_Set_Cursor(1, 0);
	      CharLCD_Write_String(line2);
	    }
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief Write a 4-bit nibble to the LCD via I2C
 * @param nibble: 4-bit data to send (lower 4 bits)
 * @param dc: data/command (1 = data, 0 = command)
 * @retval None
 */
void CharLCD_Write_Nibble(uint8_t nibble, uint8_t dc) {
 uint8_t data = nibble << D4_BIT; // Shift nibble to D4-D7 position
 data |= dc << DC_BIT; // Set DC bit for data/command selection
 data |= 1 << BL_BIT; // Include backlight state in data
 data |= 1 << EN_BIT; // Set enable bit high
 HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100); // Send data with EN high
 HAL_Delay(1); // Wait for data setup
 data &= ~(1 << EN_BIT); // Clear enable bit (falling edge triggers LCD)
 HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100); // Send data with EN low
}

/**
 * @brief Send command to LCD
 * @param cmd: 8-bit command to send to LCD controller
 * @retval None
 */
void CharLCD_Send_Cmd(uint8_t cmd) {
 uint8_t upper_nibble = cmd >> 4; // Extract upper 4 bits
 uint8_t lower_nibble = cmd & 0x0F; // Extract lower 4 bits
 CharLCD_Write_Nibble(upper_nibble, 0); // Send upper nibble (DC=0 for command)
 CharLCD_Write_Nibble(lower_nibble, 0); // Send lower nibble (DC=0 for command)
 if (cmd == 0x01 || cmd == 0x02) { // Clear display or return home commands
 HAL_Delay(2); // These commands need extra time
 }
}

/**
 * @brief Send data (character) to LCD
 * @param data: 8-bit character data to display
 * @retval None
 */
void CharLCD_Send_Data(uint8_t data) {
 uint8_t upper_nibble = data >> 4; // Extract upper 4 bits
 uint8_t lower_nibble = data & 0x0F; // Extract lower 4 bits
 CharLCD_Write_Nibble(upper_nibble, 1); // Send upper nibble (DC=1 for data)
 CharLCD_Write_Nibble(lower_nibble, 1); // Send lower nibble (DC=1 for data)
}

/**
 * @brief Initialize LCD in 4-bit mode via I2C
 * @param None
 * @retval None
 */
void CharLCD_Init() {
 HAL_Delay(50); // Wait for LCD power-on reset (>40ms)
 CharLCD_Write_Nibble(0x03, 0); // Function set: 8-bit mode (first attempt)
 HAL_Delay(5); // Wait >4.1ms
 CharLCD_Write_Nibble(0x03, 0); // Function set: 8-bit mode (second attempt)
 HAL_Delay(1); // Wait >100us
 CharLCD_Write_Nibble(0x03, 0); // Function set: 8-bit mode (third attempt)
 HAL_Delay(1); // Wait >100us
 CharLCD_Write_Nibble(0x02, 0); // Function set: switch to 4-bit mode
 CharLCD_Send_Cmd(0x28); // Function set: 4-bit, 2 lines, 5x8 font
 CharLCD_Send_Cmd(0x0C); // Display control: display on/cursor off/blink off
 CharLCD_Send_Cmd(0x06); // Entry mode: increment cursor, no shift
 CharLCD_Send_Cmd(0x01); // Clear display
 HAL_Delay(2); // Wait for clear display command
}

/**
 * @brief Write string to LCD at current cursor position
 * @param str: Pointer to null-terminated string
 * @retval None
 */
void CharLCD_Write_String(char *str) {
 while (*str) { // Loop until null terminator
 CharLCD_Send_Data(*str++); // Send each character and increment pointer
 }
}

 /**
  * @brief Set cursor position on LCD
  * @param row: Row number (0 or 1 for 2-line display)
  * @param column: Column number (0 to display width - 1)
  * @retval None
  */
 void CharLCD_Set_Cursor(uint8_t row, uint8_t column) {
  uint8_t address;
  switch (row) {
  case 0:
  address = 0x00; break; // First line starts at address 0x00
  case 1:
  address = 0x40; break; // Second line starts at address 0x40
  default:
  address = 0x00; // Default to first line for invalid row
  }
  address += column; // Add column offset
  CharLCD_Send_Cmd(0x80 | address); // Set DDRAM address command (0x80 + address)
 }

 /**
  * @brief Clear LCD display and return cursor to home position
  * @param None
  * @retval None
  */
 void CharLCD_Clear(void) {
  CharLCD_Send_Cmd(0x01); // Clear display command
  HAL_Delay(2); // Wait for command execution
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
#ifdef USE_FULL_ASSERT
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
