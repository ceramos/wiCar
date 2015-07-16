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
 *	Filename	:drivers.c
 *	Created on	:Jul 10, 2015
 *  Author		:Pham Van Tuong
 *	Version		:1
 */

#include "msp430g2553.h"
#include "drivers.h"

/*	
		Declare variables
 */
volatile unsigned int count=0;
 
volatile unsigned int tx_flag;			//Mailbox Flag for the tx_char.
volatile unsigned char tx_char;			//This char is the most current char to go into the UART

volatile unsigned int rx_flag;			//Mailbox Flag for the rx_char.
volatile unsigned char rx_char;			//This char is the most current char to come out of the UART


/*	$description: this function initilize system clock - DCO 1MHz
 * 	$function name:	System_Clock
 * 	$prams: none
 * 	$return: none
 */
void System_Clock(void) {
    BCSCTL1 = CALBC1_8MHZ; 				//Set DCO to 1Mhz
    DCOCTL = CALDCO_8MHZ; 				//Set DCO to 1Mhz
}


/*	$description: this function initilize peripheral
 * 	$function name:	Peripheral_Initialization
 * 	$prams: none
 * 	$return: none
 */
void Peripheral_Initialization(void) {
	Timer_Initialization();
	GPIO_Initialization();
}


void Timer_Initialization(void) {
	// Timer
	// Count up mode
	// Using interrupt
	// Timing 1ms
	TA0CTL = 0;					// Stop all

	//TA0CTL = TASSEL_1 | ID_0;	// ACLK + Div 1 and Stop mode

	TA0CTL |= TACLR;			// Clear all to start with

	TA0CCR0 = (FREQ / TICK_RATE_KHZ) - 1;

	TACCTL0 |= CCIE;				// Enable Interrupt


	TA0CTL |= TASSEL_2 | MC_1 | ID_0;	// Up mode and start timer
}


void GPIO_Initialization(void) {
    LED_DIR = LED_1 + LED_2;			//Enable LED
    LED_OUT = LED_1 + LED_2;			//All off
}


void ADC10_Initialization(void);


/*	$description: this function initialize UART module 
 * 	$function name:	UART_Initiaization
 * 	$prams: none
 * 	$return: none
 */
void UART_Initiaization(void) {
	P1SEL = WIFI_TX + WIFI_RX;					//Setup the I/O
	P1SEL2 = WIFI_TX + WIFI_RX;

	UCA0CTL1 |= UCSSEL_2; 				//SMCLK
										//8,000,000Hz, 9600Baud, UCBRx=52, UCBRSx=0, UCBRFx=1
	UCA0BR0 = 52;                  		//8MHz, OSC16, 9600
	UCA0BR1 = 0;                   	 	//((8MHz/9600)/16) = 52.08333
	UCA0MCTL = 0x10|UCOS16; 			//UCBRFx=1,UCBRSx=0, UCOS16=1
	UCA0CTL1 &= ~UCSWRST; 				//USCI state machine
	IE2 |= UCA0RXIE; 					//Enable USCI_A0 RX interrupt

	rx_flag = 0;						//Set rx_flag to 0
	tx_flag = 0;						//Set tx_flag to 0
}

/*	$description: Get a char from the UART. Waits till it gets one
 * 	$function name:	UART_Initiaization
 * 	$prams: none
 * 	$return: Char from UART
 */
unsigned char UART_Getc()				//Waits for a valid char from the UART
{
	while (rx_flag == 0);		 		//Wait for rx_flag to be set
	rx_flag = 0;						//ACK rx_flag
    return rx_char;
}


/*	$description: Get a string of known length from the UART. 
				  Strings terminate when enter is pressed or string buffer fills
				  Will return when all the chars are received or a carriage return (\r) is received. 
				  Waits for the data.
 * 	$function name:	UART_Gets
 * 	$prams: Array pointer and length
 * 	$return: None
 */
void UART_Gets(char* Array, int length)
{
	unsigned int i = 0;

	while((i < length))					//Grab data till the array fills
	{
		Array[i] = uart_getc();
		if(Array[i] == '\r')				//If we receive a \r the master wants to end
		{
			for( ; i < length ; i++)		//fill the rest of the string with \0 nul. Overwrites the \r with \0
			{
				Array[i] = '\0';
			}
			break;
		}
		i++;
	}

    return;
}


/*	$description: Sends a char to the UART. Will wait if the UART is busy
 * 	$function name:	UART_Putc
 * 	$prams: Char to send
 * 	$return: none
 */
void UART_Putc(unsigned char c)
{
	tx_char = c;						//Put the char into the tx_char
	IE2 |= UCA0TXIE; 					//Enable USCI_A0 TX interrupt
	while(tx_flag == 1);				//Have to wait for the TX buffer
	tx_flag = 1;						//Reset the tx_flag
	return;
}


/*	$description: Sends a string to the UART. Will wait if the UART is busy
 * 	$function name:	UART_Puts
 * 	$prams: Pointer to String to send
 * 	$return: none
 */
void UART_Puts(char *str)				//Sends a String to the UART.
{
     while(*str) uart_putc(*str++);		//Advance though string till end
     return;
}




/*	
	Interrupt routines
*/

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER_INTERRUPT(void)
{
   //TACCTL0 &= ~CCIFG;			// Clear interrupt flag
	count++;
	if(count == 500)
	{

		LED_OUT ^= LED_2;
		count=0;
	}
}



#pragma vector = USCIAB0TX_VECTOR		//UART TX USCI Interrupt
__interrupt void USCI0TX_ISR(void)
{
	UCA0TXBUF = tx_char;				//Copy char to the TX Buffer
	tx_flag = 0;						//ACK the tx_flag
	IE2 &= ~UCA0TXIE; 					//Turn off the interrupt to save CPU
}

#pragma vector = USCIAB0RX_VECTOR		//UART RX USCI Interrupt. This triggers when the USCI receives a char.
__interrupt void USCI0RX_ISR(void)
{
	rx_char = UCA0RXBUF;				//Copy from RX buffer, in doing so we ACK the interrupt as well
	rx_flag = 1;						//Set the rx_flag to 1

	P1OUT ^= LED;						//Notify that we received a char by toggling LED
}
