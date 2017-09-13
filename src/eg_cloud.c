#include "eg_cloud.h"
#include "eg_mqtt.h"
#include "eg_porting_layer.h"
#include "eg_event.h"
#include "eg_udpbroadcast.h"
eg_timer_t EventTimer=NULL;
static eg_timer_t event_udpbroadcast_timer = NULL;
static eg_timer_t eg_timer_reply_deviceid=NULL;

static eg_thread_t eg_test_thread =0 ;
static eg_thread_stack_define(eg_test_thread_stack, 1024);

int EG_test_start();

uint8_t*eg_msg_process(unsigned char *r_pain_text,int len)
{
	EG_HEX(r_pain_text,len);
}

static void UdpBroadcastTimerCB()
{
	EG_timer_deactivate(&event_udpbroadcast_timer);
	EG_udpbroadcast_stop();
	eg_set_dev_info_wifistatus_udpbroadcastStatus(WIFIMODULE_UDPBROADCAST_RUNNED);		
}

static void eg_start_udpbroadcast_timer()
{

	if(!event_udpbroadcast_timer){		
		if (EG_timer_create(&event_udpbroadcast_timer,
			    "EventTimer",
			    EG_msec_to_ticks(60000),
			    &UdpBroadcastTimerCB,
			    NULL,
			    EG_TIMER_PERIODIC,
			    EG_TIMER_AUTO_ACTIVATE) != EG_SUCCESS) 
		{
				EG_LOG_ERROR("Failed to create EventTimer timer.\r\n");
		}
	}else{
		EG_timer_activate(&event_udpbroadcast_timer);
	}

}
static void EJ_event_reapply_did_process(void* data)
{
	EG_request_deviceid(eg_get_dev_info_macaddr(), eg_get_dev_info_uuid());
}

static void EJ_event_egtest_process(void* data)
{
	EG_DEBUG("EG_event_egtest and Start EG test ................................");
	EG_test_start();	
}
static void EG_event_udpbroadcast_process(void* data)
{
	EG_DEBUG("EG_event_udpbroadcast_process and Start ................................");
	if(EG_udpbroadcast_start()==0)
	{
		eg_set_dev_info_wifistatus_udpbroadcastStatus(WIFIMODULE_UDPBROADCAST_RUNNING);
		eg_start_udpbroadcast_timer();
	}else{
		eg_set_dev_info_wifistatus_udpbroadcastStatus(WIFIMODULE_UDPBROADCAST_NOT_RUNNING);
	}
	
}

static void EJ_event_router_connected_process(void* data)
{
	eg_set_dev_info_wifistatus_routerStatus( ROUTER_CONNECTED);
	
	int r_mqtt = 0;
	/* if cloud services is not connected. */
	if (eg_get_dev_info_wifistatus_cloudServiceStatus()== CLOUD_MQTT_CONNECTTING) 
	{	

		r_mqtt = EG_mqtt_start() ;
		if (r_mqtt== INIT_MQTT_SUCCESS) 
		{		
			eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_MQTT_CONNECTED);			
			EG_LOG_INFO("Connect egcloud server success.\r\n");
			EG_send_event_sem(EJ_EVENT_reApplyIDSem);
			EG_start_get_deviceid_timer();
		}
		else
		{
			//eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_MQTT_NOT_CONNECTED);
			EG_send_event_sem(EJ_EVENT_routerConnectedSem);
			//EG_DEBUG("SEND NO2......");
			//EG_mqtt_stop();
			if(r_mqtt == INIT_MQTT_OS_THREAD_CREATE_ERROR)
				//EG_mqtt_thread_delete();
				EG_mqtt_stop();
		}	
	}
}



static void EJ_event_shouldMQTTThreadWorkProcess(void* data)
{


		if (EG_mqtt_start() == INIT_MQTT_SUCCESS) 
		{		
			eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_MQTT_CONNECTED);			
			EG_LOG_INFO("Connect egcloud server success.\r\n");
			EG_send_event_sem(EJ_EVENT_egTestSem);
		}
		else
		{
			EG_send_event_sem(EJ_EVENT_MQTTConnectionLostSem);
			//EG_mqtt_stop();
		}	

}

