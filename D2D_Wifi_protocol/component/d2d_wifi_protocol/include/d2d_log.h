/**
 * d2d_log.h
 *
 *  Created on: 25 maj 2020
 *      Author: Rafal Kubis
 */

#ifndef COMPONENT_D2D_WIFI_PROTOCOL_INCLUDE_D2D_LOG_H_
#define COMPONENT_D2D_WIFI_PROTOCOL_INCLUDE_D2D_LOG_H_

#include "print.h"
#include "esp_err.h"

#define D2D_LOG_ENABLE 1
#define D2D_DEBUG_ENABLE 1





#if D2D_LOG_ENABLE
#define D2D_LOG(tag, log, ...)	\
		printf("\033[0;" COLOR_YELLOW "m" tag ": " log "\033[0m\n", ##__VA_ARGS__)
#else
#define D2D_LOG(tag, log, ...)
#endif

#if D2D_DEBUG_ENABLE
#define D2D_DEBUG(msg, ...)	\
		printf("\033[0;" COLOR_MAGENTA "m" "%s" " : " "line:" " %d" ": " msg "\033[0m\n",__func__, __LINE__,  ##__VA_ARGS__)
#else
#define D2D_DEBUG(msg, ...)
#endif


#endif /* COMPONENT_D2D_WIFI_PROTOCOL_INCLUDE_D2D_LOG_H_ */
