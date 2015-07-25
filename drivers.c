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
#include "typedef.h"
#include "drivers.h"

/*	
		Declare variables
 */
//extern volatile uint16 count=0;


#define FIFO_SIZE 512




#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)

volatile uint8 tx_char;			//This char is the most current char to go into the UART

volatile uint16 rx_flag;			//Mailbox Flag for the rx_char.
volatile uint8 rx_char;			//This char is the most current char to come out of the UART

char tx_fifo[UART_TX_BUFFER_SIZE];	//The array for the tx fifo
char rx_fifo[UART_RX_BUFFER_SIZE];    //The array for the rx fifo

volatile uint16 tx_fifo_head;			//Theses pointers keep track where the UART and the Main program are in the Fifos
volatile uint16 tx_fifo_tail;
volatile uint16 rx_fifo_head;
volatile uint16 rx_fifo_tail;

volatile uint16 rx_fifo_full;
volatile uint16 tx_fifo_full;

volatile uint16 UART_NewEvent;



extern volatile uint16 iii;

 

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
	//Timer_Initialization();
	PWM_Initialization();
//	GPIO_Initialization();
}


// void Timer_Initialization(void) {
// 	// Timer
// 	// Count up mode
// 	// Using interrupt
// 	// Timing 1ms
// 	TA0CTL = 0;					// Stop all

// 	//TA0CTL = TASSEL_1 | ID_0;	// ACLK + Div 1 and Stop mode

// 	TA0CTL |= TACLR;			// Clear all to start with

// 	TA0CCR0 = (FREQ / TICK_RATE_KHZ) - 1;

// 	TACCTL0 |= CCIE;				// Enable Interrupt


// 	TA0CTL |= TASSEL_2 | MC_1 | ID_0;	// Up mode and start timer
// }

/*
	PWM frequency 	: 20kHz ~ 50us
	Timer frequende : 8Mhz/4 ~ 0.5us
	=> CCRx = (50us/0.5us) - 1 = 99
	

*/
void PWM_Initialization(void) {
	P1DIR |= BIT6;                            // P1.2 and P1.3 output
  	P1SEL |= BIT6;                            // P1.2 and P1.3 TA1/2 options
  	TA0CTL = 0;								  // Stop all
  	TA0CTL |= TACLR;						  // Clear all to start with
  	CCR0 = 9999;                                // CCR0 is period---------200Hz
  	  	  	  	  	  	  	  	  	  	  	  // See p365, slau144j
  	CCTL1 |= OUTMOD_7 + CCIE;                  // CCR1 reset/set
}



//void GPIO_Initialization(void) {
//    LED_DIR = LED_1 + LED_2;			//Enable LED
//    LED_OUT = LED_1 + LED_2;			//All off
//}


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

	tx_fifo_head = 0;					//Set the fifo pointers to 0
	tx_fifo_tail = 0;
	rx_fifo_head = 0;
	rx_fifo_tail = 0;

	//tx_fifo_full = 0;
	//rx_fifo_full = 0;

}

/*****************************************************************************/

/*uart_getc
* Get a char from the UART. Waits till it gets one
* INPUT: None
* RETURN: Char from UART
*/
uint8 uart_getc()					//Waits for a valid char from the UART
{
	uint8 c;
	if(rx_fifo_head == rx_fifo_tail)	// Check: Is Head = Tail?
	{
		c = '\0';
	}
	else
	{
		rx_fifo_tail = (rx_fifo_tail + 1) & UART_RX_BUFFER_MASK;
		c = rx_fifo[rx_fifo_tail];
	}	
    return c;
}

/*	$description: Get a string of known length from the UART. 
				  Strings terminate when enter is pressed or string buffer fills
				  Will return when all the chars are received or a carriage return (\r) is received. 
				  Waits for the data.
 * 	$function name:	UART_Gets
 * 	$prams: Array pointer and length
 * 	$return: None
 */
/*
 void uart_gets(char* Array, int length)
{
	uint16 i = 0;

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
*/


/*	$description: Sends a char to the UART. Will wait if the UART is busy
 * 	$function name:	UART_Putc
 * 	$prams: Char to send
 * 	$return: none
 */
void uart_putc(uint8 c)
{
	uint8 tmpHead;			// A temp pointer point to Head
	
	tmpHead = (tx_fifo_head + 1) & UART_TX_BUFFER_MASK;
	while(tmpHead == tx_fifo_tail);	// wait for free space
	
	tx_fifo[tmpHead] = c;			// Put data to buffer
	tx_fifo_head = tmpHead;			// Now, move pointer to new Head
	
	IE2 |= UCA0TXIE; 				//Enable USCI_A0 TX interrupt
}


/*	$description: Sends a string to the UART. Will wait if the UART is busy
 * 	$function name:	UART_Puts
 * 	$prams: Pointer to String to send
 * 	$return: none
 */
void uart_puts(char *str)						//Sends a String to the UART.
{
     //while(*str) uart_putc(*str++);			//Advance though string till end
    uint8 tmpHead;
	while(*str)
	{
		tmpHead = (tx_fifo_head + 1) & UART_TX_BUFFER_MASK;	// Get new head
		while(tmpHead == tx_fifo_tail);			//wait for free space in buffer 
		
		tx_fifo[tmpHead] = *str++;				// Now moving to new data
		tx_fifo_head = tmpHead;					// Moving to new position 
	}
	// Enable TX interrupt
	IE2 |= UCA0TXIE;
}




/******************************************************************************
*******************************************************************************
							INTERRUPT ROUTINE
*******************************************************************************
******************************************************************************/

/*	
	Interrupt routines
*/

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER_INTERRUPT(void)
{
	switch(TAIV)
	{
		case 2:
			//count++;
			TACTL = 0;	// Stop
			break;
		case 10:
			break;
		default:
			break;

	}
}


#pragma vector = USCIAB0TX_VECTOR			//UART TX USCI Interrupt
__interrupt void USCI0TX_ISR(void)
{
	if(tx_fifo_head != tx_fifo_tail)
	{
		tx_fifo_tail = (tx_fifo_tail + 1) & UART_TX_BUFFER_MASK;
		UCA0TXBUF = tx_fifo[tx_fifo_tail];	// Transmit data 		
	}
	else
	{
		IE2 &= ~UCA0TXIE; 					//Turn off the interrupt to save CPUIE2 &= ~UCA0TXIE; 					//Turn off the interrupt to save CPU
	}  
}

#pragma vector = USCIAB0RX_VECTOR		//UART RX USCI Interrupt. This triggers when the USCI receives a char.
__interrupt void USCI0RX_ISR(void)
{
	//rx_char = UCA0RXBUF;				//Copy from RX buffer, in doing so we ACK the interrupt as well
	
	rx_fifo_head = (rx_fifo_head + 1) & UART_RX_BUFFER_MASK;
	rx_fifo[rx_fifo_head] = UCA0RXBUF;
	
	if(rx_fifo_head ==  rx_fifo_tail)
	{
		//UART_BUFFER_OVERFLOW
	}
	
	UART_NewEvent = TRUE;
}
