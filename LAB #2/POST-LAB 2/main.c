/*
 * POST-LAB 2.c
 * 
 * Created: 19/07/2025 20:32:22
 * Author : Luis Sactic
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <avr/interrupt.h>
#include "ADC/ADC.h"
#include "UART/UART.h"

// Configuración LCD
#define LCD_RS PB3
#define LCD_EN PB2
#define LCD_D0 PB1
#define LCD_D1 PB0
#define LCD_D2 PD7
#define LCD_D3 PD6
#define LCD_D4 PD2
#define LCD_D5 PD3
#define LCD_D6 PD4
#define LCD_D7 PD5

#define ADC_MAX 1023.0
#define V_REF 5.0
#define ZERO_THRESHOLD 12
#define NOISE_THRESHOLD 0.12

// Variables globales
volatile int16_t counter = 0;  // Contador con signo para S3

// Prototipos de funciones LCD
void LCD_Init();
void LCD_Cmd(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_String(const char *str);
void LCD_SetCursor(uint8_t x, uint8_t y);
void LCD_Clear();
void displayVoltage(uint16_t adc_value, uint8_t x, uint8_t y);
void forceZeroIfNeeded(uint16_t* adc_value, uint8_t channel);
void displayCounter(int16_t count, uint8_t x, uint8_t y);
void processSerialCommand(char cmd);
void printVoltagesToSerial(uint16_t val1, uint16_t val2);

int main(void) {
    // Configuración inicial
    DDRB |= (1 << PB0); // Backlight
    PORTB |= (1 << PB0);
    
    // Inicializar LCD
    LCD_Init();
    _delay_ms(100);
    LCD_Clear();
    
    // Inicializar ADC
    ADC_Init();
    
    // Inicializar comunicación serial
    UART_Init(103); // 9600 baudios para 16MHz
    
    // Habilitar interrupciones globales
    sei();
    
    LCD_SetCursor(0, 0);
    LCD_String("S1:    S2:   S3:");
    
    // Variable para controlar la primera impresión en consola
    uint8_t first_print = 1;
    
    while(1) {
        // Leer potenciómetros
        uint16_t val1 = ADC_Read(6);
        uint16_t val2 = ADC_Read(7);
        
        forceZeroIfNeeded(&val1, 6);
        forceZeroIfNeeded(&val2, 7);
        
        displayVoltage(val1, 0, 1);
        displayVoltage(val2, 7, 1);
        
        // Mostrar contador (posición 13,1 con 3 caracteres de ancho)
        displayCounter(counter, 13, 1);
        
        // Procesar comandos seriales
        if(serial_data_ready) {
            processSerialCommand(serial_command);
            serial_data_ready = 0;
        }
        
        // Mostrar en consola (en la misma línea)
        if(first_print) {
            // Primera vez: imprimir normalmente
            UART_Transmit('\r');
            UART_Transmit('\n');
            printVoltagesToSerial(val1, val2);
            first_print = 0;
        } else {
            // Actualizaciones: sobreescribir la misma línea
            UART_Transmit('\r');
            printVoltagesToSerial(val1, val2);
        }
        
        _delay_ms(200);
    }
}

// Funciones LCD
void LCD_Init() {
    DDRB |= (1 << LCD_D0) | (1 << LCD_D1) | (1 << LCD_RS) | (1 << LCD_EN);
    DDRD |= (1 << LCD_D2) | (1 << LCD_D3) | (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
    
    _delay_ms(50);
    LCD_Cmd(0x38);
    _delay_us(4500);
    LCD_Cmd(0x38);
    _delay_us(4500);
    LCD_Cmd(0x38);
    _delay_us(150);
    
    LCD_Cmd(0x0C);
    LCD_Cmd(0x06);
    LCD_Cmd(0x01);
    _delay_ms(5);
}

void LCD_Cmd(uint8_t cmd) {
    PORTB = (PORTB & ~((1 << LCD_D0) | (1 << LCD_D1))) |
    ((cmd & 0x01) << LCD_D0) | ((cmd & 0x02) ? (1 << LCD_D1) : 0);
    
    PORTD = (PORTD & ~((1 << LCD_D2) | (1 << LCD_D3))) |
    ((cmd & 0x04) ? (1 << LCD_D2) : 0) | ((cmd & 0x08) ? (1 << LCD_D3) : 0);
    
    PORTD = (PORTD & ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7))) |
    ((cmd & 0x10) ? (1 << LCD_D4) : 0) | ((cmd & 0x20) ? (1 << LCD_D5) : 0) |
    ((cmd & 0x40) ? (1 << LCD_D6) : 0) | ((cmd & 0x80) ? (1 << LCD_D7) : 0);
    
    PORTB &= ~(1 << LCD_RS);
    PORTB |= (1 << LCD_EN);
    _delay_us(1);
    PORTB &= ~(1 << LCD_EN);
    _delay_ms(2);
}

