/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "hx711.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RELAY_PORT GPIOA
#define RELAY_PIN GPIO_PIN_5

#define KEY_PORT GPIOB
#define KEY_PIN GPIO_PIN_15

#define MENU_PORT GPIOB
#define MENU_PIN GPIO_PIN_12

#define UP_PORT GPIOB
#define UP_PIN GPIO_PIN_13

#define DOWN_PORT GPIOB
#define DOWN_PIN GPIO_PIN_14

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float grams = 0.00;
float target = 18.0;
char cur_weight[30];
int state = 0;
int grinding = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
float GetStableWeight(void) {
	float readings[5];

	// Take 5 readings
	for (int i = 0; i < 5; i++) {
		readings[i] = HX711_GetWeight();
		HAL_Delay(1); // Delay between readings
	}

	// Sort the array to find the median
	// Simple insertion sort
	for (int i = 1; i < 5; i++) {
		float key = readings[i];
		int j = i - 1;
		while (j >= 0 && readings[j] > key) {
			readings[j + 1] = readings[j];
			j = j - 1;
		}
		readings[j + 1] = key;
	}

	// Return the median value
	return readings[2]; // The middle value after sorting
}
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
  state = 0;
  int lastState = -1;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

	//init the scale
	HX711_Init();
	//init the oled
	OLED_Init();
	OLED_SetBrightness(128);
	OLED_ColorTurn(0);
	OLED_DisplayTurn(0);
//	OLED_ShowString(0,4,"Testing ",16,1);
	OLED_Refresh();
	HX711_Tare();
//	HAL_Delay(2000);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		//add case switch here
		//if button is pressed, go to next state

		char cur_weight[30];

		if(HAL_GPIO_ReadPin(MENU_PORT, MENU_PIN) == GPIO_PIN_SET){
			// Debounce delay
			HAL_Delay(100);
			while(HAL_GPIO_ReadPin(MENU_PORT, MENU_PIN) == GPIO_PIN_SET); // Wait until button is released

			state = (state + 1) % 4;
//			HAL_Delay(500);
		}
//
		if (state != lastState)
		{
			OLED_Clear();
			HX711_Tare();
			lastState = state; // Update the last state
		}

		switch(state){
		case 0:
			snprintf(cur_weight, sizeof(cur_weight), "Start %.2f g", target);
			OLED_ShowString(0, 4, cur_weight, 12, 1);
			OLED_Refresh();
			HX711_Tare();
			grams = HX711_GetWeight();
			if(grams > 100){

				HAL_Delay(500);
				if(grams > 100){
					OLED_Clear();
					snprintf(cur_weight, sizeof(cur_weight), "%.2f g", grams);
					OLED_ShowString(0, 20, (char*)cur_weight, 12, 1);
					OLED_Refresh();
					grinding = 1;
					HX711_Tare();
					HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_SET);
				}

			}
			while(grinding){
				snprintf(cur_weight, sizeof(cur_weight), "%.2f g", grams);
				OLED_ShowString(0, 20, (char*)cur_weight, 12, 1);
				OLED_Refresh();
				grams = HX711_GetWeight();
				if(grams < 0){
					break;
				}
				if(grams >= 17.9){
					HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_RESET);
					snprintf(cur_weight, sizeof(cur_weight), "Done %.2f", grams);
					OLED_ShowString(0, 20, (char*)cur_weight, 12, 1);
					OLED_Refresh();
					HAL_Delay(200);
					HX711_Tare();
					grinding = 0;
					break;
				}else{
					HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_SET);
				}
			}
			HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_RESET);
//			HAL_Delay(100);
			break;
		case 1:
			snprintf(cur_weight, sizeof(cur_weight), "Calibrate   50g", target);
//			HX711_Calibrate();
			OLED_ShowString(0, 4, cur_weight, 12, 1);
			OLED_Refresh();
//			HAL_Delay(100);
			break;
		case 2:
			OLED_ShowString(0, 4, "Change Weight", 12, 1);
			snprintf(cur_weight, sizeof(cur_weight), "Target: %.2f", target);
			OLED_ShowString(0, 20, cur_weight, 12, 1);
			OLED_Refresh();
//			HAL_Delay(100);
			if(HAL_GPIO_ReadPin(UP_PORT, UP_PIN) == GPIO_PIN_SET){
						// Debounce delay
				HAL_Delay(100);
				while(HAL_GPIO_ReadPin(UP_PORT, UP_PIN) == GPIO_PIN_SET); // Wait until button is released

				target = target + 0.1;
			}else if(HAL_GPIO_ReadPin(DOWN_PORT, DOWN_PIN) == GPIO_PIN_SET){
				// Debounce delay
				HAL_Delay(100);
				while(HAL_GPIO_ReadPin(DOWN_PORT, DOWN_PIN) == GPIO_PIN_SET); // Wait until button is released

				target = target - 0.1;
			}
			break;
		case 3:
			snprintf(cur_weight, sizeof(cur_weight), "test 50g");
			OLED_ShowString(0, 4, (char*)cur_weight, 12, 1);
			float g = GetStableWeight();
			snprintf(cur_weight, sizeof(cur_weight), "%.2f g", g);
			OLED_ShowString(0, 20, (char*)cur_weight, 12, 1);
			OLED_Refresh();

		}
