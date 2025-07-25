/*
 * LAB 2.c
 *
 * Created: 18/07/2025 10:49:30
 * Author : Luis Sactic
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD/LCD.h"
#include "ADC/ADC.h"

void displayVoltage(uint16_t adc_value, uint8_t x, uint8_t y);

int main(void) {
	// 1. Configuración inicial
	DDRB |= (1 << PB0); // Backlight
	PORTB |= (1 << PB0); // Encender backlight
	
	// 2. Inicializar LCD y ADC
	LCD_Init();
	_delay_ms(100);
	LCD_Clear();
	ADC_Init();
	
	// 3. Mostrar valores iniciales
	LCD_SetCursor(0, 0);
	LCD_String("S1:    S1:     S2:");
	
	while(1) {
		// Leer y mostrar valores
		uint16_t val1 = ADC_Read(6);
		uint16_t val2 = ADC_Read(7);
		
		// Mostrar valores formateados
		displayVoltage(val1, 0, 1);
		displayVoltage(val2, 8, 1);
		
		_delay_ms(200);
	}
}

void displayVoltage(uint16_t adc_value, uint8_t x, uint8_t y) {
	char buffer[10];
	
	// Limpiar área de visualización primero
	LCD_SetCursor(x, y);
	LCD_String("    "); // 4 espacios para "X.XXV"
	
	// Verificar valor válido
	if(adc_value > 1023) {
		LCD_SetCursor(x, y);
		LCD_String("ERR");
		return;
	}
	
	// Convertir y formatear
	float voltage = (adc_value * 5.0) / 1023.0;
	dtostrf(voltage, 4, 2, buffer); // Formato: 4 caracteres, 2 decimales
	
	// Asegurar formato correcto
	if(voltage < 10.0) {
		buffer[4] = 'V';
		buffer[5] = '\0';
	}
	
	LCD_SetCursor(x, y);
	LCD_String(buffer);
}