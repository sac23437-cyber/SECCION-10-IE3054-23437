/*
 * UART.c
 *
 * Created: 24/07/2025 22:10:06
 * Author: Luis Sactic
 */ 

#define F_CPU 16000000UL

#include "UART.h"
#include <stdlib.h> // Para itoa()

// Variables globales 
volatile uint8_t serial_data_ready = 0;
volatile char serial_command = 0;

// Inicializaci�n 
void UART_Init(uint16_t ubrr) {
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // + Interrupci�n RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits, 1 stop bit
}

// Funci�n de transmisi�n 
void UART_Transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Funci�n de recepci�n 
char UART_Receive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

// Funciones adicionales 
void UART_SendString(const char* str) {
    while (*str) UART_Transmit(*str++);
}

bool UART_Available(void) {
    return (UCSR0A & (1 << RXC0));
}

void UART_SendNumber(uint16_t num) {
    char buffer[6];
    itoa(num, buffer, 10); // Conversi�n a string
    UART_SendString(buffer);
}

// Interrupci�n RX 
ISR(USART_RX_vect) {
    serial_command = UDR0;
    serial_data_ready = 1;
}