void LCD_Data(uint8_t data) {
    PORTB = (PORTB & ~((1 << LCD_D0) | (1 << LCD_D1))) |
    ((data & 0x01) << LCD_D0) | ((data & 0x02) ? (1 << LCD_D1) : 0);
    
    PORTD = (PORTD & ~((1 << LCD_D2) | (1 << LCD_D3))) |
    ((data & 0x04) ? (1 << LCD_D2) : 0) | ((data & 0x08) ? (1 << LCD_D3) : 0);
    
    PORTD = (PORTD & ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7))) |
    ((data & 0x10) ? (1 << LCD_D4) : 0) | ((data & 0x20) ? (1 << LCD_D5) : 0) |
    ((data & 0x40) ? (1 << LCD_D6) : 0) | ((data & 0x80) ? (1 << LCD_D7) : 0);
    
    PORTB |= (1 << LCD_RS);
    PORTB |= (1 << LCD_EN);
    _delay_us(1);
    PORTB &= ~(1 << LCD_EN);
    _delay_us(100);
}

void LCD_String(const char *str) {
    while(*str) LCD_Data(*str++);
}

void LCD_SetCursor(uint8_t x, uint8_t y) {
    uint8_t addr = (y == 0) ? 0x80 + x : 0xC0 + x;
    LCD_Cmd(addr);
}

void LCD_Clear() {
    LCD_Cmd(0x01);
    _delay_ms(5);
}

// Funciones de ayuda
void displayVoltage(uint16_t adc_value, uint8_t x, uint8_t y) {
    char buffer[10];
    float voltage;
    
    // Manejo especial de los extremos
    if(adc_value == 0) {
        voltage = 0.0;
    }
    else if(adc_value >= 1018) { // Límite ajustado para 5.00V
        voltage = V_REF;
    }
    else {
        // Cálculo lineal con ajuste para el offset observado
        voltage = (adc_value * V_REF) / ADC_MAX;
        
        // Compensación para el offset de ~0.12V
        if(voltage < NOISE_THRESHOLD) {
            float compensation = (NOISE_THRESHOLD - voltage) * 1.2;
            voltage = (voltage > compensation) ? (voltage - compensation) : 0.0;
        }
    }
    
    // Redondeo a dos decimales con protección
    voltage = roundf(voltage * 100) / 100;
    if(voltage < 0.01) voltage = 0.0; // Forzar cero real
    
    // Formateo seguro
    uint16_t volts = (uint16_t)voltage;
    uint16_t cents = (uint16_t)((voltage - volts) * 100 + 0.5);
    
    // Corrección por redondeo
    if(cents >= 100) {
        cents = 0;
        volts++;
    }
    
    buffer[0] = volts + '0';
    buffer[1] = '.';
    buffer[2] = (cents / 10) + '0';
    buffer[3] = (cents % 10) + '0';
    buffer[4] = 'V';
    buffer[5] = '\0';
    
    LCD_SetCursor(x, y);
    LCD_String(buffer);
}

