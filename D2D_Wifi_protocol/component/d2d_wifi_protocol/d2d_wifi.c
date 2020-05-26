/**
 * d2d_wifi.c
 *
 *  Created on: 26 maj 2020
 *      Author: Rafal Kubis
 */
#include "d2d.h"
#include "d2d_type.h"
#include "d2d_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "string.h"


static esp_netif_t* sta_netif;
static esp_netif_t* ap_netif;

#define D2D_TAG		"D2D WIFI"

d2d_err_t d2d_WifiConnectToAp(void){
	wifi_ap_record_t ap_info[D2D_WIFI_SCAN_LIST_SIZE];
	uint16_t ap_max = D2D_WIFI_SCAN_LIST_SIZE;
	uint16_t ap_cnt = 0;
	memset(ap_info, 0, sizeof(ap_info));
	ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_max, ap_info));
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_cnt));
	wifi_ap_record_t d2d_ap_info[D2D_WIFI_SCAN_LIST_SIZE];
	uint16_t d2d_ap_cnt = 0;
	memset(d2d_ap_info, 0, sizeof(d2d_ap_info));
	char* ssid_ptr;
	D2D_LOG(D2D_TAG, "Number of all available AP: %d", ap_cnt);
	for(uint16_t i = 0; (i < D2D_WIFI_SCAN_LIST_SIZE) && (i < ap_cnt); i++){
		D2D_LOG(D2D_TAG, "No.%d SSID: %s  RSSI %d" , i, ap_info[i].ssid, ap_info[i].rssi);
		ssid_ptr = strstr((char*)ap_info[i].ssid, D2D_WIFI_SSID);
		if(ssid_ptr == NULL){
			continue;
		}else{
			D2D_LOG(D2D_TAG, "All SSID with D2D pattern: %s", ap_info[i].ssid);
			d2d_ap_info[d2d_ap_cnt] = ap_info[i];
			d2d_ap_cnt++;
		}
	}

	return D2D_OK;
}

d2d_err_t d2d_WifiInitSta(bool isRoot){
	if(isRoot == true){

	}else{

	}
	return D2D_OK;
}

d2d_err_t d2d_WifiInitAp(void){
	return D2D_OK;
}

d2d_err_t d2d_WifiInit(void){
	ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK_WITHOUT_ABORT(ap_netif = esp_netif_create_default_wifi_ap());
    ESP_ERROR_CHECK_WITHOUT_ABORT(sta_netif = esp_netif_create_default_wifi_sta());
    D2D_DEBUG("*ap_netif = 0x%x", (uint32_t)(ap_netif));
    D2D_DEBUG("*sta_netif = 0x%x", (uint32_t)(sta_netif));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(
    		WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			(esp_event_handler_t)wifi_event_handler,
			NULL));
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
	return D2D_OK;
}

/**
 * @brief Wifi event handler
 *
 * @param arg
 * @param base_event
 * @param event_id
 * @param event_data
 */
void wifi_event_handler(
		void* arg, esp_event_base_t base_event,
		wifi_event_t event_id, void* event_data){
	switch(event_id){
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

