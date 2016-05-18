/* Copyright 2011-2016 Tyler Gilbert; 
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 */

#include "mcu/mcu.h"
#include "mcu/core.h"

__IO uint32_t *  _mcu_get_iocon_regs(int port, int pin);

typedef struct {
	uint8_t entry[4];
} pinsel_table_t;

#define ENTRY(function, port) (port<<5|function)
#define ENTRY_GET_FUNCTION(entry) (entry&~0xE0)
#define ENTRY_GET_PORT(entry) ((entry>>5)&0x07)

#define TOTAL_PINS (24+32)

const pinsel_table_t pinsel_func_table[TOTAL_PINS] = {
		{{ ENTRY(CORE_PERIPH_RESET, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.0
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_CLKOUT, 0), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_USB, 0) }}, //0.1
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.2
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_USB, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.3
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_I2C, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.4
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_I2C, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.5
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_USB, 0), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.6
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.7
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.8
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_JTAG, 0) }}, //0.9
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_TMR, 0) }}, //0.10
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 0) }}, //0.11
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //0.12
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //0.13
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //0.14
		{{ ENTRY(CORE_PERIPH_JTAG, 0), ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3) }}, //0.15
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.16
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.17
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.18
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.19
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.20
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_SSP, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.21
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_SSP, 1) }}, //0.22
		{{ ENTRY(CORE_PERIPH_PIO, 0), ENTRY(CORE_PERIPH_ADC, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //0.23

		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.0
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.1
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.2
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.3
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.4
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 3), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.5

		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.6

		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.7
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.8

		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.9

		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.10
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.11

		{{ ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.12

		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.13
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.14
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_SSP, 1) }}, //1.15
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.16
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.17
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 0), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.18
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SSP, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.19
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SSP, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.20
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SSP, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.21
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_SSP, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.22
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 1), ENTRY(CORE_PERIPH_SSP, 1), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.23
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.24
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.25
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.26
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.27
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_UART, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.28
		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.29

		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_SSP, 0), ENTRY(CORE_PERIPH_TMR, 2), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.30

		{{ ENTRY(CORE_PERIPH_PIO, 1), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0), ENTRY(CORE_PERIPH_RESERVED, 0) }}, //1.31



};



int _mcu_core_set_pinsel_func(int gpio_port, int pin, core_periph_t function, int periph_port){
	int i;
	pinsel_table_t entry;
	int value;
	__IO uint32_t * regs_iocon;

	if( (gpio_port == 3) && (pin > 5) ){
		return -1;
	}

	value = gpio_port * 12 + pin;

	entry = pinsel_func_table[value];
	value = -1;
	for(i = 0; i < 4; i++){
		if ( (function == ENTRY_GET_FUNCTION(entry.entry[i])) &&
				(periph_port == ENTRY_GET_PORT(entry.entry[i])) ){
					//this is a valid pin
					value = i;
					break;
				}
	}

	if ( value < 0 ){
		return -1;
	}

	regs_iocon = _mcu_get_iocon_regs(gpio_port, pin);
	*regs_iocon &= ~0x07;
	*regs_iocon |= value;
	return 0;
}