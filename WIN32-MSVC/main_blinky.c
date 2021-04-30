
  /* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

//define queue values
#define mainQUEUE_LENGTH					( 2 )
#define mainVALUE_SENT_FROM_TASK			( 100UL )

//global queue variable
static QueueHandle_t xQueue = NULL;




//Task to take the speed reading
void vTaskSpeedReading(void* pvParameters)
{
	char* speedReadingName;
	speedReadingName = (char*)pvParameters;//Take in the name from the parameters 

	for (;;)
	{
		printf(speedReadingName);//Print that the task is running and checking for the speed
		vTaskDelay(pdMS_TO_TICKS(500));//Delay task 
	}

	vTaskDelete(NULL);//If the loop is exited, delete the task
}

//Task to take the distance reading
void vTaskDistanceReading(void* pvParameters)
{
	char* distanceReadingName;
	distanceReadingName = (char*)pvParameters;

	for (;;)
	{
		printf(distanceReadingName);//Show that the task is running
		vTaskDelay(pdMS_TO_TICKS(500));//Delay the task

	}

	vTaskDelete(NULL);//If for loop is exited, delte the task
}

//Task to calculate the speed needed from the above readings
void vTaskSpeedCalculation(void* pvParameters)
{
	const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK;//Value used to send to the queue

	TickType_t xLastWake = xTaskGetTickCount();//initialize the current tick count
	const TickType_t xDelay = pdMS_TO_TICKS(700);// variable for the delay amount in ticks

	char* speedCalculationName;
	speedCalculationName = (char*)pvParameters;//Name of the task variable

	for (;;)
	{
		printf(speedCalculationName);//Show that the speed is being calculated
		xQueueSend(xQueue, &ulValueToSend, 0U); //send to the queue which unblocks the queue receive 
		vTaskDelayUntil(&xLastWake, xDelay); //delay from the blocked state to the ready state
		
	}

	vTaskDelete(NULL);

}

void vTaskCorrectCarSpeed(void* pvParameters)
{
	uint32_t ulReceivedValue;

	char* speedCorrectionName;
	speedCorrectionName = (char*)pvParameters;
	

	for (;;) {

		xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);//receive from the queue
		if (ulReceivedValue == mainVALUE_SENT_FROM_TASK)// check to see that the value received matches the value given to the queue
		{
			printf(speedCorrectionName);// if so, show that the car correction task is running
		}
	}

}

//Variables to hold the text to be outputted by tasks

static const char* speedReadingText = "Speed Reading is running \r\n";
static const char* distanceReadingText = "Distance Reading is running \r\n";
static const char* speedCalculationText = "Speed Calculation is running \r\n";
static const char* carCorrectionText = "Car correction is running \r\n";

void main_blinky(void)
{

	xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));//Create the queue witht eh variables defined at the top of the file

	//Create the tasks with their priorities
	xTaskCreate(vTaskSpeedCalculation, "Calculation", 1000, (void*)speedCalculationText, 1, NULL);
	xTaskCreate(vTaskSpeedReading, "Speed", 1000, (void*)speedReadingText, 3, NULL);
	xTaskCreate(vTaskDistanceReading, "Distance", 1000, (void*)distanceReadingText, 2, NULL);
	xTaskCreate(vTaskCorrectCarSpeed, "Correct", 1000, (void*)carCorrectionText, 1, NULL);
	

	//Start the tasks and timer
	vTaskStartScheduler();
	
		
	
	for (;; );
}
/*-----------------------------------------------------------*/

