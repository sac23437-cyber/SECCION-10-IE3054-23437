/*
 * UART.h
 *
 * Created: 24/07/2025 22:09:55
 * Author: Luis Sactic
 */ 

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

// Variables globales para comunicación con main
extern volatile uint8_t serial_data_ready;
extern volatile char serial_command;

// Configuración básica (tu estilo)
void UART_Init(uint16_t ubrr);
void UART_Transmit(char data);
char UART_Receive(void);

// Funciones adicionales 
void UART_SendString(const char* str);
void UART_SendNumber(uint16_t num);
bool UART_Available(void);

#endif /* UART_H_ */