//
//		switch(state){
//			case 0: //grinding
//				if (state != lastState) // Only update text when first entering the state
//				{
//					snprintf(cur_weight, sizeof(cur_weight), "Start %d g", target);
//					OLED_ShowString(0, 4, cur_weight, 12, 1);
//					OLED_Refresh();
//				}
//
//				// Continuously measure the weight of the material
//				grams = GetStableWeight();
//
//				// Check if the object is placed and within the desired weight range
//				if (grams >= 100 && grams <= 600)
//				{
//					// Tare the scale to set the current weight as zero
//					HX711_Tare();
//
//					// Update OLED to show the system is ready to grind
//					OLED_Clear();
//					snprintf(cur_weight, sizeof(cur_weight), "Ready to grind towards %d g", target);
//					OLED_ShowString(0, 4, cur_weight, 16, 1);
//					OLED_Refresh();
//
//					// Start the grinding process by activating the relay/motor
//					HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_SET);
//
//					// Loop to continuously check the weight and grind
//					while (1)
//					{
//						// Measure the weight of the material
//						grams = GetStableWeight();
//
//						// Display the current weight dynamically
//						snprintf(cur_weight, sizeof(cur_weight), "Current: %d g", grams);
//						OLED_ShowString(0, 20, cur_weight, 16, 1);
//						OLED_Refresh();
//
//						// Check if the target weight is reached or exceeded
//						if (grams >= target)
//						{
//							// Stop the grinding process by deactivating the relay/motor
//							HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_RESET);
//
//							// Update the display to indicate completion
//							OLED_ShowString(0, 36, "Grinding Complete", 16, 1);
//							OLED_Refresh();
//
//							// Break from the while loop to stop grinding
//							break;
//						}
//
//						// Insert a small delay to prevent the loop from running too fast
//						HAL_Delay(100);
//					}
//
//					// After grinding, you might want to transition to a different state or wait for further input
//					// For now, we'll just stay in the same state and wait for another object
//					state = 0;
//				}
//				else
//				{
//					// If the weight is not in the range, display a waiting message
//					OLED_Clear();
//					snprintf(cur_weight, sizeof(cur_weight), "Place object (100-600g)");
//					OLED_ShowString(0, 4, cur_weight, 16, 1);
//					OLED_Refresh();
//				}
//
//				break;
//
//
//			case 1: // Calibration
//			    if (state != lastState) // Only update text when first entering the state
//			    {
//			        // Clear the display and show instructions
//			        OLED_Clear();
//			        OLED_ShowString(0, 0, "50g", 12, 1);
//			        OLED_Refresh();
//			    }
//
//			    // Continuously check if the KEY button is pressed to start calibration
//			    if(HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == GPIO_PIN_SET)
//			    {
//			        // Debounce delay
//			        HAL_Delay(200);
//			        while(HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == GPIO_PIN_SET); // Wait until button is released
//
//			        // Perform calibration using the known weight
//			        HX711_Calibrate(); // Assuming HX711_Calibrate takes the known weight as an argument
//
//			        // Indicate calibration success on the OLED
//			        OLED_ShowString(0, 20, "Done!", 12, 1);
//			        OLED_Refresh();
//
//			        // Add a delay to allow the user to read the message
//			        HAL_Delay(2000);
//
//			        // Optionally, move to the next state or back to the initial state
//			        state = 0; // or whatever the next appropriate state is
//			    }
//			    break;
//
//
//			case 2: // Change grind weight
//			    if (state != lastState) // Only update text when first entering the state
//			    {
//			        // Clear the display and show instructions
//			        OLED_Clear();
//			        OLED_ShowString(0, 4, "Adjust", 12, 1);
//			        OLED_Refresh();
//			    }
//
//			    // Continuously read the current target weight and display it
//			    snprintf(cur_weight, sizeof(cur_weight), "%d g", target);
//			    OLED_ShowString(0, 20, cur_weight, 12, 1);
//			    OLED_Refresh();
//
//			    // Check for UP button press to increase the target weight
//			    if(HAL_GPIO_ReadPin(UP_PORT, UP_PIN) == GPIO_PIN_SET)
//			    {
//			        target += 0.1; // Adjust the increment value as necessary
//			        HAL_Delay(200); // Debounce delay
//			    }
//
//			    // Check for DOWN button press to decrease the target weight
//			    if(HAL_GPIO_ReadPin(DOWN_PORT, DOWN_PIN) == GPIO_PIN_SET)
//			    {
//			        target -= 0.1; // Adjust the decrement value as necessary
//			        HAL_Delay(200); // Debounce delay
//			    }
//
//			    // Add a small delay to prevent the loop from running too fast
//			    HAL_Delay(100);
//			    break;
//
//		}
//




		//	  OLED_Clear();
		//	  HAL_Delay(200);
		//		if(HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == GPIO_PIN_SET){
		////			grams = grams + 1; // Increase grams - make sure 'grams' is defined appropriately
		//			grams = HX711_GetWeight();
		//		}
		//running state, if more than 17.6, stop grinding and add bit by bit


		// grams = GetStableWeight();
		// if(grams > 17.6){
		// 	HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_RESET);
		// }else{
		// 	HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_SET);
		// }
		// while(!(grams > 18)){
		// 	HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_SET);
		// 	HAL_GPIO_WritePin(RELAY_PORT, RELAY_PIN, GPIO_PIN_RESET);
		// }

		// char cur_weight[30];
		// // Use snprintf for safety to avoid buffer overflow
		// //		OLED_Clear();
		// snprintf(cur_weight, sizeof(cur_weight), "%d g    ", grams);
		// //	  OLED_ShowString(0,4,"Grind Weight: ",16,1);
		// OLED_ShowString(0,20,cur_weight,12,1);
//		 OLED_Refresh();
//		 HAL_Delay(200);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
