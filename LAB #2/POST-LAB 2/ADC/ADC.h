/*
 * ADC.h
 *
 * Created: 24/07/2025 22:09:17
 *  Author: Luis Sactic
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <util/delay.h>

#define NUM_SAMPLES 64

void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);

#endif /* ADC_H_ */