static void EG_event_mqtt_connection_lost_process(void* data)
{		
	/*
	*Need to Do ...................................
	*/
	EG_LOG_INFO("process an mqtt connection lost .\r\n");
	eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_MQTT_NOT_CONNECTED);
	//UnInit_MQTTThread();
	EG_mqtt_stop();
	
	
	//_g_pdevinfo->wifiStatus.cloudServiceStatus = CLOUD_NOT_CONNECTED;
	//EG_LOG_INFO("receive an mqtt connection lost event2.\r\n");
}

static void EG_event_default_process()
{
		//WIFIMODULE_ROUTER_STATUS wlan_state = eg_get_dev_info_wifistatus_routerStatus();
	unsigned char wlan_state = 0;
	int ret = -1;
	ret = EG_wlan_get_connection_state(&wlan_state);
	if(ret==0)
	{
		if(wlan_state == ROUTER_CONNECTED)
		{
			if(eg_get_dev_info_wifistatus_cloudServiceStatus()== CLOUD_MQTT_NOT_CONNECTED)
			//if(_g_pdevinfo->wifiStatus.cloudServiceStatus == CLOUD_NOT_CONNECTED)
			{	
				eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_MQTT_CONNECTTING);
				EG_send_event_sem(EJ_EVENT_routerConnectedSem);
				//EG_DEBUG("SEND NO1......");
			}

			if (eg_get_dev_info_wifistatus_udpbroadcastStatus()== WIFIMODULE_UDPBROADCAST_NOT_RUNNING) 
			{
				EG_send_event_sem(EJ_EVENT_udpBroadcastSem);
			}
			//GetWifiModuleStatusSignalStrength();
			eg_set_dev_info_wifistatus_routerStatus(ROUTER_CONNECTED);
			
			//_g_pdevinfo->wifiStatus.routerStatus = ROUTER_CONNECTED;
			eg_set_dev_info_wifistatus_workMode(WIFIMODULE_CLIENT_MODE);
			//_g_pdevinfo->wifiStatus.workMode  = WIFIMODULE_CLIENT_MODE;
			
    			
		}else
		{

			//GetWifiModuleStatusSignalStrength();
			eg_set_dev_info_wifistatus_routerStatus(ROUTER_NOT_CONNECTED);
			eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_MQTT_NOT_CONNECTED);
			//eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_FAILED);
			//_g_pdevinfo->wifiStatus.routerStatus = ROUTER_CONNECTED;
			
			//_g_pdevinfo->wifiStatus.routerStatus = ROUTER_NOT_CONNECTED;
			//_g_pdevinfo->wifiStatus.cloudServiceStatus = CLOUD_NOT_CONNECTED;

		}
	}


}
static void  EG_event_handler(eg_event_t event,void *data)
{	
	//EG_DEBUG("is %d\r\n",event);
	switch(event)
	{
		case  EJ_EVENT_shouldUARTThreadWorkSem:
			{	
				//EJ_event_shouldUARTThreadWorkProcess(NULL);
				break;
			}
		case  EJ_EVENT_shouldMQTTThreadWorkSem:
			{	
				//EJ_event_shouldMQTTThreadWorkProcess(NULL);
				break;
			}
		case  EJ_EVENT_ezconnectCompletedSem:
			{	
				//EJ_event_ezconnectCompletedProcess(NULL);
				break;
			}
		case  EJ_EVENT_routerConnectedSem:
			{	
				EJ_event_router_connected_process(NULL);
				break;
			}
		case  EJ_EVENT_broadcastRequestSem:
			{	
				//EJ_event_broadcastRequestProcess(NULL);
				break;
			}
		case  EJ_EVENT_broadcastResponseSem:
			{	
				//EJ_event_broadcastResponseProcess(NULL);
				break;
			}
		case  EJ_EVENT_uapStartedSem:
			{	
				//EJ_event_uapStartedProcess(NULL);
				break;
			}
		case  EJ_EVENT_easylinkRequestSem:
			{	
				//EJ_event_easylinkRequestProcess(NULL);
				break;
			}
		case  EJ_EVENT_softAPRequestSem:
			{	
				//EJ_event_softAPRequestProcess(NULL);
				break;
			}
		case  EJ_EVENT_airkissRequestSem:
			{	
				//uint8_t configmode  = WIFICONFIG_AIRKISS_MODE;
				//EJ_event_airkissRequestProcess(&configmode);
				break;
			}
		case  EJ_EVENT_joylinkRequestSem:
			{	
				//uint8_t configmode  = WIFICONFIG_JOYLINK_MODE;
				//EJ_event_joylinkRequestProcess(&configmode);
				break;
			}
		case  EJ_EVENT_MQTTConnectionLostSem:
			{	
				EG_event_mqtt_connection_lost_process(NULL);
				break;
			}
		case  EJ_EVENT_rebootRequestSem:
			{	
				//EJ_event_rebootRequestProcess(NULL);
				break;
			}
		case  EJ_EVENT_Reset2FactoryRequestSem:
			{	
				//EJ_event_Reset2FactoryRequestProcess(NULL);
				break;
			}
		case  EJ_EVENT_routerAuthFailedSem:
			{	
				//EJ_event_routerAuthFailedProcess(NULL);
				break;
			}
		case  EJ_EVENT_reApplyIDSem:
			{	
				EJ_event_reapply_did_process(NULL);
				break;
			}
		case  EJ_EVENT_timeSyncSem:
			{	
				//EJ_event_timeSyncProcess(NULL);
				break;
			}
		case  EJ_EVENT_homeApNotConfiguredSem:
			{	
				//EJ_event_homeApNotConfiguredProcess(NULL);
				break;
			}
		case EJ_EVENT_egTestSem:
			{
				EJ_event_egtest_process(NULL);
				break;
			}
		case EJ_EVENT_udpBroadcastSem:
			{	
				EG_event_udpbroadcast_process(NULL);
			}
		default:
			EG_event_default_process();
			break;

	}
	
}

