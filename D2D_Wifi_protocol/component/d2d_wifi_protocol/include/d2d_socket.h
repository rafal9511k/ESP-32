/**
 * d2d_socket.h
 *
 *  Created on: 25 kwi 2020
 *      Author: Rafal Kubis
 */

#ifndef COMPONENT_D2D_WIFI_PROTOCOL_D2D_SOCKET_H_
#define COMPONENT_D2D_WIFI_PROTOCOL_D2D_SOCKET_H_

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "d2d_type.h"



#define D2D_SERVER_TX			1<<0
#define D2D_SERVER_RX			1<<1
#define D2D_CLIENT_TX			1<<2
#define D2D_CLIENT_RX			1<<3
#define D2D_CLIENT_CONN_ERR		1<<4
#define D2D_CLIENT_RECV_ERR		1<<5


extern EventGroupHandle_t xSocketEvent;
extern uint8_t* serverRxBuff;
extern uint8_t* serverTxBuff;
extern uint8_t* clientRxBuff;
extern uint8_t* clientTxBuff;

extern in_addr_t serverAddr;
extern in_addr_t clientAddr;

esp_err_t d2d_socketClient(void* args);
void d2d_socketServer(void* args);
esp_err_t d2d_socketInitalize(void);
#endif /* COMPONENT_D2D_WIFI_PROTOCOL_D2D_SOCKET_H_ */
