/**
 * d2d.c
 *
 *  Created on: 25 kwi 2020
 *      Author: Rafal Kubis
 */

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "d2d_socket.h"
#include "d2d_type.h"
#include "d2d.h"
#include "print.h"




/**
 * @brief Server check if new data has been received
 *
 * @param [out] frame  		Received frame
 * @param [in] 	blocktime 	Waiting time
 * @return ESP_OK
 */
esp_err_t d2d_serverCheckReceive(d2d_frame_t* frame, TickType_t blocktime){
	EventBits_t rx_tx_bits;
	printCF(COLOR_RED, "debug");
	rx_tx_bits = xEventGroupWaitBits(
			xSocketEvent,
			D2D_SERVER_RX,
			pdTRUE,
			pdFALSE,
			blocktime);
	if((rx_tx_bits & D2D_SERVER_RX) == D2D_SERVER_RX){
		printCF(COLOR_MAGENTA, "RX_EVENT_BITS_SET");
		memcpy((void*)frame, serverRxBuff, d2d_frame_size);
		printCF(COLOR_MAGENTA, "Frame id : %d", frame->id.id);
		printCF(COLOR_MAGENTA, "Frame bits : 0x%x", frame->bits.bits);
	}else{
		return ESP_ERR_NOT_FOUND;
	}
	return ESP_OK;
}

/**
 * @brief Server transmit frame
 *
 * @param frame		Frame to send
 * @return
 */
esp_err_t d2d_serverSend(d2d_frame_t* frame){
	if(frame == NULL){
		return ESP_FAIL;
	}
	memcpy(serverTxBuff, (void*)frame, d2d_frame_size);
	xEventGroupSetBits(xSocketEvent, D2D_SERVER_TX);
	printCF(COLOR_MAGENTA, "TX_EVENT_BITS_SET");
	return ESP_OK;
}

/**
 * @brief Client send frame
 *
 * @param ip		IP address of destination device
 * @param frame		Frame to send
 * @return ESP_OK
 */
esp_err_t d2d_clientSend(esp_ip4_addr_t ip, d2d_frame_t* frame){
	if(frame == NULL){
		return ESP_FAIL;
	}
	if(xSocketEvent == NULL){
		return ESP_FAIL;
	}
	memcpy(clientTxBuff,(void*)frame, d2d_frame_size);
	xEventGroupSetBits(xSocketEvent, D2D_CLIENT_TX);
	return ESP_OK;
}

/**
 * @brief Client receive frame
 *
 * @param frame		Received frame
 * @param timeout	Wainting time
 * @return
 */
esp_err_t d2d_clientReceive(d2d_frame_t* frame, TickType_t timeout){
	EventBits_t rx_tx_bits;
	rx_tx_bits = xEventGroupWaitBits(
			xSocketEvent,
			D2D_CLIENT_RX | D2D_CLIENT_CONN_ERR | D2D_CLIENT_RECV_ERR,
			pdTRUE,
			pdFALSE,
			timeout);
	if(rx_tx_bits  == D2D_CLIENT_CONN_ERR){
		return 0;
	}else if((rx_tx_bits& D2D_CLIENT_RECV_ERR) == D2D_CLIENT_RECV_ERR){
		return 0;
	}else if((rx_tx_bits & D2D_CLIENT_RX) == D2D_CLIENT_RX){
		memcpy((void*)frame, clientRxBuff, d2d_frame_size);
		return ESP_OK;
	}else{
		return 0;
	}
}
/*
 * for testing
 */
void v_d2d_testTask(void* args){
	d2d_frame_t frame;
	while(1){
		printCF(COLOR_RED, "debug");
		if(d2d_serverCheckReceive(&frame,  100) == ESP_OK){
			d2d_serverSend(&frame);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

void v_d2d_testServerSocketTask(void *args){
	while(1){
		printCF(COLOR_RED, "debug");
		d2d_socketServer(NULL);
	}
	vTaskDelete(NULL);
}

void v_d2d_testClientSocketTask(void* args){
	while(1){
		d2d_socketClient(NULL);
	}
	vTaskDelete(NULL);
}

esp_err_t d2d_testInitalize(void){
	d2d_socketInitalize();
	printCF(COLOR_RED, "debug");
	xTaskCreate(
			v_d2d_testServerSocketTask,
			"D2D Socket Server Task",
			4096,
			NULL,
			configMAX_PRIORITIES-3,
			NULL);
	/*
	xTaskCreate(
			v_d2d_testClientSocketTask,
			"D2D Socket Client Task",
			4096,
			NULL,
			configMAX_PRIORITIES-3,
			NULL);
	*/
	xTaskCreate(
			v_d2d_testTask,
			"D2D Test Task",
			4096,
			NULL,
			5,
			NULL);
	return ESP_OK;
}

