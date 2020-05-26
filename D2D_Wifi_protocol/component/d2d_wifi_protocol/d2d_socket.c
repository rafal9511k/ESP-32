/**
 * d2d_socket.c
 *
 *  Created on: 25 kwi 2020
 *      Author: Rafal Kubis
 */
#include "stdlib.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "lwip/sockets.h"

#include "d2d_socket.h"
#include "print.h"

#define D2D_PORT 4001
#define SOCKET_MAX_CONNECTIONS 4
static const char* D2D_SOCKET_SERVER_TAG = "D2D SOCKET SERVER";
static const char* D2D_SOCKET_CLIENT_TAG = "D2D SOCKET CLIENT";
const uint32_t msg_size = 12;

EventGroupHandle_t xSocketEvent;
uint8_t* serverRxBuff;
uint8_t* serverTxBuff;
uint8_t* clientRxBuff;
uint8_t* clientTxBuff;
in_addr_t serverAddr;
in_addr_t clientAddr;
/**
 * @brief Open listen socket on defined port
 *
 * @param args [in]
 */
void d2d_socketServer(void *args){
	serverRxBuff = malloc(d2d_frame_size);
	serverTxBuff = malloc(d2d_frame_size);
	EventBits_t tx_rx_bits;
	if(xSocketEvent != NULL){
		struct sockaddr_in source_addr;
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(D2D_PORT);
		int addr_family = AF_INET;
		int ip_protocol = IPPROTO_IP;

		int listen_socket = socket(addr_family, SOCK_STREAM, ip_protocol);
		if(listen_socket < 0){
			ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Unable to create socket: errno %d", errno);
		}else{
			int err = bind(listen_socket, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
			if(err != 0){
				ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Socket unable to bind: errno %d", errno);
			}else{
				err = listen(listen_socket, SOCKET_MAX_CONNECTIONS);
				if(err != 0){
					ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Error during listen: errno %d", errno);
				}else{
					while(true){
						unsigned int addr_len = sizeof(source_addr);
						printCF(COLOR_MAGENTA, "listen_socket = %d", listen_socket);
						int sock = accept(listen_socket, (struct sockaddr*)&source_addr, &addr_len);
						printCF(COLOR_MAGENTA, "sock = %d", sock);
						if(sock < 0){
							ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Unable to accept connection: errno %d", errno);
						}else{
							int len = recv(sock, serverRxBuff, d2d_frame_size, 0);
							if(len < 0){
								ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Receive error: errno %d", errno);
							}else if(len == 0){
								// no data receive
							}else{
								// check if full frame receive
							}
							ESP_LOGI(D2D_SOCKET_CLIENT_TAG, "Server RX Buffer =");
							ESP_LOG_BUFFER_HEX(D2D_SOCKET_SERVER_TAG, serverRxBuff, d2d_frame_size);

							xEventGroupSetBits(xSocketEvent, D2D_SERVER_RX);
							tx_rx_bits = xEventGroupWaitBits(
										xSocketEvent,
										D2D_SERVER_TX,
										pdTRUE,
										pdTRUE,
										100000 / portTICK_PERIOD_MS);
							if((tx_rx_bits & D2D_SERVER_TX) == D2D_SERVER_TX){
								int to_send = d2d_frame_size;
								while(to_send > 0){
									int sendof = send(sock, serverTxBuff+(d2d_frame_size-to_send), to_send, 0);
									ESP_LOGI(D2D_SOCKET_CLIENT_TAG, "Server TX Buffer =");
									ESP_LOG_BUFFER_HEX(D2D_SOCKET_SERVER_TAG, serverTxBuff, d2d_frame_size);
									if(sendof < 0){
										ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Error during sending");
									}
									to_send -= sendof;
								}
							shutdown(sock, 0);
							close(sock);
							}
						}
					}
				}

			}
		}
	}else{
		ESP_LOGE(D2D_SOCKET_SERVER_TAG, "Event group was not created");
	}
	free(serverRxBuff);
	free(serverTxBuff);
}

/**
 * @brief Create client socket
 * @param args
 */
esp_err_t d2d_socketClient(void* args){
	EventBits_t tx_rx_bits;
	clientRxBuff = malloc(d2d_frame_size);
	clientTxBuff = malloc(d2d_frame_size);
	if((clientRxBuff == NULL) | (clientTxBuff == NULL)){
		return ESP_FAIL;
	}
	struct sockaddr_in dest;
	dest.sin_port = htons(D2D_PORT);
	dest.sin_family = AF_INET;
	int addr_family = AF_INET;
	int ip_protocol = IPPROTO_IP;
	int sock = -1;
	while(1){
		close(sock);
		sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if(sock < 0){
			ESP_LOGE(D2D_SOCKET_CLIENT_TAG, "Unable to create socket");
			return ESP_FAIL;
		}
		tx_rx_bits = xEventGroupWaitBits(
				xSocketEvent,
				D2D_CLIENT_TX,
				pdFALSE,
				pdFALSE,
				100000 / portTICK_PERIOD_MS);
		if((tx_rx_bits & D2D_CLIENT_TX) == D2D_CLIENT_TX){
			//dest.sin_addr.s_addr = clientAddr;
			dest.sin_addr.s_addr = inet_addr("192.168.4.2");
			int err = connect(sock, (struct sockaddr*)&dest, sizeof(dest));
			printCF(COLOR_MAGENTA, "sock = %d", sock);
			if(err == 0){
				err = send(sock, clientTxBuff, d2d_frame_size, 0);
				ESP_LOGI(D2D_SOCKET_CLIENT_TAG, "Client TX Buffer =");
				ESP_LOG_BUFFER_HEX(D2D_SOCKET_CLIENT_TAG, clientTxBuff, d2d_frame_size);
				if(err < 0){
					ESP_LOGE(D2D_SOCKET_CLIENT_TAG, "Unable to send, errno : %d", errno);
					continue;
				}
				// delay
				int len = recv(sock, clientRxBuff, d2d_frame_size, 0);
				ESP_LOGI(D2D_SOCKET_CLIENT_TAG, "Client RX Buffer =");
				ESP_LOG_BUFFER_HEX(D2D_SOCKET_CLIENT_TAG, clientRxBuff, d2d_frame_size);
				if(len < 0){
					ESP_LOGE(D2D_SOCKET_CLIENT_TAG, "Receive error, errno : %d", errno);
					xEventGroupClearBits(xSocketEvent, D2D_CLIENT_TX);
					xEventGroupSetBits(xSocketEvent, D2D_CLIENT_RECV_ERR);
				}else{
					xEventGroupClearBits(xSocketEvent, D2D_CLIENT_TX);
					xEventGroupSetBits(xSocketEvent, D2D_CLIENT_RX);
				}
			}else{
				ESP_LOGE(D2D_SOCKET_CLIENT_TAG, "Unable to connect, errno : %d", errno);
				xEventGroupSetBits(xSocketEvent, D2D_CLIENT_CONN_ERR);
			}
		}

	}
}

esp_err_t d2d_socketInitalize(void){
	xSocketEvent = xEventGroupCreate();
	return ESP_OK;
}

