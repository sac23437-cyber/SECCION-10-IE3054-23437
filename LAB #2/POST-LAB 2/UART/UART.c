/*
 * UART.c
 *
 * Created: 24/07/2025 22:10:06
 *  Author: Luis Sactic
 */ 

#include "UART.h"

// Variables globales para comunicación con main
volatile uint8_t serial_data_ready = 0;
volatile char serial_command = 0;

void UART_Init(uint16_t ubrr) {
	// Configurar baud rate
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	
	// Habilitar receptor y transmisor + interrupción de recepción
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	
	// Configurar formato: 8 bits de datos, 1 bit de parada
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void UART_Transmit(char data) {
	// Esperar hasta que el buffer de transmisión esté vacío
	while(!(UCSR0A & (1<<UDRE0)));
	// Poner el dato en el buffer
	UDR0 = data;
}

char UART_Receive(void) {
	// Esperar hasta que exista un dato disponible
	while(!(UCSR0A & (1<<RXC0)));
	// Obtener y devolver el dato recibido
	return UDR0;
}

// Interrupción de recepción serial
ISR(USART_RX_vect) {
	serial_command = UDR0;
	serial_data_ready = 1;
}