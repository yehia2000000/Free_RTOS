/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "semphr.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
	

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
*/

typedef struct 
{
	signed char * ID ;
	signed char * Data ; 
	
	
}Queue_t;

QueueHandle_t  xQueue ; 
pinState_t Button1_State = PIN_IS_HIGH ;
pinState_t Button2_State = PIN_IS_HIGH ;

void Tk_Button2 (void * pvParameters )
{

	Queue_t Queue_B2 = {(signed char *)"None",(signed char *)"None"};
	for (;;)
	{
		if ((Button2_State^GPIO_read(PORT_0,PIN1)) == 1)
		{
			Button2_State = GPIO_read(PORT_0,PIN1);
			if (Button2_State == PIN_IS_HIGH )
			{
				Queue_B2.ID = (signed char *)"button2";
				Queue_B2.Data = (signed char *)"rising";
				xQueueSend(xQueue,&Queue_B2,(TickType_t)10);
			}
			else 
			{
				Queue_B2.ID = (signed char *)"button2";
				Queue_B2.Data = (signed char *)"Falling";
				xQueueSend(xQueue,&Queue_B2,(TickType_t)10);
			}
		}
		vTaskDelay(100);
	}
	
}
void Tk_Button1 (void * pvParameters )
{
	Queue_t Queue_B1 = {(signed char *)"None",(signed char *)"None"};
	for (;;)
	{
		if ((Button1_State^GPIO_read(PORT_0,PIN0)) == 1)
		{
			Button1_State = GPIO_read(PORT_0,PIN0);
			if (Button1_State == PIN_IS_HIGH )
			{
				Queue_B1.ID = (signed char *)"button1";
				Queue_B1.Data = (signed char *)"rising";
				xQueueSend(xQueue,&Queue_B1,(TickType_t)10);
			}
			else 
			{
				Queue_B1.ID = (signed char *)"button1";
				Queue_B1.Data = (signed char *)"Falling";
				xQueueSend(xQueue,&Queue_B1,(TickType_t)10);
			}
		}
		vTaskDelay(200);
	}
	
}

void Tk_UartWrite (void * pvParameters )
{
		Queue_t Rev_Queue1 = {(signed char *)"None",(signed char *)"None"};
		int i ;
		xQueue = xQueueCreate (10,sizeof(Queue_t));
for (;;)
{
if (xQueueReceive(xQueue ,&Rev_Queue1,(TickType_t)10)==pdPASS)
{
	vSerialPutString(Rev_Queue1.ID,7);
	for( i=0 ; i <100000;i++);
	vSerialPutString((signed char *)":",1);
	for( i=0 ; i <100000;i++);
	vSerialPutString(Rev_Queue1.Data,6);
	for(i=0 ; i <100000;i++);
	vSerialPutString((signed char *)"/",1);
	for(i=0 ; i <100000;i++);
	
	vTaskDelay(300);
}

}
	
}
void Tk_SendString (void * pvParameters )
{
		Queue_t Queue_S = {(signed char *)"None",(signed char *)"None"};
for (;;)
{
			Queue_S.ID = (signed char *)"TaskS";
			Queue_S.Data = (signed char *)"yehia";
			
			xQueueSend(xQueue,&Queue_S,(TickType_t)10);
	
vTaskDelay(300);

}	
	
}


TaskHandle_t Tk_Button2_Ha = NULL;
TaskHandle_t Tk_Button1_Ha =NULL;
TaskHandle_t Tk_UartWrite_Ha =NULL;
TaskHandle_t Tk_SendString_Ha = NULL;

int main( void )
 {
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	xTaskCreate(
                    Tk_Button2,       /* Function that implements the task. */
                    "Tk_UartWrite",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &Tk_Button2_Ha );      /* Used to pass out the created task's handle. */\
										
	xTaskCreate(
                    Tk_Button1,       /* Function that implements the task. */
                    "Tk_Button1",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    2,/* Priority at which the task is created. */
                    &Tk_Button1_Ha );      /* Used to pass out the created task's handle. */
										
	xTaskCreate(
                    Tk_SendString,       /* Function that implements the task. */
                    "Tk_SendString",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    3,/* Priority at which the task is created. */
                    &Tk_SendString_Ha );      /* Used to pass out the created task's handle. */
									
	xTaskCreate(
                    Tk_UartWrite,       /* Function that implements the task. */
                    "Tk_UartWrite",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    4,/* Priority at which the task is created. */
                    &Tk_UartWrite_Ha );      /* Used to pass out the created task's handle. */
	
	
   /* Create Tasks here */


	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


