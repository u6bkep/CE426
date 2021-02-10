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
#define  LED_1    1
#define  LED_2    2

void delay (uint32_t count);
void SendText(uint8_t *txt);

volatile int intKey = 0;

/*-----------------------------------------------------------------------------
  USART1 IRQ Handler
       The hardware automatically clears the interrupt flag, once the ISR is entered
*----------------------------------------------------------------------------*/
       
void USART1_IRQHandler (void) 
{
	intKey = (int16_t) (USART1->DR & 0x1FF);
}


int main()
{
		uint8_t		inKey;
	int state = 0;
	
		LED_Initialize();
		USART1_Init();
	
	USART1_Init ();
  //Configure and enable USART1 interrupt
  NVIC->IP[USART1_IRQn] = 0x80;        //set priority to 0x80
  //NVIC_SetPriority(USART1_IRQn, 0x80);
  NVIC->ISER[USART1_IRQn/32] = 1UL << (USART1_IRQn%32);     //set interrupt enable bit
  //NVIC_EnableIRQ(USART1_IRQn);
  USART1->CR1 |= USART_CR1_RXNEIE;     //enable receiver not empty interrupt

	
		//send a character to UART for testing
		SendChar('H');			
		SendChar('i');
		SendChar('\n');			//send newline character
	
		//use your function SendText to send a text string to the UART
		SendText("Welcome!\n");	
	
		while (1) {
			if(intKey < 0x1FF)
			{
				inKey = (char) intKey;			//get input character from the UART
				intKey = 0xffff;
				SendChar(inKey);
				switch(inKey)
				{
					case '0':
						state = 2;
						break;
					case '1':
						state = 1;
						break;
					
					default:
						break;
				}
			}
			
      switch(state)
			{
				case 0:
					
				  break;
				case 1:										//normal mode
					LED_On(LED_0);
				  LED_Off(LED_1);
				  LED_Off(LED_2);
				  delay(3000);
				  LED_Off(LED_0);
				  LED_On(LED_1);
				  LED_Off(LED_2);
				  delay(1000);
				  LED_Off(LED_0);
				  LED_Off(LED_1);
				  LED_On(LED_2);
				  delay(3000);
				  break;
				case 2:										//overide mode
					LED_On(LED_0);
				  LED_Off(LED_1);
				  LED_Off(LED_2);
				  delay(1000);
				  LED_Off(LED_0);
				  delay(1000);
					break;
			}
		}
	}
//}	


/*----------------------------------------------------------------------------
  Simple delay routine
*---------------------------------------------------------------------------*/
void delay (uint32_t count)
{
	uint32_t index1, index2;

	for(index1 =0; index1 < count; index1++)
	{
			for (index2 = 0; index2 < 1000; index2++)__NOP();	
	}
}

//complete this function for sending a string of characters to the UART
void SendText(uint8_t *text) {
	char currentChar = text[0];
	int i = 0;
	while(currentChar != 0)
	{
		SendChar(currentChar);
		i++;
		currentChar = text[i];
	}
	
}
