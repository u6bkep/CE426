
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stm32f10x.h> 

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
void Thread (void const *argument);                             // thread function
osThreadId tid_Thread;                                          // thread id
osThreadDef (Thread, osPriorityNormal, 1, 0);                   // thread object

int Init_Thread (void) {

  tid_Thread = osThreadCreate (osThread(Thread), NULL);
  if (!tid_Thread) return(-1);
  
  return(0);
}

void Thread (void const *argument) {

  while (1) {
    __NOP(); // Insert thread code here...
    osThreadYield ();                                           // suspend thread
  }
}
