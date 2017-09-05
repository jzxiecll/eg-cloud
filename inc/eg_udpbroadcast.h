#ifndef __EG_UDPBROADCAST_H__
#define __EG_UDPBROADCAST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "eg_porting_layer.h"

typedef enum {
	INIT_UDPBROADCAST_SUCCESS = 0x00,
	INIT_UDPBROADCAST_SOCKET_CREATE_ERROR,
	INIT_UDPBROADCAST_SOCKET_SET_BROADCAST_ERROR,
	INIT_UDPBROADCAST_SOCKET_BIND_ERROR,
	INIT_UDPBROADCAST_THREAD_CREATE_ERROR
}EG_UDPBROADCAST_START_R;

typedef enum {
	EG_UDPBROADCAST_PACKET_TUNKOWN = 0x00,
	EG_UDPBROADCAST_PACKET_TCALL ,
	EG_UDPBROADCAST_PACKET_TGATEWAYINFO,
	EG_UDPBROADCAST_PACKET_TGETDID
}EG_UDPBROADCAST_PACKET_T;


typedef struct {

	uint8_t uuid[8];
	uint8_t did[14];
	uint8_t mac[14];
	uint8_t ipaddr[20];
	
}PakcetWifiModuleBroadcastResponse;


int EG_udpbroadcast_start();
int EG_udpbroadcast_stop();


#ifdef __cplusplus
}
#endif

#endif // __EG_SMARTLINK_H__

