/*
 * SPI.h
 *
 * Created: 28/07/2025 18:05:11
 * Author : Luis Sactic
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

// Inicializa SPI como maestro
void SPI_MasterInit(void);

// Inicializa SPI como esclavo
void SPI_SlaveInit(void);

// Intercambia un byte por SPI (env�a y recibe simult�neamente)
uint8_t SPI_Exchange(uint8_t data);

#endif /* SPI_H_ */

