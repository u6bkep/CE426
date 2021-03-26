/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Message Queue Example

*----------------------------------------------------------------------------*/
#include "STM32F10x.h"
#include "cmsis_os.h"
#include "uart.h"
#include "stdio.h"

osMessageQId Q_uartQ1;																		//define the message queue
osMessageQDef (Q_uartQ1,0x16,unsigned char);
osMessageQId Q_uartQ2;
osMessageQDef (Q_uartQ2,0x16,unsigned char);

osEvent  result;

int messageSequenceNumber = 0;

void uart1Thread (void const *argument);																
void uart2Thread (void const *argument);
void periodicMessageSender (void const *argument);
void messageHandler (void const *argument);

osThreadDef(uart1Thread, osPriorityAboveNormal, 1, 0);
osThreadDef(uart2Thread, osPriorityAboveNormal, 1, 0);
osThreadDef(periodicMessageSender, osPriorityNormal, 1, 0);
osThreadDef(messageHandler, osPriorityNormal, 1, 0);

osThreadId T_uart1Thread;
osThreadId T_uart2Thread;
osThreadId T_periodicMessageSenderThread;
osThreadId T_messageHandlerThread;
osThreadId	T_main; 

osMutexId uart_mutex;
osMutexDef(uart_mutex);

#define MAX_MESSAGE_SIZE 32
#define MAIL_Q_SIZE 10

typedef struct {
  uint32_t length;
  uint32_t messagesRemaining;
	uint32_t uartID;
  char message[MAX_MESSAGE_SIZE];
} messageMail_t;

osMailQDef (messageMail_pool_q, MAIL_Q_SIZE, messageMail_t);  // Declare mail queue
osMailQId  (messageMail_pool_q_id);                 // Mail queue ID

void SendText1(char *text) {
	char USART1currentChar = text[0];
	int i = 0;
	while(USART1currentChar != 0)
	{
		USART1SendChar(USART1currentChar);
		i++;
		USART1currentChar = text[i];
	}
}

void SendText2(char *text) {
	char USART2currentChar = text[0];
	int i = 0;
	while(USART2currentChar != 0)
	{
		USART2SendChar(USART2currentChar);
		i++;
		USART2currentChar = text[i];
	}
}

void uart1Thread (void const *argument) 
{
	//gets messages from the uart IRQ and collects them into larger messages for the handler thread
  int readyToSend, messagesRemaining;
	messageMail_t *letter;
	for (;;)
    {
			letter = (messageMail_t*) osMailAlloc(messageMail_pool_q_id,osWaitForever);
			letter->length = 0;
			letter->messagesRemaining = 0;
			letter->uartID = 1;
			readyToSend = 0;
			messagesRemaining = 0;
			while(!readyToSend)
			{
				// get input from uartIRQ and command worker threads appropriately
				uint8_t userInput = osMessageGet(Q_uartQ1,osWaitForever).value.v;
				//osMutexWait(uart_mutex, osWaitForever);
			
        if(userInput == '\b' && letter->length != 0)
				{
					letter->length--;
				}
				else if((userInput == '\r' || userInput == '\n') && letter->length != 0)
				{
					readyToSend = 1;
					messagesRemaining = 0;
					letter->message[letter->length] = 0;
					USART1SendChar('\r'); USART1SendChar('\n');
				}
				else
				{
					letter->message[letter->length] = userInput;
					letter->length++;
				  USART1SendChar(userInput);
					if(MAX_MESSAGE_SIZE - 2 == letter->length)
					{
						readyToSend = 1;
						messagesRemaining++;
						letter->messagesRemaining = 1;
						letter->message[MAX_MESSAGE_SIZE -1] = 0;
					}
				}
				
			}
			//osMutexRelease(uart_mutex);
			osMailPut(messageMail_pool_q_id, letter);
			
			readyToSend = 0;
        
		}
}