void forceZeroIfNeeded(uint16_t* adc_value, uint8_t channel) {
    if(*adc_value > ZERO_THRESHOLD) return;
    
    // Si está cerca de cero, hacer una verificación más estricta
    uint16_t strict_zero_count = 0;
    for(uint8_t i = 0; i < 10; i++) {
        ADMUX = (1 << REFS0) | (channel & 0x07);
        ADCSRA |= (1 << ADSC);
        while(ADCSRA & (1 << ADSC));
        if(ADC <= (ZERO_THRESHOLD/2)) strict_zero_count++;
        _delay_us(50);
    }
    
    // Si al menos 8 de 10 lecturas están muy cerca de cero
    if(strict_zero_count >= 8) {
        *adc_value = 0;
    }
}

void displayCounter(int16_t count, uint8_t x, uint8_t y) {
    char buffer[4]; // Suficiente para 3 dígitos + null terminator
    
    if(count < 0) {
        // Números negativos (-99 a -1)
        snprintf(buffer, sizeof(buffer), "%3d", count); // Formato: -X o -XX
    } else {
        // Números positivos y cero (0 a 999)
        snprintf(buffer, sizeof(buffer), "%3d", count); // Formato: XX o XXX
    }
    
    LCD_SetCursor(x, y);
    LCD_String(buffer);
}

void processSerialCommand(char cmd) {
    if(cmd == '+') {
        counter++;
        if(counter > 999) counter = 999;
    }
    else if(cmd == '-') {
        counter--;
        if(counter < -99) counter = -99;
    }
}

void printVoltagesToSerial(uint16_t val1, uint16_t val2) {
    char buffer[32];
    float voltage1, voltage2;
    
    // Calcular voltajes con el mismo método que en displayVoltage()
    if(val1 == 0) {
        voltage1 = 0.0;
    } else if(val1 >= 1018) {
        voltage1 = V_REF;
    } else {
        voltage1 = (val1 * V_REF) / ADC_MAX;
        if(voltage1 < NOISE_THRESHOLD) {
            float compensation = (NOISE_THRESHOLD - voltage1) * 1.2;
            voltage1 = (voltage1 > compensation) ? (voltage1 - compensation) : 0.0;
        }
    }
    
    if(val2 == 0) {
        voltage2 = 0.0;
    } else if(val2 >= 1018) {
        voltage2 = V_REF;
    } else {
        voltage2 = (val2 * V_REF) / ADC_MAX;
        if(voltage2 < NOISE_THRESHOLD) {
            float compensation = (NOISE_THRESHOLD - voltage2) * 1.2;
            voltage2 = (voltage2 > compensation) ? (voltage2 - compensation) : 0.0;
        }
    }
    
    // Formatear con 2 decimales
    int volts1 = (int)voltage1;
    int cents1 = (int)((voltage1 - volts1) * 100 + 0.5);
    int volts2 = (int)voltage2;
    int cents2 = (int)((voltage2 - volts2) * 100 + 0.5);
    
    // Construir cadena manualmente para evitar problemas con snprintf
    buffer[0] = 'S';
    buffer[1] = '1';
    buffer[2] = ':';
    buffer[3] = ' ';
    buffer[4] = volts1 + '0';
    buffer[5] = '.';
    buffer[6] = (cents1 / 10) + '0';
    buffer[7] = (cents1 % 10) + '0';
    buffer[8] = 'V';
    buffer[9] = ' ';
    buffer[10] = ' ';
    buffer[11] = ' ';
    buffer[12] = ' ';
    buffer[13] = 'S';
    buffer[14] = '2';
    buffer[15] = ':';
    buffer[16] = ' ';
    buffer[17] = volts2 + '0';
    buffer[18] = '.';
    buffer[19] = (cents2 / 10) + '0';
    buffer[20] = (cents2 % 10) + '0';
    buffer[21] = 'V';
    buffer[22] = '\0';
    
    // Enviar por serial
    for(uint8_t i = 0; buffer[i] != '\0'; i++) {
        UART_Transmit(buffer[i]);
    }
}