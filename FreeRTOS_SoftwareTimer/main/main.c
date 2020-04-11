#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "print.h"


void pxTimerCallback(TimerHandle_t timer);
void vTask1(void *pvParams);
/**
 *  Main function create and start timer,
 *  create Task1.
 */
void app_main(void)
{
    nvs_flash_init();
    TimerHandle_t xTimer = xTimerCreate(
    		/*	Name	*/
    		"Timer",
			/* Number of period ticks */
			10000 / portTICK_PERIOD_MS,
			/* Autoreload */
			pdFALSE,
			/* ID */
			0,
			/* Callback function */
			pxTimerCallback);
	printCF(COLOR_CYAN, "Timer instance = 0x%x", (uint32_t)xTimer);
    /* (timer , ticks to wait) */
    xTimerStart(xTimer, 0);
    printCF(COLOR_CYAN, "Timer was started");

    xTaskCreate(
    		vTask1,
			"Task 1",
			4096,
			(void *)xTimer,
			5,
			NULL
			);
}
/**
 * This is a timer period expired callback function,
 * prints timer ID and start timer.
 *
 * @param timer	Timer instance
 */
void pxTimerCallback(TimerHandle_t timer){
	printCF(COLOR_MAGENTA,
			"Period expired, ID = %d",
			(uint32_t)pvTimerGetTimerID(timer));
	xTimerStart(timer, 0);
	printCF(COLOR_MAGENTA, "Timer was started");
}

/**
 * This task function prints timer expiry time
 * and timer period.
 *
 * @param pvParams	Timer instance
 */
void vTask1(void *pvParams){
	TimerHandle_t timer = (void *)pvParams;
	printCF(COLOR_RED, "Timer instance = 0x%x", (uint32_t)timer);
	uint32_t expiry = 0;
	uint32_t period = 0;
	while(true){
		 expiry = xTimerGetExpiryTime(timer);
		 period = xTimerGetPeriod(timer);
		 printCF(COLOR_RED,
				 "Timer expired time = %d \n"
				 "Timer period = %d",
				 expiry,
				 period);

		 vTaskDelay(300 / portTICK_PERIOD_MS);
	}
}
