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
 *	Filename	:application.c
 *	Created on	:Jul 10, 2015
 *  Author		:Pham Van Tuong
 *	Version		:1
 */

#include "msp430g2553.h"
#include "drivers.h"
#include "application.h"

/*	
		Declare variables
*/

 
/*
		Function define
*/

/*
	This function is used to generate PWM pulse
	Params:	duty cycle : 0 - 100%
	Return: None
*/
void PWM_Out(unsigned int duty) {
	// Check
	if((duty <= 100) && (duty >= 0)) {
		CCR1  = duty;			// CCR1/CCR0 = duty
		TACTL = MC_1;           // Upmode
	}
	else
		break;	
}

/*****************************************************************************/
void ESP_Setup(void) {
	unsigned char OK = 0;
	char *oks = "OK"
	char *nc  = "no change";
	int mem_loop;

	do {
		uart_puts("AT+RST");
    	uart_puts("\r\n");
    	//delay_ms(4000); 
    	uart_puts("ATE0");
    	uart_puts("\r\n");
    	//delay_ms(1000);

    	for (int mem_loop = 0; mem_loop <= 150 ;mem_loop++) {
    		
    	}


	}
}