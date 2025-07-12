/*
 * _7segmentos.h
 *
 * Created: 11/07/2025 11:33:26
 *  Author: Luis Sactic
 */ 


#ifndef _7SEGMENTOS_H_
#define _7SEGMENTOS_H_

#include <avr/io.h>
#include <util/delay.h>

// Definición de pines (ajustar según conexiones)
#define SEGMENT_PORT PORTD
#define SEGMENT_DDR  DDRD

// Prototipos de funciones
void segmentos_init(void);
void mostrar_numero(uint8_t numero);
void cuenta_regresiva(uint8_t inicio);
uint8_t boton_presionado(uint8_t pin_boton);

#endif // _7SEGMENTOS_H_