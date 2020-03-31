#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "print.h"


void vTask1(void *pvParams);
void vTask2(void *pvParams);
void vTask3(void *pvParams);

QueueHandle_t xQueue;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();

    xQueue = xQueueCreate(10, sizeof(int));
    if(xQueue == NULL){
    	ESP_LOGE("RTOS", "Queue was not created");
    }
    ESP_LOGI("RTOS", "Queue was created");

    xTaskCreate(
    		vTask1,
			"Task 1",
			4096,
			NULL,
			2,
			NULL);

    xTaskCreate(
    		vTask2,
			"Task 2",
			4096,
			NULL,
			2,
			NULL);

    xTaskCreate(
    		vTask3,
			"Task 3",
			4096,
			NULL,
			3,
			NULL);
    while(true){
        TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed = 1;
        TIMERG0.wdt_wprotect - 0;
    }
}
/*
 * @brief Task 1 function
 *
 *  Send the counter value to queue
 */
void vTask1(void *pvParams){
	int cnt = 0;
	BaseType_t xResult;
	while(true){
		printCF(COLOR_CYAN, "cnt = %d", cnt);
		if(xQueue != NULL){
			if(xQueueSend(xQueue, &cnt, 0) != pdPASS){
				printCF(COLOR_CYAN, "Queue is full");
				cnt -= 2;
			}else{
				printCF(COLOR_CYAN, "Element added to Queue");
				cnt += 2;
			}
		}else{
			printCF(COLOR_CYAN, "Queue is NULL");
		}
		//printCF(COLOR_CYAN, "xQueueSend = %d", xResult);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

/*
 * @breif Task 2 function
 *
 * Send the counter value to the front of queue
 */
void vTask2(void *pvParams){
	int cnt = 9999;
	while(true){
		printCF(COLOR_MAGENTA, "cnt = %d", cnt);
		if(xQueue != NULL){
			if(xQueueSendToFront(xQueue, &cnt, 0) != pdPASS){
				printCF(COLOR_MAGENTA, "Queue is full");
				cnt++;
			}else{
				printCF(COLOR_MAGENTA, "Element added at the front of Queue");
				cnt--;
			}
		}else{
			printCF(COLOR_MAGENTA, "Queue is NULL");
		}
		vTaskDelay(3000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
/*
 * @brief Task 3 function
 *
 * Receieve all value from queue
 */
void vTask3(void *pvParams){
	int buffer;
	uint8_t queueCnt = 0;
	while(true){
		if(xQueue != NULL){
			while(xQueueReceive(xQueue, &buffer, 100 / portTICK_RATE_MS) == pdPASS){
				printCF(COLOR_RED, "No. %d  value = %d", queueCnt, buffer);
				queueCnt++;
			}
			printCF(COLOR_RED, "No more elements in Queue");
			queueCnt = 0;
		}else{
			printCF(COLOR_RED, "Queue is NULL");
		}
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
