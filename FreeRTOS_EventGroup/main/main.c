#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include "print.h"

#define TASK1_BIT 1<<0
#define TASK2_BIT 1<<1

void vTask1(void *pvParams);
void vTask2(void *pvParams);
void vTask3(void *pvParams);

EventGroupHandle_t xEventGroup;


void app_main(void)
{
    nvs_flash_init();

    xEventGroup = xEventGroupCreate();
    if(xEventGroup != NULL){
    	printCF(COLOR_RED, "Event Group was not created");
    }else{
    	printCF(COLOR_RED, "Event Group was created");
    }

    xTaskCreate(
    		vTask1,
			"Task 1",
			4096,
			NULL,
			6,
			NULL);

    xTaskCreate(
    		vTask2,
			"Task 2",
			4096,
			NULL,
			6,
			NULL);

    xTaskCreate(
    		vTask3,
			"Task 3",
			4096,
			NULL,
			5,
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
 * Set TASK1_bit then wait for defined time
 */
void vTask1(void *pvParams){
	EventBits_t eventBits;
	while(true){
		eventBits = xEventGroupSetBits(xEventGroup, TASK1_BIT);
		printCF(COLOR_GREEN, "TASK1_BIT was set, eventBits = 0x%x", (uint32_t)eventBits);
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

/*
 * @breif Task 2 function
 *
 *	Set TASK2_bit then wait for defined time
 */
void vTask2(void *pvParams){
	EventBits_t eventBits;
	while(true){
		if(xEventGroup != NULL){
			eventBits = xEventGroupSetBits(xEventGroup, TASK2_BIT);
			printCF(COLOR_MAGENTA, "TASK2_BIT was set, eventBits = 0x%x", (uint32_t)eventBits);
		}

		vTaskDelay(15000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
/*
 * @brief Task 3 function
 *
 * Wait for TASK1_bit and TASK2_bit for 500 ms,
 * if bits are 1 then clear them, and wait for
 * defined time.
 */
void vTask3(void *pvParams){
	EventBits_t eventBits;
	while(true){
		printCF(COLOR_CYAN, "Waiting for 5 seconds for event bits");
		eventBits = xEventGroupWaitBits(
				xEventGroup,
				TASK1_BIT | TASK2_BIT,
				pdTRUE,
				pdTRUE,
				500 / portTICK_PERIOD_MS);
		printCF(COLOR_CYAN, "eventBits = 0x%x", (uint32_t)eventBits);
		if((eventBits & (TASK1_BIT | TASK2_BIT)) == (TASK1_BIT | TASK2_BIT)){
			printCF(COLOR_CYAN, "TASK1_BIT and TASK2_BIT was set");
		}else{
			printCF(COLOR_CYAN, "TASK1_BIT and TASK2_BIT was not set");
		}
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

