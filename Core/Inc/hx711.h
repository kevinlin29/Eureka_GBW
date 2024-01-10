/*
 * hx711.h
 *
 *  Created on: Dec 29, 2023
 *      Author: kevin
 */

#ifndef INC_HX711_H_
#define INC_HX711_H_

#include "stm32f4xx_hal.h"

// Define pins and ports for HX711
#define HX711_SCK_PORT GPIOA
#define HX711_SCK_PIN GPIO_PIN_4
#define HX711_DT_PORT GPIOA
#define HX711_DT_PIN GPIO_PIN_3

// Define pin and port for the calibration key
#define KEY_PORT GPIOA
#define KEY_PIN GPIO_PIN_1


void HX711_Init(void);
uint32_t HX711_Read(void);
void HX711_Tare(void);
float HX711_GetWeight(void);
void HX711_Calibrate(void);

#endif /* INC_HX711_H_ */
