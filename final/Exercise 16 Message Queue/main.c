/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Message Queue Example

*----------------------------------------------------------------------------*/
#include "STM32F10x.h"
#include "cmsis_os.h"

#include "uart.h"

osMessageQId Q_uartQ1;																		//define the message queue
osMessageQDef (Q_uartQ1,0x16,unsigned char);
osMessageQId Q_uartQ2;
osMessageQDef (Q_uartQ2,0x16,unsigned char);

osEvent  result;


void uart1Thread (void const *argument);																
void uart2Thread (void const *argument);
void messageHandlerThread (void const *argument);

osThreadDef(uart1Thread, osPriorityAboveNormal, 1, 0);
osThreadDef(uart2Thread, osPriorityAboveNormal, 1, 0);
osThreadDef(messageHandlerThread, osPriorityNormal, 1, 0);

osThreadId T_uart1Thread;
osThreadId T_uart2Thread;
osThreadId T_messageHandlerThread;
osThreadId	T_main; 

typedef struct {
  uint32_t length;
  uint32_t messagesRemaining;
	uint32_t uartID;
  char message[32];
} messageMail_t;

osMailQDef (messageMail_pool_q, 10, messageMail_t);  // Declare mail queue
osMailQId  (messageMail_pool_q_id);                 // Mail queue ID


void uart1Thread (void const *argument) 
{
	//gets messages from the uart IRQ and collects them into larger messages for the handler thread
	for (;;)
    {
        // get input from uartIRQ and command worker threads appropriately
        uint8_t userInput = osMessageGet(Q_uartQ1,osWaitForever).value.v;
        SendChar(userInput);
		}
}

void uart2Thread (void const *argument) 
{
	//gets messages from the uart IRQ and collects them into larger messages for the handler thread
	for (;;)
    {
        // get input from uartIRQ and command worker threads appropriately
        uint8_t userInput = osMessageGet(Q_uartQ2,osWaitForever).value.v;
        SendChar(userInput);
		}
}

void messageHandlerThread (void const *argument) 
{
	//recieves messages from the uart recieve worker threads and sends messages out over the proper uart channels
}

void USART1_IRQHandler (void)
    {
        //send uart messages to message queue
    uint8_t intKey = (int8_t) (USART1->DR & 0x1FF);
    osMessagePut(Q_uartQ1, intKey, 0);
    }

void USART2_IRQHandler (void)
    {
        //send uart messages to message queue
    uint8_t intKey = (int8_t) (USART1->DR & 0x1FF);
    osMessagePut(Q_uartQ2, intKey, 0);
    }

		
/*----------------------------------------------------------------------------
  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/

int main (void) 
{
	osKernelInitialize ();                    						// initialize CMSIS-RTOS
	
	USART1_Init ();
    //configure USART interrupt ... so we can read user inputs using interrupt
    //Configure and enable USART1 interrupt
    NVIC->ICPR[USART1_IRQn/32] = 1UL << (USART1_IRQn%32);  //clear any previous pending interrupt flag
    NVIC->IP[USART1_IRQn] = 0x80;    //set priority to 0x80
    NVIC->ISER[USART1_IRQn/32] = 1UL << (USART1_IRQn%32);
    //set interrupt enable bit
    USART1->CR1 |= USART_CR1_RXNEIE;//enable USART receiver not empty interrupt
	
	//USART2_Init ();
    //configure USART interrupt ... so we can read user inputs using interrupt
    //Configure and enable USART2 interrupt
    //NVIC->ICPR[USART2_IRQn/32] = 1UL << (USART1_IRQn%32);  //clear any previous pending interrupt flag
    //NVIC->IP[USART2_IRQn] = 0x80;    //set priority to 0x80
    //NVIC->ISER[USART2_IRQn/32] = 1UL << (USART1_IRQn%32);
    //set interrupt enable bit
    //USART1->CR1 |= USART_CR1_RXNEIE;//enable USART receiver not empty interrupt
	
	
	Q_uartQ1 = osMessageCreate(osMessageQ(Q_uartQ1),NULL);					//create the message queue
	Q_uartQ2 = osMessageCreate(osMessageQ(Q_uartQ2),NULL);
	messageMail_pool_q_id = osMailCreate(osMailQ(messageMail_pool_q), NULL);
	
	T_uart1Thread = osThreadCreate(osThread(uart1Thread), NULL);
	T_uart2Thread =	osThreadCreate(osThread(uart2Thread), NULL);
	
	osKernelStart ();                         						// start thread execution 
}
