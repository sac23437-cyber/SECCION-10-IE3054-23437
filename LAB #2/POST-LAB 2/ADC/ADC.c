/*
 * ADC.c
 *
 * Created: 24/07/2025 22:09:31
 *  Author: Luis Sactic
 */ 

#include "ADC.h"

void ADC_Init(void) {
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX = (1 << REFS0);
}

uint16_t ADC_Read(uint8_t channel) {
	uint32_t sum = 0;
	
	for(uint8_t i = 0; i < NUM_SAMPLES; i++) {
		ADMUX = (1 << REFS0) | (channel & 0x07);
		ADCSRA |= (1 << ADSC);
		while(ADCSRA & (1 << ADSC));
		sum += ADC;
		_delay_us(50);
	}
	
	return sum / NUM_SAMPLES;
}