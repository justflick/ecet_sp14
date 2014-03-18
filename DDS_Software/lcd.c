/**
 File:       lcd.c
 Version:    0.1.0
 Date:       Feb. 25, 2006

 C header file for HD44780 LCD module using a 74HCT164 serial in, parallel
 out out shift register to operate the LCD in 8-bit mode.  Example schematic
 usuing a Powertip PC-1202A LCD available at http://www.micahcarrick.com

 Uses the avr-libc library for gcc (avr-gcc).

 ****************************************************************************
 Copyright (C) 2006 Micah Carrick   <email@micahcarrick.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************
 */
#include "main.h"
#include "lcd.h"
#include <inttypes.h>
#include <avr/io.h>
#include "serial.h"
#include <util/delay.h>
#include <stdlib.h>


/**
 * Turns the backlight on or off.  The LCD_BACKLIGHT_PIN should be defined as
 * the pin connected to the backlight control of the LCD.
 *
 *
 * @param backlight_on backlight_on    0=off, 1=on
 */
void lcd_backlight(int backlight_on) {
	if (backlight_on) {
		/* LCD Backlight on */
		LCD_PORT |= _BV(LCD_BACKLIGHT_PIN);
	} else {
		/*  LCD Backlight off */
		LCD_PORT &= ~_BV(LCD_BACKLIGHT_PIN);
	}
}

/**
 *  Initializes the LCD.  Should be called during the initialization of the
 * program.
 *
 * @param set_function See LCD_FUNCTION_* definitions in lcd.h
 * @param set_entry_mode See LCD_CMD_ENTRY_* definitions in lcd.h
 * @param on See LCD_CMD_ON_* definitions in lcd.h
 */
void lcd_initialize(uint8_t set_function, uint8_t set_entry_mode, uint8_t on) {
	/* 20ms delay while LCD powers on */
//	_delay_ms(1);
	delayTicker_ms(16);

	/* Write 0x30 to LCD and wait 5 mS for the instruction to complete */
	lcd_load_byte(0x30);
	lcd_send_cmd();
	delayTicker_ms(5);



	/* Write 0x30 to LCD and wait 160 uS for instruction to complete */
	lcd_load_byte(0x30);
	lcd_send_cmd();
	_delay_us(120);

	/* Write 0x30 AGAIN to LCD and wait 160 uS */
	lcd_load_byte(0x30);
	lcd_send_cmd();
	_delay_us(120);

	/* Set function and wait 40uS */
	lcd_load_byte(set_function);
	lcd_send_cmd();

	/* Turn off the display and wait 40uS */
	lcd_load_byte(LCD_CMD_OFF);
	lcd_send_cmd();

	/* Clear display and wait 1.64mS */
	lcd_load_byte(LCD_CMD_CLEAR);
	lcd_send_cmd();
//	_delay_ms(2);
	delayTicker_ms(2);


	/* Set entry mode and wait 40uS */
	lcd_load_byte(set_entry_mode);
	lcd_send_cmd();

	/* Turn display back on and wait 40uS */
	lcd_load_byte(on);
	lcd_send_cmd();
}
;
/**
 * Function for quick mode change of the LCD
 * Exmaples include LCD_CMD_CLR
 * LCD_CMD_HOME
 * LCD_CMD_ENTRY_DEC
 * LCD_CMD_ENTRY_DEC_SHIFT
 *
 * @param mode_const
 */
void lcd_set_mode(uint8_t mode_const) {
	lcd_load_byte(mode_const);
	lcd_send_cmd();
}
/**
 *
 *
 * @param value: The value to be displayed. (will be shifted right by # of decimal digits)
 * @param digits: total number of places to be printed
 * @param decimalPt: number of digits to print as a deciaml. 0 omits decimal point
 */
