/*
 * ADC.c
 *
 * Created: 18/07/2025 10:56:17
 *  Author: Luis Sactic
 */ 

#include "ADC.h"
#include <avr/io.h>

void ADC_Init() {
	// Configurar ADC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, prescaler 128
}

uint16_t ADC_Read(uint8_t channel) {
	// Seleccionar canal y referencia AVcc
	ADMUX = (1 << REFS0) | (channel & 0x07);
	
	// Iniciar conversión
	ADCSRA |= (1 << ADSC);
	
	// Esperar fin de conversión
	while(ADCSRA & (1 << ADSC));
	
	return ADC;
}