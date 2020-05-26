/**
 * d2d_wifi.h
 *
 *  Created on: 26 maj 2020
 *      Author: Rafal Kubis
 */

#ifndef COMPONENT_D2D_WIFI_PROTOCOL_D2D_WIFI_H_
#define COMPONENT_D2D_WIFI_PROTOCOL_D2D_WIFI_H_



// Wifi network ssid pattern
#define D2D_WIFI_SSID				"esp32_AP"
#define D2D_WIFI_PASS				"qwerty123456"


// Root device STA config
#define D2D_WIFI_ROOT_SSID			"AP_for_root"
#define D2D_WIFI_ROOT_PASS			"asdfghjkl"


#define D2D_WIFI_SCAN_LIST_SIZE		12



d2d_err_t d2d_WifiConnectToAp(void);

d2d_err_t d2d_WifiInitSta(bool isRoot);

d2d_err_t d2d_WifiInitAp(void);

d2d_err_t d2d_WifiInit(void);

void wifi_event_handler(void* arg, esp_event_base_t base_event, wifi_event_t event_id, void* event_data);

#endif /* COMPONENT_D2D_WIFI_PROTOCOL_D2D_WIFI_H_ */
