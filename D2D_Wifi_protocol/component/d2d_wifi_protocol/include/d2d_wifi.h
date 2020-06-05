/**
 * d2d_wifi.h
 *
 *  Created on: 26 maj 2020
 *      Author: Rafal Kubis
 */

#ifndef COMPONENT_D2D_WIFI_PROTOCOL_D2D_WIFI_H_
#define COMPONENT_D2D_WIFI_PROTOCOL_D2D_WIFI_H_



// Wifi network ssid pattern
#define D2D_WIFI_SSID				"esp32_AP_"
#define D2D_WIFI_PASS				"qwerty12345"


// Root device STA config
#define D2D_WIFI_ROOT_SSID			"AP_for_root"
#define D2D_WIFI_ROOT_PASS			"asdfghjkl"


// Wifi ecent bits
#define D2D_WIFI_STA_CONN			1<<0
#define D2D_WIFI_STA_DISCONN		1<<1
#define D2D_WIFI_STA_START			1<<2


#define D2D_WIFI_EVENT_WAIT_TIME	1000 / portTICK_PERIOD_MS
#define D2D_WIFI_SCAN_LIST_SIZE		12
#define D2D_WIFI_MAX_CONN			6

#define D2D_WIFI_NETWORK_PATTERN	"100.200.0.0"
#define D2D_WIFI_NETWORK_NETMASK	"255.255.255.0"


d2d_err_t d2d_WifiConnectToAp(uint8_t* level);

d2d_err_t d2d_WifiInitSta(bool isRoot);

d2d_err_t d2d_WifiInitAp(void);

static d2d_err_t d2d_wifi_select_ap(wifi_ap_record_t* winner, uint8_t* winner_level, wifi_ap_record_t* ap_info, uint8_t* level, uint16_t len);

static d2d_err_t d2d_wifi_ap_dhcp_config(uint8_t level);

d2d_err_t d2d_WifiStartAP(uint8_t level);

d2d_err_t d2d_WifiInit(void);

void wifi_event_handler(void* arg, esp_event_base_t base_event, wifi_event_t event_id, void* event_data);

#endif /* COMPONENT_D2D_WIFI_PROTOCOL_D2D_WIFI_H_ */
