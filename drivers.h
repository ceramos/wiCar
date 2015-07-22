/*
*******************************************************************************
*	The MIT License (MIT)
*
*	Copyright (c) 2015 Pham Van Tuong 
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files (the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included in
*	all copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*	THE SOFTWARE.
*
*******************************************************************************
*/

/*
 *	Filename	:drivers.h
 *	Created on	:Jul 10, 2015
 *  Author		:Pham Van Tuong
 *	Version		:1
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

#include "msp430g2553.h"

#define 	FREQ		(unsigned int)1000	/* 1MHz	*/
#define 	TICK_RATE_KHZ	(unsigned int)1		/* 1ms Timing */

#define		BAUDRATE	(9600)

#define 	DIR			(BIT1)
#define		PWM			(BIT6)	/* P1.6 */
#define		SLEEP		(BIT3)
#define		FAULT		(BIT4)
#define		VPROPI		(BIT5)	/* Feedback pin. Connect to ADC's pin */

#define		WIFI_TX		(BIT1)	/* P1.1 */
#define		WIFI_RX		(BIT2)	/* P1.2 */


void System_Clock(void);
void Peripheral_Initialization(void);
void Timer_Initialization(void);
void PWM_Initialization(void);
void GPIO_Initialization(void);
void ADC10_Initialization(void);
void UART_Initiaization(void);

/*****************************************************************************/
/*rx_flag
* This flag is to be used by other modules to check and see if a new transmission has happened.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile unsigned int rx_flag;

/*rx_fifo_full
* This flag is to be used by other modules to check and see if the rx fifo is full.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile unsigned int rx_fifo_full;

/*tx_fifo_full
* This flag is to be used by other modules to check and see if the rx fifo is full.
* This is READ ONLY. Do not write to it or the UART may crash.
*/
extern volatile unsigned int tx_fifo_full;


unsigned char uart_getc();

void uart_gets();

void uart_putc(unsigned char c);

void uart_puts(char *str);

#endif /* DRIVERS_H_ */
