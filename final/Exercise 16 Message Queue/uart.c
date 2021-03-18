#include "STM32F10x.h"
/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void USART1_Init (void) {
  int i;

  RCC->APB2ENR |=  (   1UL <<  0);        /* enable clock Alternate Function  */
  AFIO->MAPR   &= ~(   1UL <<  2);        /* clear USART1 remap               */

  RCC->APB2ENR |=  (   1UL <<  2);        /* enable GPIOA clock               */
  GPIOA->CRH   &= ~(0xFFUL <<  4);        /* clear PA9, PA10                  */
  GPIOA->CRH   |=  (0x0BUL <<  4);        /* USART1 Tx (PA9) output push-pull */
  GPIOA->CRH   |=  (0x04UL <<  8);        /* USART1 Rx (PA10) input floating  */

  RCC->APB2ENR |=  (   1UL << 14);        /* enable USART#1 clock             */

  USART1->BRR   = 0x0271;                 /* 115200 baud @ PCLK2 72MHz        */
  USART1->CR1   = ((   1UL <<  2) |       /* enable RX                        */
                   (   1UL <<  3) |       /* enable TX                        */
                   (   0UL << 12) );      /* 1 start bit, 8 data bits         */
  USART1->CR2   = 0x0000;                 /* 1 stop bit                       */
  USART1->CR3   = 0x0000;                 /* no flow control                  */
  for (i = 0; i < 0x1000; i++) __NOP();   /* avoid unwanted output            */

  USART1->CR1  |= ((   1UL << 13) );      /* enable USART                     */
}

void USART2_Init (void)
{
	RCC->APB2ENR |= 1;	// enable clock for AF
	AFIO->MAPR |= 1<<3; 	// set USART2 remap
	RCC->APB2ENR |= 1<<5; 	// enable clock for GPIOD
	GPIOD->CRL &= ~(0xFF << 20); 	// Clear PD5, PD6
	GPIOD->CRL |= (0x0B << 20); 	// USART2 Tx (PD5) output push-pull
	GPIOD->CRL |= (0x04 << 24); 	// USART2 Rx (PD6) input floating
	RCC->APB1ENR |= 1<<17; 	// enable clock for USART2
	USART2->BRR = 0x138; 	// set baudrate -115.2kB from 36MHz
	USART2->CR1 &= ~(1<<12); 	// force 8 data bits
	USART2->CR2 &= ~(3<<12); 	//force 1 stop bit
	USART2->CR3 &= ~(3<<8); 	//force no flow control
	USART2->CR1 &= ~(3<<9); 	//force no parity
	USART2->CR1 |= 3<<2; 	// RX, TX enable
	USART2->CR1 |= 1<<5; 	// Rx interrupts if required
	NVIC->ISER[1] = (1 << 6); 	// enable interruptsif required
	USART2->CR1 |= 1<<13; 	// USART enable
}


/*----------------------------------------------------------------------------
  SendChar
  Write character to Serial Port.
 *----------------------------------------------------------------------------*/
int UART1SendChar (int ch)  {

  while (!(USART1->SR & USART_SR_TXE));
  USART1->DR = (ch & 0x1FF);

  return (ch);
}

int UART2SendChar (int ch)  {

  while (!(USART2->SR & USART_SR_TXE));
  USART2->DR = (ch & 0x1FF);

  return (ch);
}

/*----------------------------------------------------------------------------
  GetKey
  Read character to Serial Port.
 *----------------------------------------------------------------------------*/
int UART1GetKey (void)  {

  while (!(USART1->SR & USART_SR_RXNE));

  return ((int)(USART1->DR & 0x1FF));
}

int UART2GetKey (void)  {

  while (!(USART2->SR & USART_SR_RXNE));

  return ((int)(USART2->DR & 0x1FF));
}
