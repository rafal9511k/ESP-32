#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "string.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include "d2d.h"
#include "print.h"

#define AP_SSID		"esp32_AP_1"
#define AP_PASS		"qwerty12345"
#define STA_SSID	"FunBox2-3EED"
#define STA_PASS	"asdfghjkl"

esp_netif_t* sta_netif;
esp_netif_t* ap_netif;

void vWifiTask(void* pvParams);
void vD2dClientTestTask(void *pvParams);

/**
 * @brief Wifi event handler
 *
 * @param arg
 * @param base_event
 * @param event_id
 * @param event_data
 */
/*
void wifi_event_handler(
			void* arg, esp_event_base_t base_event,
			wifi_event_t event_id, void* event_data){

	switch(event_id){
	case WIFI_EVENT_STA_START:
		printCF(COLOR_BLUE, "Station started");
		break;
	case WIFI_EVENT_AP_START:
		printCF(COLOR_BLUE, "Access point started");
		break;
	case WIFI_EVENT_AP_STACONNECTED:
	{
		wifi_event_ap_staconnected_t* data
		= (wifi_event_ap_staconnected_t*) event_data;
		printCF(
			COLOR_BLUE,
			"STA connected to AP, MAC : "MACSTR"",
			MAC2STR(data->mac)
			);
	}
		break;
	case WIFI_EVENT_AP_STADISCONNECTED:
	{
		wifi_event_ap_stadisconnected_t* data
		= (wifi_event_ap_stadisconnected_t*) event_data;
		printCF(
			COLOR_BLUE,
			"STA disconnected from AP, MAC : "MACSTR"",
			MAC2STR(data->mac)
			);
	}
		break;
	default:
		printCF(COLOR_BLUE, "Wifi event id = %d", event_id);

	}
}
*/
void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	/*
	ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK_WITHOUT_ABORT(ap_netif = esp_netif_create_default_wifi_ap());
    ESP_ERROR_CHECK_WITHOUT_ABORT(sta_netif = esp_netif_create_default_wifi_sta());
    printCF(COLOR_RED, "debug ap_netif = 0x%x", (uint32_t)(ap_netif));
    printCF(COLOR_RED, "debug sta_netif = 0x%x", (uint32_t)(sta_netif));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(
    		WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			(esp_event_handler_t)wifi_event_handler,
			NULL));
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA));
    printCF(COLOR_RED, "debug");
    wifi_config_t sta_config = {
        .sta = {
            .ssid = STA_SSID,
            .password = STA_PASS,
            .bssid_set = false
        }
    };

    wifi_config_t ap_config = {
    	.ap = {
			.ssid = AP_SSID,
			.ssid_len = strlen(AP_SSID),
			.password = AP_PASS,
			.channel = 1,
			.max_connection = 3,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK
    	}
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
*/
    printCF(COLOR_RED, "debug");

    ESP_ERROR_CHECK(d2d_testInitalize());

    xTaskCreate(
    		vWifiTask,
			"Wifi Task",
			4096,
			NULL,
			5,
			NULL);

    xTaskCreate(
    		vD2dClientTestTask,
			"D2D Client Task",
			4096,
			NULL,
			5,
			NULL);



    while (true) {
        TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed = 1;
        TIMERG0.wdt_wprotect - 0;
    }
}

/**
 * @brief Print MAC and IP address
 *
 * @param pvParams
 *
 *
 * Print MAC and IP address of connected devices to STA and print
 * IP address, SSID of AP which is connected
 *
 */
void vWifiTask(void* pvParams){
	wifi_sta_list_t wifi_sta;
	esp_netif_ip_info_t sta_ip_info;
	esp_netif_sta_list_t netif_sta;
	esp_netif_sta_info_t netif_info;
	while(true){
		/* STA info */
		if(esp_wifi_ap_get_sta_list(&wifi_sta) != ESP_OK){
			printCF(COLOR_CYAN, "Get STA list error");
		}else{
			/* get all STA ip and mac addr */
			esp_netif_get_sta_list(&wifi_sta, &netif_sta);
			printCF(COLOR_CYAN, "Number of connected STA = %d", netif_sta.num);
			for(uint8_t i = 0; i < netif_sta.num; i++){
				netif_info = netif_sta.sta[i];
				printCF(
					COLOR_CYAN,
					"Device %d. MAC : "MACSTR" , IP : %s ",
					i,
					MAC2STR(netif_info.mac),
					ip4addr_ntoa(&netif_info.ip));
			}
		}
		if(esp_netif_get_ip_info(sta_netif, &sta_ip_info) != ESP_OK){
			printCF(COLOR_CYAN, "Get STA ip info error");
		}else{

			printCF(COLOR_CYAN, "STA ip address : %s", ip4addr_ntoa(&sta_ip_info.ip));
		}
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
}

/**
 * @brief D2D protocol client test task
 * @param pvParams
 */
void vD2dClientTestTask(void *pvParams){
	esp_ip4_addr_t ip;
	d2d_frame_t frame;
	ip.addr = ipaddr_addr("192.168.4.2");
	//ip4addr_aton("192.168.1.36", &ip);
	vTaskDelay(10000 / portTICK_PERIOD_MS);
	while(1){
		frame.id.id++;
		printCF(COLOR_GREEN , "Client send");
		int err = d2d_clientSend(ip, &frame);
		printCF(COLOR_GREEN , "return value = %d", err);
		d2d_clientReceive(&frame, 1000 / portTICK_PERIOD_MS);
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
}

