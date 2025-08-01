/*
 * LAB y POST-LAB 3 (ESCLAVO).c
 *
 * Created: 28/07/2025 17:49:30
 * Author : Luis Sactic
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC/ADC.h"
#include "SPI/SPI.h"

// Máscaras para control de LEDs
#define LEDS_PORTD_MASK 0b11111100  // Pines D2-D7
#define LEDS_PORTB_MASK 0b00000011  // Pines B0-B1 (D8-D9)


void actualizarLEDs(uint8_t valor) {
    PORTD = (PORTD & ~LEDS_PORTD_MASK) | ((valor << 2) & LEDS_PORTD_MASK);
    PORTB = (PORTB & ~LEDS_PORTB_MASK) | ((valor >> 6) & LEDS_PORTB_MASK);
}

ISR(SPI_STC_vect) {
    static uint8_t esperando_comando = 1;
    uint8_t dato = SPDR;
    
    if(esperando_comando) {
        switch(dato) {
            case 'A': 
                SPDR = ADC_Read(0) >> 2;  // Valor POT1
                break;
            case 'B':
                SPDR = ADC_Read(1) >> 2;  // Valor POT2
                break;
            default:
                actualizarLEDs(dato);     // Control LEDs
                SPDR = 0x00;
                break;
        }
        esperando_comando = 0;
    } else {
        esperando_comando = 1;
        SPDR = 0x00;
    }
}

void init_perifericos() {
    // Configuración SPI
    SPI_SlaveInit();
    SPDR = 0x00;
    SPCR |= (1 << SPIE);
    
    // Configuración ADC
    ADC_Init();
    
    // Configuración LEDs
    DDRD |= LEDS_PORTD_MASK;
    DDRB |= LEDS_PORTB_MASK;
}

int main(void) {
    init_perifericos();
    sei();
    
    while(1) {
        // Todo en interrupciones
    }
}