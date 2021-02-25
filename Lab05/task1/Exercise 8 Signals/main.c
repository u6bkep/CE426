/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Signal Example

*----------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <cmsis_os.h>
#include "Board_LED.h"
#include "uart.h"

void greenThread (void const *argument);
void yellowThread (void const *argument);
void redThread (void const *argument);

osThreadDef(greenThread, osPriorityNormal, 1, 0);
osThreadDef(yellowThread, osPriorityNormal, 1, 0);
osThreadDef(redThread, osPriorityNormal, 1, 0);

osThreadId T_greenThread_ID1;
osThreadId T_yellowThread_ID2;
osThreadId T_redThread_ID3;

osMutexId uart_mutex;
osMutexDef(uart_mutex);

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
void delay (void)
{
unsigned int index;
const unsigned int count = 1000000;
	for(index =0;index<count;index++)
	{
		;
	}
}
void greenThread (void const *argument) 
{
	for (;;) 
	{
		SendText("Green Thread Running...");
		SendText("GREEN LED ON\n\r");
		osSignalWait (0x00,osWaitForever);
		LED_On(2);
		osMutexRelease(uart_mutex);
		osDelay(3000);
		LED_Off(2);
		osSignalSet	(T_yellowThread_ID2,0x01);
		osMutexWait(uart_mutex, osWaitForever);
	}
}
void yellowThread (void const *argument) 
{
	for (;;) 
	{
		osMutexWait(uart_mutex, osWaitForever);
		SendText("Yellow Thread Running...");
		SendText("Yellow LED ON\n\r");
		osSignalWait (0x00,osWaitForever);
		LED_On(1);
		osMutexRelease(uart_mutex);
		osDelay(1000);
		LED_Off(1);
		osSignalSet	(T_redThread_ID3,0x01);
	}
}
void redThread (void const *argument) 
{
	for (;;) 
	{
		osMutexWait(uart_mutex, osWaitForever);
		SendText("Red Thread Running...");
		SendText("red LED ON\n\r");
		osSignalWait (0x00,osWaitForever);
		LED_On(0);
		osMutexRelease(uart_mutex);
    osDelay(3000);
		LED_Off(0);
		osSignalSet	(T_greenThread_ID1,0x01);
	}
}
/*----------------------------------------------------------------------------
  Start the threads
 *---------------------------------------------------------------------------*/
int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	LED_Initialize();
	USART1_Init ();
	uart_mutex = osMutexCreate(osMutex(uart_mutex));	
	T_yellowThread_ID2 = osThreadCreate(osThread(yellowThread), NULL);
	T_greenThread_ID1 = osThreadCreate(osThread(greenThread), NULL);
	T_redThread_ID3 = osThreadCreate(osThread(redThread), NULL);	
	osSignalSet	(T_greenThread_ID1,0x01);
	osKernelStart ();                         // start thread execution 
}
