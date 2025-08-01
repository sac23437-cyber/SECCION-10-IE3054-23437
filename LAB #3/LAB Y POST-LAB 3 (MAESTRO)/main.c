/*
 * LAB Y POST-LAB 3 (MAESTRO).c
 *
 * Created: 28/07/2025 18:01:22
 * Author : Luis Sactic
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "UART/UART.h"
#include "SPI/SPI.h"

// Variables de estado
#define COMANDO_POT1  0x41  // 'A' en ASCII
#define COMANDO_POT2  0x42  // 'B' en ASCII
uint8_t modo_operacion = 'P';
const uint8_t MODO_LECTURA = 'P';
const uint8_t MODO_ESCRITURA = 'L';

// Configuración de pines
void configurarPuertos() {
    DDRD |= 0b11111100;  // Pines D2-D7 como salida
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2); // Pines B0-B1 como salida
    PORTB |= (1 << PB2);  // SS en alto inicialmente
}

// Control de LEDs
void actualizarLEDs(uint8_t valor) {
    PORTD = (PORTD & 0b00000011) | (valor << 2);
    PORTB = (PORTB & 0b11111100) | (valor >> 6);
}

// Limpieza de buffer serial
void limpiarBufferSerial() {
    while (UART_Available()) {
        UART_Receive();
        _delay_us(100);
    }
}

// Procesamiento de entrada numérica
uint8_t obtenerValorNumerico() {
    char buffer[5] = {0};
    uint8_t indice = 0;
    
    while (indice < 4) {
        char caracter = UART_Receive();
        if (caracter == '\r' || caracter == '\n') break;
        
        buffer[indice++] = caracter;
        UART_Transmit(caracter);
    }
    
    uint16_t valor = atoi(buffer);
    return (valor > 255) ? 255 : (uint8_t)valor;
}

// Modo de lectura de potenciómetros
void leerPotenciometros() {
    uint8_t valor_pot1, valor_pot2;
    
    // Lectura del primer potenciómetro
    PORTB &= ~(1 << PB2);          // Activar línea SS
    SPI_Exchange(COMANDO_POT1);    // Enviar comando para POT1
    _delay_us(100);                // Espera para estabilización
    valor_pot1 = SPI_Exchange(0x00); // Recibir valor
    PORTB |= (1 << PB2);           // Desactivar línea SS

    _delay_ms(5);                  // Pequeña pausa entre lecturas

    // Lectura del segundo potenciómetro
    PORTB &= ~(1 << PB2);          // Activar línea SS
    SPI_Exchange(COMANDO_POT2);    // Enviar comando para POT2
    _delay_us(100);                // Espera para estabilización
    valor_pot2 = SPI_Exchange(0x00); // Recibir valor
    PORTB |= (1 << PB2);           // Desactivar línea SS

    UART_SendString("POT1: ");
    UART_SendNumber(valor_pot1);
    UART_SendString("  POT2: ");
    UART_SendNumber(valor_pot2);
    UART_SendString("\r\n");
}

int main(void) {
    // Inicialización
    UART_Init(9600);
    SPI_MasterInit();
    configurarPuertos();

    // Bucle principal
    while (1) {
        if (UART_Available()) {
            char comando = UART_Receive();

            if (comando == 'P' || comando == 'p') {
                modo_operacion = MODO_LECTURA;
                UART_SendString("Modo Lectura \r\n");
                limpiarBufferSerial();
            }
            else if (comando == 'L' || comando == 'l') {
                modo_operacion = MODO_ESCRITURA;
                UART_SendString("Modo Escritura \r\n");
                UART_SendString("Ingrese un valor entre 0 y 255: ");
                
                uint8_t valor = obtenerValorNumerico();
                
                UART_SendString("\r\nValor configurado: ");
                UART_SendNumber(valor);
                UART_SendString("\r\n");

                actualizarLEDs(valor);
                PORTB &= ~(1 << PB2);
                SPI_Exchange(valor);
                PORTB |= (1 << PB2);
            }
        }

        if (modo_operacion == MODO_LECTURA) {
            leerPotenciometros();
            _delay_ms(500);
        }
    }
}