void uart2Thread (void const *argument) 
{
	//gets messages from the uart IRQ and collects them into larger messages for the handler thread
  int readyToSend, messagesRemaining;
	messageMail_t *letter;
	for (;;)
    {
			letter = (messageMail_t*) osMailAlloc(messageMail_pool_q_id,osWaitForever);
			letter->length = 0;
			letter->messagesRemaining = 0;
			letter->uartID = 2;
			readyToSend = 0;
			messagesRemaining = 0;
			while(!readyToSend)
			{
				// get input from uartIRQ and command worker threads appropriately
				uint8_t userInput = osMessageGet(Q_uartQ2,osWaitForever).value.v;
				//osMutexWait(uart_mutex, osWaitForever);
			
        if(userInput == '\b' && letter->length != 0)
				{
					letter->length--;
				}
				else if((userInput == '\r' || userInput == '\n') && letter->length != 0)
				{
					readyToSend = 1;
					messagesRemaining = 0;
					letter->message[letter->length] = 0;
					USART2SendChar('\r'); USART2SendChar('\n');
				}
				else
				{
					letter->message[letter->length] = userInput;
					letter->length++;
				  USART2SendChar(userInput);
					if(MAX_MESSAGE_SIZE - 2 == letter->length)
					{
						readyToSend = 1;
						messagesRemaining++;
						letter->messagesRemaining = 1;
						letter->message[MAX_MESSAGE_SIZE -1] = 0;
					}
				}
				
			}
			//osMutexRelease(uart_mutex);
			osMailPut(messageMail_pool_q_id, letter);
			
			readyToSend = 0;
	}
}

void messageHandler (void const *argument)
{
	messageMail_t * mail[MAIL_Q_SIZE];
	int currentMailQIndex = 0;
	int i;
	while(1)
	{
		osEvent event = osMailGet(messageMail_pool_q_id, osWaitForever);
		mail[currentMailQIndex] = (messageMail_t *) event.value.p;
		if(mail[currentMailQIndex]->messagesRemaining)
		{
			currentMailQIndex++;
		}
		else
		{
			osMutexWait(uart_mutex, osWaitForever);
			
			
				switch(mail[currentMailQIndex]->uartID)
				{
					case 1:
						SendText2(">>>");
						for(i = 0; i <= currentMailQIndex; i++)
						{
							SendText2(mail[i]->message);
							osMailFree(messageMail_pool_q_id, mail[i]);
						}
						USART2SendChar('\r'); USART2SendChar('\n');
						break;
					case 2:
						SendText1(">>>");
						for(i = 0; i <= currentMailQIndex; i++)
						{
							SendText1(mail[i]->message);
							osMailFree(messageMail_pool_q_id, mail[i]);
						}
						USART1SendChar('\r'); USART1SendChar('\n');
						break;
				}
			
			messageSequenceNumber++;
			currentMailQIndex = 0;
			osMutexRelease(uart_mutex);
		}
	}
}

void periodicMessageSender (void const *argument) {
	char str[30];
	for (;;) {
		osMutexWait(uart_mutex, osWaitForever);
		
		sprintf(str, "+++sequence number: %d\n", messageSequenceNumber);
		SendText1(str);
		
		SendText2(str);
		osMutexRelease(uart_mutex);  
		osDelay(40000);
    }
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
    uint8_t intKey = (int8_t) (USART2->DR & 0x1FF);
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
	
	  USART2_Init ();
		//configure USART interrupt ... so we can read user inputs using interrupt
    //Configure and enable USART1 interrupt
    NVIC->ICPR[USART2_IRQn/32] = 1UL << (USART2_IRQn%32);  //clear any previous pending interrupt flag
    NVIC->IP[USART2_IRQn] = 0x80;    //set priority to 0x80
    NVIC->ISER[USART2_IRQn/32] = 1UL << (USART2_IRQn%32);
    //set interrupt enable bit
    USART2->CR1 |= USART_CR1_RXNEIE;//enable USART receiver not empty interrupt
	
	
	Q_uartQ1 = osMessageCreate(osMessageQ(Q_uartQ1),NULL);					//create the message queue
	Q_uartQ2 = osMessageCreate(osMessageQ(Q_uartQ2),NULL);
	messageMail_pool_q_id = osMailCreate(osMailQ(messageMail_pool_q), NULL);
	
	uart_mutex = osMutexCreate(osMutex(uart_mutex));
	T_uart1Thread = osThreadCreate(osThread(uart1Thread), NULL);
	T_uart2Thread =	osThreadCreate(osThread(uart2Thread), NULL);
	T_messageHandlerThread = osThreadCreate(osThread(messageHandler), NULL);
	T_periodicMessageSenderThread = osThreadCreate(osThread(periodicMessageSender), NULL);
	
	osKernelStart ();                         						// start thread execution 
}
