/* 
   Sample program 2 – Hello World for Embedded Systems
   Toggle an LED attached to a GPIO pin
	 Use UART inputs to control the operating mode of the program
*/

#include <stm32f10x.h> 
#include "GPIO_STM32F10x.h"
#include "Board_LED.h"
#include "uart.h"

#define  LED_0		0					//this is connected to GPIOB pin 8  (PORTB.8)

void delay (uint32_t count);
void SendText(uint8_t *txt);

int main()
{
		uint8_t		inKey;
	
		LED_Initialize();
		USART1_Init();
	
		//send a character to UART for testing
		SendChar('H');			
		SendChar('i');
		SendChar('\n');			//send newline character
	
		//use your function SendText to send a text string to the UART
		SendText("Welcome!\n");	
	
		while (1) {
			inKey = GetKey();			//get input character from the UART
			
			//include a command here to echo the input you received back to the terminal window
		
			if (inKey == '1') {		//toggle LED_0 for one cycle and leave it on
				LED_On(LED_0);
				delay(1000);
				LED_Off(LED_0);
				delay(1000);
				LED_On(LED_0);
			}
			else {
				LED_Off(LED_0);		//turn off LED_0
			}
		}
}	


/*----------------------------------------------------------------------------
  Simple delay routine
*---------------------------------------------------------------------------*/
void delay (uint32_t count)
{
	uint32_t index1, index2;

	for(index1 =0; index1 < count; index1++)
	{
			for (index2 = 0; index2 < 1000; index2++);	
	}
}

//complete this function for sending a string of characters to the UART
void SendText(uint8_t *text) {
	
	
}
