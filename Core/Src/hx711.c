/*
 * hx711.c
 *
 *  Created on: Dec 29, 2023
 *      Author: kevin
 */

#include "hx711.h"
#include "stm32f4xx_hal.h"

static volatile float calibration_factor = 2096  ;
static volatile uint32_t tare_value = 0;

void HX711_Init(void){
	 GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE(); // Enable clock for GPIOA

	// SCK Pin configuration
	GPIO_InitStruct.Pin = HX711_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(HX711_SCK_PORT, &GPIO_InitStruct);

	// DT Pin configuration
	GPIO_InitStruct.Pin = HX711_DT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(HX711_DT_PORT, &GPIO_InitStruct);

	//make sure that the hx711 is in a known state
	HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
	HAL_Delay(1); // 1 ms delay
	HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);
}

uint32_t HX711_Read(void) {
	//setting to gain of 32 to get 80hz reading for a quicker response
    uint32_t count = 0;
    uint8_t i;

    // Wait for the HX711 to become ready
    while(HAL_GPIO_ReadPin(HX711_DT_PORT, HX711_DT_PIN) == GPIO_PIN_SET);

    // Pulse the clock pin 24 times to read the data
    for(i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
        count = count << 1;
        HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);

        if(HAL_GPIO_ReadPin(HX711_DT_PORT, HX711_DT_PIN) == GPIO_PIN_SET) {
            count++;
        }
    }

    // The 25th pulse, keeping SCK high for gain of 32
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET);
    // The 26th pulse is completed to set the gain to 32 for the next measurement
    HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET);

    // XOR the count with 0x800000 as the HX711 output is in 2's complement form
    count ^= 0x800000;

    return count;
}

float HX711_GetWeight(void) {
    uint32_t raw_value = HX711_Read();
    uint32_t tare_offset = raw_value - tare_value;
    float weight = (float)tare_offset / calibration_factor;
    if(weight > 1000000) weight = 0;
    return weight;
}

void HX711_Tare(void) {
    uint32_t sum = 0;
    const int samples = 10;
    uint32_t reading;

    // Take several readings to average
    for (int i = 0; i < samples; i++) {
        reading = HX711_Read();
        sum += reading;
        HAL_Delay(10); // Delay between readings for stability
    }

    tare_value = sum / samples; // Average tare value
}

void HX711_Calibrate(void) {
    HX711_Tare(); // Reset the scale to zero
    while(HX711_Read() - tare_value < 5); // Wait for significant change
    uint32_t calibration_raw = HX711_Read() - tare_value; // Get a raw reading with the known weight
    calibration_factor = (float)calibration_raw / 50.0f; // Calibrate with 50g
}


