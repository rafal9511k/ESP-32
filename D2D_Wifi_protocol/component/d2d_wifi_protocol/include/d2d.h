/**
 * d2d.h
 *
 *  Created on: 25 kwi 2020
 *      Author: Rafal Kubis
 */

#ifndef COMPONENT_D2D_WIFI_PROTOCOL_D2D_H_
#define COMPONENT_D2D_WIFI_PROTOCOL_D2D_H_

#include "esp_err.h"
#include "d2d_type.h"
#include "d2d_socket.h"

esp_err_t d2d_serverCheckReceive(d2d_frame_t* frame, TickType_t blocktime);

// For testing
void v_d2d_testTask(void* args);
void v_d2d_testServerSocketTask(void *args);
void v_d2d_testClientSocketTask(void* args);
esp_err_t d2d_testInitalize(void);




#endif /* COMPONENT_D2D_WIFI_PROTOCOL_D2D_H_ */
