/*
 * _7segmentos.c
 *
 * Created: 11/07/2025 11:32:46
 *  Author: Luis Sactic
 */ 

#include "7segmentos.h"

// Patrones para display de 7 segmentos (cátodo común)
const uint8_t segmentos[10] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111  // 9
};

void segmentos_init(void) {
	// Configurar PD0-PD6 como salidas (display)
	SEGMENT_DDR = 0b01111111;
	// Mostrar '0' al inicio
	SEGMENT_PORT = segmentos[0];
}

void mostrar_numero(uint8_t numero) {
	if (numero < 10) {
		SEGMENT_PORT = segmentos[numero];
		} else {
		// Si el número no es válido, mostrar nada (todos los segmentos apagados)
		SEGMENT_PORT = 0b00000000;
	}
}

void cuenta_regresiva(uint8_t inicio) {
	if (inicio > 9) inicio = 9; // Limitar máximo a 9
	
	for (uint8_t i = inicio; i > 0; i--) {
		mostrar_numero(i);
		_delay_ms(10000);
	}
	mostrar_numero(0); // Mostrar '0' al finalizar
}

uint8_t boton_presionado(uint8_t pin_boton) {
	if (PINC & (1 << pin_boton)) {  // Leemos HIGH cuando está presionado (pull-down)
		_delay_ms(50);             // Antirrebote
		if (PINC & (1 << pin_boton)) {
			while (PINC & (1 << pin_boton));  // Esperar a que se suelte el botón
			return 1;
		}
	}
	return 0;
}