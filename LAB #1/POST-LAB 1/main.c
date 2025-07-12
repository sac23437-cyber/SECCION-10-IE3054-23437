/*
 * POST-LAB 1.c
 *
 * Created: 11/07/2025 19:01:45
 * Author : Luis Sactic
 */ 

//-------------------------------------------------------- POST-LAB #1 --------------------------------------------------------

/************************/
// Encabezado (Libraries)
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/************************/
// Constants and Variables
const uint8_t segmentos[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

typedef enum {
    WAITING,    // Esperando inicio
    COUNTDOWN,  // Cuenta regresiva activa
    RACE,       // Carrera en progreso
    WIN_P1,     // Jugador 1 ganó
    WIN_P2      // Jugador 2 ganó
} GameState;

volatile GameState gameState = WAITING;
volatile uint8_t player1Step = 0;
volatile uint8_t player2Step = 0;

/************************/
// Function prototypes
void setup(void);
void updatePlayerLeds(void);
uint8_t isButtonPressed(uint8_t pin);
void countdown(void);
void showVictory(uint8_t player);
void resetGame(void);
void incrementPlayer1(void);
void incrementPlayer2(void);

/************************/
// Main Function
int main(void) 
{
    setup();
    
    while (1) 
    {
        switch (gameState) 
        {
            case WAITING:
                if (isButtonPressed(PC3)) {
                    countdown();
                }
                break;
                
            case RACE:
                if (isButtonPressed(PC4)) {
                    incrementPlayer1();
                }
                if (isButtonPressed(PC5)) {
                    incrementPlayer2();
                }
                if (isButtonPressed(PC3)) {
                    resetGame();
                }
                break;
                
            case WIN_P1:
            case WIN_P2:
                if (isButtonPressed(PC3)) {
                    resetGame();
                }
                break;
                
            default:
                break;
        }
    }
}

/************************/
// NON-Interrupt subroutines
void setup(void) 
{
    // Configurar PD0-PD6 como salidas (display)
    DDRD  = 0b01111111;
    
    // Configurar botones (A3, A4, A5 como entradas sin pull-up)
    DDRC  &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
    PORTC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));
    
    // Configurar LEDs de jugadores
    DDRD  |= (1 << PD7);
    DDRB  |= (1 << PB0) | (1 << PB1) | (1 << PB2);
    DDRB  |= (1 << PB3) | (1 << PB4) | (1 << PB5);
    DDRC  |= (1 << PC0);
    
    // Mostrar '0' al inicio
    PORTD = segmentos[0];
}

void incrementPlayer1(void) 
{
    if (player1Step < 4) {
        player1Step++;
        updatePlayerLeds();
    } else if (player1Step == 4) {
        gameState = WIN_P1;
        showVictory(1);
    }
}

void incrementPlayer2(void) 
{
    if (player2Step < 4) {
        player2Step++;
        updatePlayerLeds();
    } else if (player2Step == 4) {
        gameState = WIN_P2;
        showVictory(2);
    }
}

void updatePlayerLeds(void) 
{
    // Jugador 1
    PORTD = (PORTD & 0b01111111) | ((player1Step == 1) ? (1 << PD7) : 0);
    PORTB = (PORTB & 0b11111000) |
           ((player1Step == 2) ? (1 << PB0) : 0) |
           ((player1Step == 3) ? (1 << PB1) : 0) |
           ((player1Step == 4) ? (1 << PB2) : 0);
    
    // Jugador 2
    PORTB = (PORTB & 0b11000111) |
           ((player2Step == 1) ? (1 << PB3) : 0) |
           ((player2Step == 2) ? (1 << PB4) : 0) |
           ((player2Step == 3) ? (1 << PB5) : 0);
    PORTC = (PORTC & 0b11111110) | ((player2Step == 4) ? (1 << PC0) : 0);
}

uint8_t isButtonPressed(uint8_t pin) 
{
    if (pin != PC3 && pin != PC4 && pin != PC5) return 0;
    
    if (PINC & (1 << pin)) {
        _delay_ms(50);
        if (PINC & (1 << pin)) {
            while (PINC & (1 << pin));
            return 1;
        }
    }
    return 0;
}

void countdown(void) 
{
    gameState = COUNTDOWN;
    for (uint8_t i = 5; i > 0; i--) {
        PORTD = segmentos[i];
        _delay_ms(500);
    }
    PORTD = segmentos[0];
    gameState = RACE;
}

void showVictory(uint8_t player) 
{
    if (player == 1) {
        PORTB &= ~((1 << PB3) | (1 << PB4) | (1 << PB5));
        PORTC &= ~(1 << PC0);
        PORTD = segmentos[1];
        
        for (uint8_t i = 0; i < 3; i++) {
            PORTD |= (1 << PD7);
            PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
            _delay_ms(300);
            PORTD &= ~(1 << PD7);
            PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
            _delay_ms(300);
        }
        
        PORTD |= (1 << PD7);
        PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
    } else {
        PORTD &= ~(1 << PD7);
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
        PORTD = segmentos[2];
        
        for (uint8_t i = 0; i < 3; i++) {
            PORTB |= (1 << PB3) | (1 << PB4) | (1 << PB5);
            PORTC |= (1 << PC0);
            _delay_ms(300);
            PORTB &= ~((1 << PB3) | (1 << PB4) | (1 << PB5));
            PORTC &= ~(1 << PC0);
            _delay_ms(300);
        }
        
        PORTB |= (1 << PB3) | (1 << PB4) | (1 << PB5);
        PORTC |= (1 << PC0);
    }
}

void resetGame(void) 
{
    PORTD &= ~(1 << PD7);
    PORTB = 0;
    PORTC &= ~(1 << PC0);
    
    player1Step = 0;
    player2Step = 0;
    gameState = WAITING;
    PORTD = segmentos[0];
}

/************************/
// Interrupt routines