static void EventTimerCB()
{
	eg_event_t event = 0;
	event = EG_event();
	EG_event_handler(event,NULL);		
}



void EG_start_event_machine()
{

	if(!EventTimer){
		
		if (EG_timer_create(&EventTimer,
			    "EventTimer",
			    EG_msec_to_ticks(2000),
			    &EventTimerCB,
			    NULL,
			    EG_TIMER_PERIODIC,
			    EG_TIMER_AUTO_ACTIVATE) != EG_SUCCESS) 
		{
				EG_LOG_ERROR("Failed to create EventTimer timer.\r\n");
		}
	}else{
		EG_timer_activate(EventTimer);
	}
	
}




static void get_deviceid_timer_cb()
{
	int ret = 0;
	uint8_t t_did=0 ;
	uint8_t *p_did = eg_get_dev_info_deviceid();
	
	t_did = p_did[0]|p_did[1]|p_did[2]|p_did[3]|p_did[4]|p_did[5];
	EG_LOG_INFO("Check device id is %d",t_did);
	if(t_did==0)
	{	if(eg_get_dev_info_wifistatus_cloudServiceStatus()== CLOUD_MQTT_CONNECTED)
			EG_request_deviceid(eg_get_dev_info_macaddr(), eg_get_dev_info_uuid());
	}else
	{
		eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_SUCCESS);
		EG_timer_deactivate(&eg_timer_reply_deviceid);
		
	}	
	return ret;
		
}

void EG_start_get_deviceid_timer()
{

	if(!eg_timer_reply_deviceid){
		
		if (EG_timer_create(&eg_timer_reply_deviceid,
			    "EventTimer",
			    EG_msec_to_ticks(1500),
			    &get_deviceid_timer_cb,
			    NULL,
			    EG_TIMER_PERIODIC,
			    EG_TIMER_AUTO_ACTIVATE) != EG_SUCCESS) 
		{
				EG_LOG_ERROR("Failed to create EventTimer timer.\r\n");
		}
	}else{
		EG_timer_activate(&eg_timer_reply_deviceid);
	}
	
}

static eg_thread_t eg_event_thread=NULL;
static eg_thread_stack_define(eg_event_thread_stack, 1024);

static void EventProcessCB(void *data)
{
	eg_event_t event = 0;
	while(1)
	{
		event = EG_event();
		EG_event_handler(event,NULL);
		EG_thread_sleep(500);
	}
			
}

void EG_start_event_machine1()
{

	if (eg_event_thread == 0) 
	{
		int ret = EG_thread_create(&eg_event_thread,"EG_Event_machine_thread",
			(void *)EventProcessCB, 0,&eg_event_thread_stack, EG_PRIO_0);

		if (ret!=EG_SUCCESS) 
		{
			EG_LOG_ERROR(" Unable to create event_machine.\r\n");
			return INIT_MQTT_OS_THREAD_CREATE_ERROR;
		}
	}
	else 
	{
		EJ_thread_resume(&eg_event_thread);
	}
	
}

