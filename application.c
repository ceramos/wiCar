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
#include "typedef.h"
#include <stdio.h>
#include <string.h>
#include "drivers.h"
#include "application.h"

/*	
		Declare variables
*/

extern char tx_fifo[UART_TX_BUFFER_SIZE];	//The array for the tx fifo
extern char rx_fifo[UART_RX_BUFFER_SIZE];    //The array for the rx fifo

extern volatile uint8 tx_char;
extern volatile uint8 rx_char;
extern volatile uint16 UART_NewEvent;

char  wifi_buff[512];
unsigned char	wifi_esp;
uint16 	count=0;


/*
		Function define
*/

/*
	This function is used to generate PWM pulse
	Params:	duty cycle : 0 - 100%
	Return: None
*/
void PWM_Out(uint16 duty) {
	// Check
	if((duty < 100) && (duty > 0)) {
		CCR1 = duty;
		TA0CTL |= TASSEL_2 + MC_1 + ID_2 + TAIE;           // SMCLK, Div 4 ,
	}
}

/*****************************************************************************/
void ESP_Setup(void) {
	uint8 OK = FALSE;
	char *oks = "OK";
	char *nc  = "no change";
	int mem_loop;
	//char *pch;
	
	// 
	do 
	{
		uart_puts("AT+RST");
		uart_puts("\r\n");
		//delay_ms(4000);		// waiting for answer
		
		uart_puts("ATE0");
		uart_puts("\r\n");
		//delay_ms(1000);		// waiting for answer
		
		for(mem_loop = 0; mem_loop <= 150; mem_loop++) 
		{
			while(UART_NewEvent == 0);	// waiting for new event
			
			if(UART_NewEvent != 0) 
			{
				rx_char = uart_getc();
				wifi_buff[count] = rx_char;
				count++;
				if(count > 150)
				{
					count = 0;
				}
			}
		}
		
		char *pch = strstr(wifi_buff,oks);	// Copy string
		if(pch)
		{
			OK = TRUE;
		}
		//delay_ms(100);
	} while(!OK);
	
	
	do
	{
		uart_puts("AT+CWMODE=3");
		uart_puts("\r\n");
		for(mem_loop = 0; mem_loop < 150; mem_loop++)
		{
			while(UART_NewEvent==0);		// Standing here and waiting for new event
			if(UART_NewEvent != 0)
			{
				rx_char = uart_getc();		// Get data
				wifi_buff[count] = rx_char;	// Moving to buff
				count++;
				if(count > 150) 
				{
					count = 0;				// Reset counter
				}
			}
		}
		
		char *pch = strstr(wifi_buff,oks);
		if(pch)
		{
			OK = TRUE;
		}
		else
		{
			char *no_change = strstr(rx_fifo,nc);
			if(no_change)
			{
				OK = TRUE;
			}
		}
		//delay_ms(100);
	}
	while(!OK);
}

void Join_AP(void)
{
	uint8 OK = FALSE;
	char *oks = "OK";
	uint16 loop;
	uint16 mem_loop;
	do
	{
		for(loop = 0; loop < 3; loop++)
		{
			uart_puts("AT+CWJAP");
			uart_puts("=\"Connectify-TD\"");
			uart_puts(",\"");
			uart_puts("016825982723");
			uart_puts("\"");
			uart_puts("\r\n");
			
			//delay_ms(2000);
			
			long long timeout = 0;
		    while((++timeout < 30000)) //30 second
			{
				//delay_us(10);
			}
			
			for(mem_loop = 0; mem_loop < 512; mem_loop++)
			{
				while(UART_NewEvent == 0);		// If no new event; standing here
				
				if(UART_NewEvent != 0)
				{
					rx_char = uart_getc();
					wifi_buff[count] = rx_char;
					count++;
					if(count > 512)
					{
						count = 0;
					}
				}
				
			}
			
			char *pch = strstr(wifi_buff,oks);
			
			if(pch)
			{
				OK = TRUE;
				break;
			}
			//delay_ms(100);
		}
	} while(!OK);
}














