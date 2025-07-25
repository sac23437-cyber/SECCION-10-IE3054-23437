/*
 * LCD.c
 *
 * Created: 18/07/2025 11:05:46
 *  Author: Luis Sactic
 */ 

#include "LCD.h"

void LCD_Init() {
	DDRD |= (1<<LCD_RS)|(1<<LCD_EN)|(1<<LCD_D4)|(1<<LCD_D5);
	_delay_ms(50);
	
	// Secuencia de inicialización para 8 bits
	LCD_Cmd(0x038);			
	_delay_ms(100);
	LCD_Cmd(0x0C);
	_delay_us(100);
	LCD_Cmd(0x06);
	_delay_us(100);
	LCD_Clear();
	_delay_ms(5);
	
	/*
	// Secuencia de inicialización para 4 bits
	LCD_Cmd(0x02); // Modo 4 bits
	LCD_Cmd(0x28); // 2 líneas, 5x8 puntos
	LCD_Cmd(0x0C); // Display ON, cursor OFF
	LCD_Cmd(0x06); // Incremento automático
	LCD_Cmd(0x01); // Limpiar display
	_delay_ms(5);
	*/
}

void LCD_Cmd(uint8_t cmd) {
	PORTD = (PORTD & 0x03) | ((cmd & 0xFC) >> 2);
	PORTD &= ~(1<<LCD_RS);
	PORTD |= (1<<LCD_EN);
	_delay_us(1);
	PORTD &= ~(1<<LCD_EN);
	_delay_us(100);
	
	PORTD = (PORTD & 0x03) | ((cmd & 0xFC) << 2);
	PORTD |= (1<<LCD_EN);
	_delay_us(1);
	PORTD &= ~(1<<LCD_EN);
	_delay_ms(2);
}

void LCD_Data(uint8_t data) {
	PORTD = (PORTD & 0x03) | ((data & 0xFC) >> 2);
	PORTD |= (1<<LCD_RS);
	PORTD |= (1<<LCD_EN);
	_delay_us(1);
	PORTD &= ~(1<<LCD_EN);
	_delay_us(100);
	
	PORTD = (PORTD & 0x03) | ((data & 0xFC) << 2);
	PORTD |= (1<<LCD_EN);
	_delay_us(1);
	PORTD &= ~(1<<LCD_EN);
	_delay_ms(2);
}

void LCD_String(const char *str) {
	while(*str) {
		LCD_Data(*str++);
	}
}

void LCD_SetCursor(uint8_t x, uint8_t y) {
	uint8_t addr = (y == 0) ? 0x80 + x : 0xC0 + x;
	LCD_Cmd(addr);
}

void LCD_Clear() {
	LCD_Cmd(0x01);
	_delay_ms(5);
}