/*
 * LAB 1.c
 *
 * Created: 11/07/2025 10:46:40
 * Author : Luis Sactic
 */ 


//-------------------------------------------- LAB #1 --------------------------------------------


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "7segmentos/7segmentos.h"

void setup() {
    segmentos_init();
    
    // Configurar botón de inicio (PC3 como entrada sin pull-up - pull-down externo)
    DDRC &= ~(1 << PC3);
    PORTC &= ~(1 << PC3);  // Desactivar pull-up interno
}

int main(void) {
    setup();
    
    while (1) {
        // Botón de inicio (PC3) inicia la cuenta regresiva desde 5
        if (boton_presionado(PC3)) {
            cuenta_regresiva(5);
        }
    }
}

