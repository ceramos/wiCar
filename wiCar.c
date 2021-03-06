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
 *	Filename	:wiCar.c -- This is main
 *	Created on	:Jul 12, 2015
 *  Author		:Pham Van Tuong
 *	Version		:
 */

#include "msp430g2553.h"
#include "typedef.h"
#include "drivers.h"
#include "application.h"
#include "interrupts.h"

// Global variables
volatile uint16 iii=0;


// Local variables


// 


int main(void) {
	WDTCTL = WDTPW + WDTHOLD; 			//Stop WDT

	System_Clock();
	
	Peripheral_Initialization();

	__enable_interrupt();				//Interrupts Enabled


	while(1){
		// Do something
		PWM_Out(1000);
		__delay_cycles(1000);
	}
}

