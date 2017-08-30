#ifndef __EG_EVENT_H__
#define __EG_EVENT_H__



#ifdef __cplusplus
extern "C" {
#endif

#include "eg_porting_layer.h"

typedef enum
{

	EJ_EVENT_shouldUARTThreadWorkSem =1 ,
	EJ_EVENT_shouldMQTTThreadWorkSem,	
	EJ_EVENT_ezconnectCompletedSem,
	EJ_EVENT_routerConnectedSem,
	EJ_EVENT_broadcastRequestSem,	
	EJ_EVENT_broadcastResponseSem,	
	EJ_EVENT_uapStartedSem,	
	EJ_EVENT_easylinkRequestSem,	
	EJ_EVENT_softAPRequestSem,	
	EJ_EVENT_airkissRequestSem,
	EJ_EVENT_joylinkRequestSem,
	EJ_EVENT_MQTTConnectionLostSem,	
	EJ_EVENT_rebootRequestSem,	
	EJ_EVENT_Reset2FactoryRequestSem,	
	EJ_EVENT_routerAuthFailedSem,	
	EJ_EVENT_reApplyIDSem,
	EJ_EVENT_timeSyncSem,
	EJ_EVENT_routerConnectingSem,
	EJ_EVENT_homeApNotConfiguredSem,
	EJ_EVENT_udpBroadcastSem,
	EJ_EVENT_egTestSem,
	EJ_EVENT_NO
}eg_event_t;

int  EG_event_init();

unsigned char  EG_event();

int  EG_send_event_sem(eg_event_t EventItem);


#ifdef __cplusplus
}
#endif

#endif // __EG_SMARTLINK_H__

