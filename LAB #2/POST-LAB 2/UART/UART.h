/*
 * UART.h
 *
 * Created: 24/07/2025 22:09:55
 *  Author: Luis Sactic
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void UART_Init(uint16_t ubrr);
void UART_Transmit(char data);
char UART_Receive(void);

// Variable global para comunicación con main
extern volatile uint8_t serial_data_ready;
extern volatile char serial_command;

#endif /* UART_H_ */