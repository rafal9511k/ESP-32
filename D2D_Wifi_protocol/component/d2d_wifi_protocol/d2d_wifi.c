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


extern esp_netif_t* sta_netif;
extern esp_netif_t* ap_netif;
EventGroupHandle_t xWifiEvent;
#define D2D_TAG		"D2D WIFI"
/**
 * @brief Connect to appropriate access point
 *
 * Function scan for access points with ssid compatible
 * with D2D_WIFI_NETWORK_PATTERN, if is available connect
 * to the access point selected by function d2d_wifi_select_ap.
 *
 * @param[out] level Level of access point
 *
 * @return
 * 		- D2D_OK
 * 		- D2D_FAIL
 */
d2d_err_t d2d_WifiConnectToAp(uint8_t* level){
	d2d_err_t err;
	wifi_ap_record_t ap_info[D2D_WIFI_SCAN_LIST_SIZE];
	uint16_t ap_max = D2D_WIFI_SCAN_LIST_SIZE;
	uint16_t ap_cnt = 0;
	memset(ap_info, 0, sizeof(ap_info));
	ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_max, ap_info));
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_cnt));
	wifi_ap_record_t d2d_ap_info[D2D_WIFI_SCAN_LIST_SIZE];
	uint8_t d2d_ap_level[D2D_WIFI_SCAN_LIST_SIZE];
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
			//D2D_LOG(D2D_TAG, "All SSID with D2D pattern: %s", ap_info[i].ssid);
			d2d_ap_info[d2d_ap_cnt] = ap_info[i];
			d2d_ap_level[d2d_ap_cnt] = atoi(ssid_ptr + strlen(D2D_WIFI_SSID));
			D2D_LOG(D2D_TAG, "SSID with D2D pattern: %s , level : %d", ap_info[i].ssid, d2d_ap_level[d2d_ap_cnt]);
			d2d_ap_cnt++;
		}
	}
	wifi_ap_record_t d2d_ap_winner;
	err = d2d_wifi_select_ap(&d2d_ap_winner, level, d2d_ap_info, d2d_ap_level, d2d_ap_cnt);
	if(err == D2D_OK){
		wifi_config_t sta_config = {
				.sta ={}
		};

		D2D_DEBUG("d2d_ap_winner.ssid : %s", (char*)d2d_ap_winner.ssid);
		memcpy(sta_config.sta.ssid, d2d_ap_winner.ssid, sizeof(sta_config.sta.ssid));
		memcpy(sta_config.sta.password, D2D_WIFI_PASS, sizeof(D2D_WIFI_PASS));
		sta_config.sta.bssid_set = false;
		sta_config.sta.pmf_cfg.required = false;

		D2D_DEBUG("sta_config.sta.ssid : %s", sta_config.sta.ssid);
		D2D_DEBUG("sta_config.sta.password : %s", sta_config.sta.password);
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
		ESP_ERROR_CHECK(esp_wifi_connect());
	}else{
		return D2D_FAIL;
	}
	return D2D_OK;
}
/**
 * @brief Select the most appropriate access point
 *
 * @param[out] winner The winner access point record
 * @param[out] winner_level Level of access point
 * @param[in] ap_info Pointer to table with all access points record
 * @param[in] level	Pointer to table with access point levels
 * @param[in[ len Length of ap_info and level
 *
 * @return
 * 		- D2D_OK
 * 		- D2D_FAIL
 */
static d2d_err_t d2d_wifi_select_ap(wifi_ap_record_t* winner, uint8_t* winner_level, wifi_ap_record_t* ap_info, uint8_t* level, uint16_t len){
	if((ap_info == NULL)
			|| (level == NULL)
			|| (winner == NULL)
			|| (winner_level == NULL)){
		return D2D_FAIL;
	}
	if(len <= 0){
		D2D_DEBUG("len = %d", len);
		return D2D_WIFI_NO_AP;
	}

	memcpy(winner, ap_info, sizeof(wifi_ap_record_t));
	*winner_level = level[0];
	return D2D_OK;
}

d2d_err_t d2d_WifiStartAP(uint8_t level){
	d2d_wifi_ap_dhcp_config(level);
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	wifi_config_t ap_config = {
			.ap = {}
	};
	char ssid_str[sizeof(ap_config.ap.ssid)];
	char level_str[10];
	utoa(level, level_str, 10);
	D2D_DEBUG("level = %d", level);
	D2D_DEBUG("level_str = %s", level_str);
	strcpy(ssid_str, D2D_WIFI_SSID);
	strcat(ssid_str, level_str);
	D2D_DEBUG("ssid_str = %s", ssid_str);
	memcpy(ap_config.ap.ssid, ssid_str, sizeof(ap_config.ap.ssid));
	if(sizeof(D2D_WIFI_PASS) > sizeof(ap_config.ap.password)){
		return D2D_FAIL;
	}
	memcpy(ap_config.ap.password, D2D_WIFI_PASS, sizeof(D2D_WIFI_PASS));
	ap_config.ap.max_connection = D2D_WIFI_MAX_CONN;
	ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));


	return D2D_OK;
}
/**
 * @brief Configure access point dhcp
 * @param[in] level Level of access point
 * @return
 * 		- D2D_OK
 * 		- D2D_FAIL
 */
