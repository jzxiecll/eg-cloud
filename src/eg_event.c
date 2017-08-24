#include "eg_event.h"


eg_queue_t *egEventQueue;
static eg_queue_pool_define(egEventQueueData, 10*sizeof(uint8_t*));

int  EG_event_init()
{
	int  ret = 0;	
	egEventQueue  =  (eg_queue_t *)EG_mem_malloc(sizeof(eg_queue_t));
	if(egEventQueue != NULL){		
		if (EG_queue_create(egEventQueue, "egEventQueue", sizeof(void *) , &egEventQueueData) != EG_SUCCESS) {	
		  ret = EG_FAIL;	
		  EG_E("EG_EventManagerInit failed.\r\n");
		}else{
		  EG_D("EG_EventManagerInit success.\r\n");
		}
	}else{
		EG_D("EG_EventManagerInit failed for malloc.\r\n");
		ret = EG_FAIL;
	}
	return ret;
}



int  EG_send_event_sem(eg_event_t EventItem)
{
	int ret = 0 ;
	uint8_t msg = EventItem ; 
	ret = EG_queue_send(egEventQueue, &msg, EG_msec_to_ticks(1));	
	EG_D("EG_send_event_sem is %d ret=%d\r\n",msg,ret);		
	return  ret;	
}


eg_event_t  EG_event()
{
	eg_event_t msg=EJ_EVENT_NO;
	if(EG_queue_recv(egEventQueue, &msg, EG_msec_to_ticks(0))==EG_SUCCESS)
	{
		return msg;	
	}

	return msg;
	
}


