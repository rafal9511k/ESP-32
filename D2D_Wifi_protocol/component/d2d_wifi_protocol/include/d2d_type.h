/**
 * d2d_type.h
 *
 *  Created on: 25 kwi 2020
 *      Author: Rafal Kubis
 */

#ifndef COMPONENT_D2D_WIFI_PROTOCOL_D2D_TYPE_H_
#define COMPONENT_D2D_WIFI_PROTOCOL_D2D_TYPE_H_

#include "esp_netif.h"

#define D2D_FAIL			-1
#define D2D_OK				1
#define D2D_NO_RESPONSE		2
#define	D2D_INVALID_IP		3
#define D2D_NOT_READY		4

typedef int32_t d2d_err_t;

typedef struct{
	uint32_t id;
	uint32_t type;
}d2d_device_id_t;

typedef struct{
	uint32_t cmd;
	uint32_t cmd_param;
}d2d_device_cmd_t;

typedef struct{
	uint32_t data[12];
}d2d_device_data_t;

typedef struct{
	uint32_t bits;
}d2d_device_bits_t;

typedef struct{
	d2d_device_id_t id;
	//	d2d_device_id_t id_destination;
	//	d2d_device_id_t id_source;
	d2d_device_cmd_t cmd;
	d2d_device_data_t data;
	d2d_device_bits_t bits;
}d2d_frame_t;

typedef struct{
	esp_ip4_addr_t ip;
	d2d_device_id_t id;
}d2d_device_t;

typedef struct{
	d2d_device_t parent;
	d2d_device_t children[2];
}d2d_net_t;


enum d2d_cmd{
	SET_BIT,
	READ_BIT,
	WRITE_DATA,
	READ_DATA
};

extern const uint32_t d2d_frame_size;

#endif /* COMPONENT_D2D_WIFI_PROTOCOL_D2D_TYPE_H_ */