static d2d_err_t d2d_wifi_ap_dhcp_config(uint8_t level){
	esp_err_t err;
	esp_netif_dhcp_status_t status;
	err = esp_netif_dhcps_get_status(ap_netif, &status);
	D2D_DEBUG("err = 0x%x", err);
	D2D_DEBUG("status = 0x%x", status);
	if(err != ESP_OK){
		return D2D_FAIL;
	}
	if(status == ESP_NETIF_DHCP_INIT){
		err = esp_netif_dhcps_stop(ap_netif);
		D2D_DEBUG("err = 0x%x", err);
		if(err == ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED){
			err = esp_netif_dhcps_get_status(ap_netif, &status);
			D2D_DEBUG("err = 0x%x", err);
			D2D_DEBUG("status = 0x%x", status);
			return D2D_FAIL;
		}

	}
	D2D_DEBUG("err = 0x%x", err);
	if(status == ESP_NETIF_DHCP_STARTED){
		err = esp_netif_dhcps_stop(ap_netif);
		D2D_DEBUG("err = 0x%x", err);
		if(err != ESP_OK){
			return D2D_FAIL;
		}
		err = esp_netif_dhcps_get_status(ap_netif, &status);
		D2D_DEBUG("err = 0x%x", err);
		D2D_DEBUG("status = 0x%x", status);
		if(err != ESP_OK){
			return D2D_FAIL;
		}
	}
	D2D_DEBUG("err = 0x%x", err);
	if(status == ESP_NETIF_DHCP_STOPPED || status == ESP_NETIF_DHCP_INIT){
		esp_netif_ip_info_t ip_info;
		ip_info.netmask.addr = esp_ip4addr_aton(D2D_WIFI_NETWORK_NETMASK);
		uint32_t ip_gw = esp_ip4addr_aton(D2D_WIFI_NETWORK_PATTERN);
		ip_gw |= level << 16;
		uint32_t ip_ip = ip_gw | 1<<24;
		ip_info.ip.addr = ip_ip;
		ip_info.gw.addr = ip_gw;
		err = esp_netif_set_ip_info(ap_netif, &ip_info);
		D2D_DEBUG("err = 0x%x", err);
		if(err != ESP_OK){
			return D2D_FAIL;
		}
		err = esp_netif_dhcps_start(ap_netif);
		D2D_DEBUG("err = 0x%x", err);
		if(err != ESP_OK){
			return D2D_FAIL;
		}else{
			return D2D_OK;
		}
	}else{
		return D2D_FAIL;
	}
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
/**
 *
 * @return
 */
d2d_err_t d2d_WifiInit(void){
	xWifiEvent = xEventGroupCreate();
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
	D2D_DEBUG("esp_wifi_start -->");
    ESP_ERROR_CHECK(esp_wifi_start());
	return D2D_OK;
}

/**
 * Wifi task
 * @param args
 *
 * Task try to recover connection
 */
void task_d2d_wifi(void* args){
	d2d_err_t err;
	EventBits_t bits;
	uint8_t level;
	while(true){
		bits = xEventGroupWaitBits(
				xWifiEvent,
				0xffff,		// event on all bits
				pdTRUE,	// clear on exit
				pdFALSE,
				D2D_WIFI_EVENT_WAIT_TIME);
		if((bits & D2D_WIFI_STA_START) == D2D_WIFI_STA_START){
			err = d2d_WifiConnectToAp(&level);
			while(err == D2D_WIFI_NO_AP){
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				err = d2d_WifiConnectToAp(&level);
			}
		}else if((bits & D2D_WIFI_STA_DISCONN) == D2D_WIFI_STA_DISCONN){
			D2D_DEBUG("Wifi event: D2D_WIFI_STA_DISCONN");
			err = d2d_WifiConnectToAp(&level);
			while(err == D2D_WIFI_NO_AP){
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				err = d2d_WifiConnectToAp(&level);
			}
		}else if((bits & D2D_WIFI_STA_CONN) == D2D_WIFI_STA_CONN){
			D2D_DEBUG("Wifi event: D2D_WIFI_STA_CONN");
			err = d2d_WifiStartAP(++level);
		}
	}
	vTaskDelete(NULL);
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
	case WIFI_EVENT_STA_START:
		D2D_LOG(D2D_TAG, "WIFI STA START");
		xEventGroupSetBits(xWifiEvent, D2D_WIFI_STA_START);
		break;
	case WIFI_EVENT_STA_CONNECTED:
		xEventGroupSetBits(xWifiEvent, D2D_WIFI_STA_CONN);
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
		xEventGroupSetBits(xWifiEvent, D2D_WIFI_STA_DISCONN);
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

