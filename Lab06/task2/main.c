/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Message Queue Example

*----------------------------------------------------------------------------*/
#include "STM32F10x.h"
#include "cmsis_os.h"
#include "uart.h"
#include "Board_LED.h"

#define  LED_0		0					//this is connected to GPIOB pin 8  (PORTB.8)
#define  LED_1    1
#define  LED_2    2

//osMessageQDef (Q_LED,0x16,unsigned char);
//osEvent  result;

//define message queue for uart
osMessageQId (Q_UART1);
osMessageQDef (Q_UART1,0x16,unsigned char);

osMutexId led_mutex;
osMutexDef(led_mutex);

void Thread1 (void const *argument);																
void Thread2 (void const *argument);
void Thread3 (void const *argument);

osThreadDef(Thread1, osPriorityAboveNormal, 1, 0);
osThreadDef(Thread2, osPriorityNormal, 1, 0);
osThreadDef(Thread3, osPriorityNormal, 1, 0);

osThreadId T_Thread1;
osThreadId T_Thread2;
osThreadId T_Thread3;
osThreadId	T_main; 


/*----------------------------------------------------------------------------
  Task 2 'ledOff': switches the LED off
 *---------------------------------------------------------------------------*/
void Thread1 (void const *argument) 
{
	for (;;) 
	{
		//result = 	osMessageGet(Q_UART1,osWaitForever);				//wait for a message to arrive
		uint8_t userInput = osMessageGet(Q_UART1,osWaitForever).value.v;
		SendChar(userInput);
		switch(userInput)
		{
			case '0':
				osSignalSet	(T_Thread2,0x01);
				osSignalSet	(T_Thread3,0x02);
			break;
			case '1':
				osSignalSet	(T_Thread3,0x01);
				osSignalSet	(T_Thread2,0x02);
			break;
		}
	}
}
/*----------------------------------------------------------------------------
  Task 1 'ledOn': switches the LED on
 *---------------------------------------------------------------------------*/
void Thread2 (void const *argument) 
{
	int state = 0;
	osEvent  result;
	for (;;) 
	{
		result = osSignalWait (0x00,0);
		switch(result.value.v)
		{
			case 1:
				state = 1;
				break;
			case 2:
				state = 2;
				break;
			default:
				break;
		}
		switch(state)
		{
			case 1:
				osMutexWait(led_mutex, osWaitForever);
				LED_Off(LED_0);
				LED_Off(LED_1);
				LED_On(LED_2);
				osDelay(3000);
				LED_Off(LED_0);
				LED_On(LED_1);
				LED_Off(LED_2);
				osDelay(1000);
				LED_On(LED_0);
				LED_Off(LED_1);
				LED_Off(LED_2);
				osDelay(3000);
			  osMutexRelease(led_mutex);
				break;
			case 2:
			default:
				osSignalWait (0x01,osWaitForever);
				state = 1;
				break;
		}
	}
}

void Thread3 (void const *argument) 
{
	int state = 0;
	osEvent  result;
	for (;;) 
	{
		result = osSignalWait (0x00,0);
		switch(result.value.v)
		{
			case 1:
				state = 1;
				break;
			case 2:
				state = 2;
				break;
			default:
				break;
		}
		switch(state)
		{
			case 1:
				osMutexWait(led_mutex, osWaitForever);
				LED_On(LED_0);
				LED_Off(LED_1);
				LED_Off(LED_2);
				osDelay(1000);
				LED_Off(LED_0);
				osDelay(1000);
			  osMutexRelease(led_mutex);
				break;
			case 2:
			default:
				osSignalWait (0x01,osWaitForever);
				state = 1;
				break;
		}
	}
}

void USART1_IRQHandler (void) 
	{
	uint8_t intKey = (int8_t) (USART1->DR & 0x1FF);
	osMessagePut(Q_UART1, intKey, 0);
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
	NVIC->IP[USART1_IRQn] = 0x80;	//set priority to 0x80
	NVIC->ISER[USART1_IRQn/32] = 1UL << (USART1_IRQn%32);
	//set interrupt enable bit
	USART1->CR1 |= USART_CR1_RXNEIE;//enable USART receiver not empty interrupt
	
	LED_Initialize ();
	led_mutex = osMutexCreate(osMutex(led_mutex));
	Q_UART1 = osMessageCreate(osMessageQ(Q_UART1),NULL);					//create the message queue
	T_Thread1 = osThreadCreate(osThread(Thread1), NULL);
	T_Thread2 =	osThreadCreate(osThread(Thread2), NULL);
	T_Thread3 =	osThreadCreate(osThread(Thread3), NULL);
	
	osKernelStart ();                         						// start thread execution 
}
