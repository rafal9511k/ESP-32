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

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

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

    /* (timer , ticks to wait) */
    xTimerStart(xTimer, 0);
    printCF(COLOR_CYAN, "Timer was started");
}

void pxTimerCallback(TimerHandle_t timer){
	printCF(COLOR_MAGENTA, "Period expired, ID = %d", (uint32_t)pvTimerGetTimerID(timer));
	xTimerStart(timer, 0);
	printCF(COLOR_MAGENTA, "Timer was started");
}