void lcd_print_numeric(int32_t value, uint8_t digits, uint8_t decimalPt) {
	int8_t tempArray[digits + 2];
//	uint16_t serialtemp = (uint16_t) value;
//	serialPutChar('\n');
//	serialWriteNum(serialtemp);
//	serialWriteString("\narrary[]=");
//serialPutChar('0' + value % 10);
	if (value < 0) lcd_putc('-');	//handling of negative values.
	else lcd_putc(' ');
	for (int i = 0; i < digits; i++) {	//break the value down to a BCD array.
		tempArray[i + 1] = abs(value % 10);
		value /= 10;
	}

	for (int i = 0; i < digits; i++) {
		if ((digits - decimalPt == i) && decimalPt) lcd_putc('.');
		lcd_putc('0' + tempArray[digits - i]);

	}
}

/**
 * Loads a byte into the shift register (74'164).  Does NOT load into the LCD.
 *
 * @param out_byte The byte to load into the '164
 */
void lcd_load_byte(uint8_t out_byte) {
	/* make sure clock is low */
	LCD_PORT &= ~_BV(LCD_CLOCK_PIN);

	int i;
	for (i = 0; i < 8; i++) {
		/* loop through bits */

		if (out_byte & 0x80) {
			/* this bit is high */
			LCD_PORT |= _BV(LCD_RSDS_PIN);
		} else {
			/* this bit is low */
			LCD_PORT &= ~_BV(LCD_RSDS_PIN);
		}
		out_byte = out_byte << 1;

		/* pulse the the shift register clock */
		LCD_PORT |= _BV(LCD_CLOCK_PIN);
		LCD_PORT &= ~_BV(LCD_CLOCK_PIN);
	}
}

/**
 * Loads the byte in the '164 shift register into the LCD as a command. The
 * '164 should already be loaded with the data using lcd_load_byte().
 */
void lcd_send_cmd(void) {
	/* Data in '164 is a command, so RS must be low (0) */
	LCD_PORT &= ~_BV(LCD_RSDS_PIN);
	lcd_strobe_E();
	_delay_us(1);
}

/**
 * Loads the byte in the '164 shift register into the LCD as a character. The
 * '164 should already be loaded with the data using lcd_load_byte().
 */
void lcd_send_char(void) {
	/* Data in '164 is a character, so RS must be high (1) */
	LCD_PORT |= _BV(LCD_RSDS_PIN);
	lcd_strobe_E();
	_delay_us(1);
}

/**
 * Loads the byte into the shift register and then sends it to the LCD as a char
 * @param c The byte (character) to display
 */
void lcd_putc(const char c) {
	lcd_load_byte(c);
	lcd_send_char();
//	serialPutChar(c);
}

void lcd_putstring(char *string) {
	while (*string) {
		lcd_putc(*string++);
	}
}

/**
 * Strobes the E signal on the LCD to "accept" the byte in the '164.  The RS
 * line determines wheter the byte is a character or a command.
 */
void lcd_strobe_E(void) {
	/* strobe E signal */
	LCD_PORT |= _BV(LCD_ENABLE_PIN);
	_delay_us(20);
	LCD_PORT &= ~_BV(LCD_ENABLE_PIN);
}

/**Moves the cursor to the home position.
 *
 */
void lcd_cursor_home() {
	lcd_load_byte(LCD_CMD_HOME);
	lcd_send_cmd();
}

/**
 * Moves the cursor to the specified position.
 *
 * @param pos Position on that line starting at zero
 * @param line  Line number starting at zero
 */
void lcd_move_cursor(uint8_t pos, uint8_t line) {

	uint8_t lineMap[5] =
		{ LCD_LINE_1,
		LCD_LINE_2,
		LCD_LINE_3,
		LCD_LINE_4 };
	lcd_load_byte(0x80 | (pos + lineMap[line]));
	lcd_send_cmd();
}

/**
 * Moves the cursor a number of spaces to the right
 *
 * @param spaces Number of spaces to move
 */
void lcd_inc_cursor(uint8_t spaces) {
	while (spaces--) {
		lcd_load_byte(LCD_CMD_CURSOR_RIGHT);
		lcd_send_cmd();
	}
}

/**
 * Moves the cursor a number of spaces to the left
 *
 * @param space  sNumber of spaces to move
 */
void lcd_dec_cursor(uint8_t spaces) {
	while (spaces--) {
		lcd_load_byte(LCD_CMD_CURSOR_LEFT);
		lcd_send_cmd();
	}
}