//process cloud control command: cloud-->uart
void EG_cloud_msg_process(EG_DEV_MSG_CB f_dev_callback)
{
	EG_MSG_Packet *pCloud2DevicePacket = NULL;
	EG_DEV_Packet *pDevPacket = NULL;
	eg_list_pop(GetCloud2deviceList(), (void **)&pCloud2DevicePacket);
	if(pCloud2DevicePacket)
	{
		pDevPacket = EG_cloud_msg_extract(pCloud2DevicePacket);
		if(pDevPacket!=NULL)
		{
			f_dev_callback(pDevPacket->data,pDevPacket->lenth);
			EG_device_packet_free(pDevPacket);
		}
		EG_msg_packet_free(pCloud2DevicePacket);
	}
}


//upload uart data : uart-->cloud
int EG_device_snap_shot(char *out_snap, unsigned int out_len)
{

	int ret = 0 ;
	int len = 0 ;
    if(NULL == out_snap || out_len < 0){
        return ret;
    }
   /**
     *FIXME:must to do
     */
	//1.create a device packet 
	EG_DEV_Packet *pDevPacket  = (EG_DEV_Packet *)EG_mem_malloc(sizeof(EG_DEV_Packet));

	//2.push a device packet into device2cloudlist
	if(pDevPacket!=NULL)
	{
		pDevPacket->type = EG_DEVICE_MSG_DATA;
		pDevPacket->lenth= 0;
		pDevPacket->data = NULL;
		//EG_device_deviceid_get(pDevPacket->deviceID,len);
		memcpy(pDevPacket->deviceID,eg_get_dev_info_deviceid(),6);
		pDevPacket->data =(uint8_t *)EG_mem_malloc(out_len);
		if(pDevPacket->data != NULL)
		{
			memcpy(pDevPacket->data,out_snap,out_len);
			pDevPacket->lenth = out_len;
			if(EG_queue_send(EG_msg_queue_get(EG_CLOUD_MSG_OUT),&pDevPacket,EG_NO_WAIT)!=EG_SUCCESS)
			{
				EG_LOG_ERROR("add an packet to device2cloudQueue failed!\r\n");
				EG_device_packet_free(pDevPacket);
				ret = EG_FAIL;
			}
			
		}else
		{
			EG_device_packet_free(pDevPacket);
			ret = EG_FAIL;
		}
		
	}
	else
	{	
		ret = EG_FAIL;
	}
	return ret;
}

static void EG_test(void *data)
{
		
		char out_snap[30];
		int  out_len = 30;
		EG_DEBUG("Start EG_test!!!!!!!!");
		while(1)
		{
			EG_device_snap_shot(out_snap, out_len);
			//EG_HEX(out_snap,out_len);
			EG_cloud_msg_process(eg_msg_process);
			EG_thread_sleep(2000);
		}		
}		


int EG_test_start()
{

	//EG_thread_create(&eg_test_thread,"EG_test_thread",(void *)EG_test,NULL,1024,EG_PRIO_3);
	if (eg_test_thread == 0) 
	{
		int ret = EG_thread_create(&eg_test_thread,"EG_test_thread",
			(void *)EG_test, 0,&eg_test_thread_stack, EG_PRIO_0);

		if (ret!=EG_SUCCESS) 
		{
			EG_LOG_ERROR(" Unable to create MQTTSendThread.\r\n");
			return INIT_MQTT_OS_THREAD_CREATE_ERROR;
		}
	}
	else 
	{
		EJ_thread_resume(&eg_test_thread);
	}

	
}


int EG_start(const char* uuid,const char* macaddr)
{
	int ret = 0 ;
	ret = EG_service_init(uuid,macaddr);
	//1 init hardware and start mqtt thread
	if(ret!=EG_SUCCESS)
	{	
		EG_LOG_ERROR("EG_service_init failed ,result=%d",ret);
	}else{

		//EG_start_get_deviceid_timer();
		EG_start_event_machine1();
		//EG_start_event_machine();
	}
	EG_test_start();
	return ret;
}




