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

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
	
typedef enum 
{
	LED_OFF,
	LED_400MS, 
	LED_100MS
	
}LED_STATE;


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/
volatile LED_STATE LED1_STATE = LED_400MS ; 
volatile pinState_t Button_State = PIN_IS_LOW ; 
volatile unsigned int Counter_Press = 0 ;

void Button_voidRead (void * pvParameters )
{
	
	for (;;)
	{
		 Button_State = GPIO_read(PORT_0,PIN0);
	 if ((Button_State == PIN_IS_LOW )&& (Counter_Press !=0))
	 {
		 if (Counter_Press <2)
		 {
			 LED1_STATE = LED_OFF ; 
		 }
		 else if ( (Counter_Press >2 ) && (Counter_Press <4))
		 {
			 LED1_STATE = LED_400MS;
		 }
		 else
		 {
			 LED1_STATE = LED_100MS;
		 }
		Counter_Press =0 ;
			 
	 }
	 else if (Button_State == PIN_IS_HIGH)
		  Counter_Press++;
	 vTaskDelay(100);
 }
	 
}

void Toggle_voidLed ( void * pvParameters )
{
    for( ;; )
    {
			switch (LED1_STATE)
			{
				case LED_OFF :GPIO_write(PORT_0,PIN1,PIN_IS_LOW); break;
				case LED_400MS :GPIO_write(PORT_0,PIN1,PIN_IS_HIGH); vTaskDelay(400);GPIO_write(PORT_0,PIN1,PIN_IS_LOW); vTaskDelay(400); break;
				case LED_100MS : GPIO_write(PORT_0,PIN1,PIN_IS_HIGH); vTaskDelay(100);GPIO_write(PORT_0,PIN1,PIN_IS_LOW); vTaskDelay(100);break;
				
			}
			 vTaskDelay(50); 

    }
}
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */

TaskHandle_t Global_ToggleLed = NULL ;
TaskHandle_t Global_Button = NULL ; 


int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	xTaskCreate(
                    Toggle_voidLed,       /* Function that implements the task. */
                    "Toggle",          /* Text name for the task. */
                    100,              /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &Global_ToggleLed ); 
										
xTaskCreate(
                   Button_voidRead,       /* Function that implements the task. */
                   "Button",          /* Text name for the task. */
                   100,              /* Stack size in words, not bytes. */
                   ( void * ) 1,    /* Parameter passed into the task. */
                   2,/* Priority at which the task is created. */
                   &Global_Button ); 
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


