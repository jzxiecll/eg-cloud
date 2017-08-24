#include "eg_cloud.h"
#include "eg_mqtt.h"
#include "eg_porting_layer.h"
#include "eg_event.h"
eg_timer_t EventTimer=NULL;
int EG_test_start();

uint8_t*eg_msg_process(unsigned char *r_pain_text,int len)
{
	EG_HEX(r_pain_text,len);
}

static void EJ_event_reapply_did_process(void* data)
{
	EG_request_deviceid(eg_get_dev_info_macaddr(), eg_get_dev_info_uuid());
}

static void EJ_event_egtest_process(void* data)
{
	EG_D("EG_event_egtest and Start EG test ................................");
	EG_test_start();	
}

static void EJ_event_router_connected_process(void* data)
{
	eg_set_dev_info_wifistatus_routerStatus( ROUTER_CONNECTED);
	/* if cloud services is not connected. */
	if (eg_get_dev_info_wifistatus_cloudServiceStatus()== CLOUD_NOT_CONNECTED) 
	{
	
		if (EG_mqtt_start() == INIT_MQTT_SUCCESS) 
		{		
			eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_CONNECTED);			
			EG_I("Connect egcloud server success.\r\n");
			EG_send_event_sem(EJ_EVENT_egTestSem);
		}else
		{
			EG_send_event_sem(EJ_EVENT_routerConnectedSem);
		}	

	}
}

static void EG_event_mqtt_connection_lost_process(void* data)
{		
	/*
	*Need to Do ...................................
	*/
	EG_E("receive an mqtt connection lost event.\r\n");
	//UnInit_MQTTThread();
	EG_mqtt_stop();
	eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_NOT_CONNECTED);
	//_g_pdevinfo->wifiStatus.cloudServiceStatus = CLOUD_NOT_CONNECTED;

}

static void EG_event_default_process()
{
	WIFIMODULE_ROUTER_STATUS wlan_state = eg_get_dev_info_wifistatus_routerStatus();
	int ret = EG_wlan_get_connection_state(&wlan_state);
	EG_D("ret  = %d,wlan_state = %d\r\n",ret,wlan_state);
	//int ret = 0;
	if(ret==0)
	{
		if(wlan_state == ROUTER_CONNECTED)
		{
			if(eg_get_dev_info_wifistatus_cloudServiceStatus()== CLOUD_NOT_CONNECTED)
			//if(_g_pdevinfo->wifiStatus.cloudServiceStatus == CLOUD_NOT_CONNECTED)
			{
				EG_send_event_sem(EJ_EVENT_routerConnectedSem);
			}
			//GetWifiModuleStatusSignalStrength();
			eg_set_dev_info_wifistatus_routerStatus(ROUTER_CONNECTED);
			
			//_g_pdevinfo->wifiStatus.routerStatus = ROUTER_CONNECTED;
			eg_set_dev_info_wifistatus_workMode(WIFIMODULE_CLIENT_MODE);
			//_g_pdevinfo->wifiStatus.workMode  = WIFIMODULE_CLIENT_MODE;

			//4.check did
			if(eg_get_dev_info_wifistatus_deviceIDRequestStatus()== WIFIMODULE_DEVICEID_FAILED)
			{
				EG_device_check_deviceid();
			}
    			
		}else
		{

						//GetWifiModuleStatusSignalStrength();
			eg_set_dev_info_wifistatus_routerStatus(ROUTER_NOT_CONNECTED);
			eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_FAILED);
			//_g_pdevinfo->wifiStatus.routerStatus = ROUTER_CONNECTED;
			eg_set_dev_info_wifistatus_cloudServiceStatus(CLOUD_NOT_CONNECTED);
			//_g_pdevinfo->wifiStatus.routerStatus = ROUTER_NOT_CONNECTED;
			//_g_pdevinfo->wifiStatus.cloudServiceStatus = CLOUD_NOT_CONNECTED;

		}
	}

}
static void  EG_event_handler(eg_event_t event,void *data)
{	
	EG_D("is %d\r\n",event);
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
				EG_E("Failed to create EventTimer timer.\r\n");
		}
	}else{
		EG_timer_activate(EventTimer);
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
		EG_str2hex(pDevPacket->deviceID,eg_get_dev_info_deviceid());
		pDevPacket->data =(uint8_t *)EG_mem_malloc(out_len);
		if(pDevPacket->data != NULL)
		{
			memcpy(pDevPacket->data,out_snap,out_len);
			pDevPacket->lenth = out_len;
			if(EG_queue_send(EG_msg_queue_get(EG_CLOUD_MSG_OUT),&pDevPacket,EG_NO_WAIT)!=EG_SUCCESS)
			{
				EG_I("add an packet to device2cloudQueue failed!\r\n");
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
		EG_D("Start EG_test!!!!!!!!");
		while(1)
		{
			EG_device_snap_shot(out_snap, out_len);
			EG_HEX(out_snap,out_len);
			EG_cloud_msg_process(eg_msg_process);
			EG_thread_sleep(1000);
		}
		
}		
static eg_thread_t eg_test_thread =0 ;
static eg_thread_stack_define(eg_test_thread_stack, 512);

int EG_test_start()
{

	//EG_thread_create(&eg_test_thread,"EG_test_thread",(void *)EG_test,NULL,1024,EG_PRIO_3);
	if (eg_test_thread == 0) 
	{
		int ret = EG_thread_create(&eg_test_thread,"EG_test_thread",
			(void *)EG_test, 0,&eg_test_thread_stack, EG_PRIO_3);

		if (ret!=EG_SUCCESS) 
		{
			EG_E(" Unable to create MQTTSendThread.\r\n");
			return INIT_MQTT_OS_THREAD_CREATE_ERROR;
		}
	}
	else 
	{
		EJ_task_Resume(eg_test_thread);
	}

	
}


int EG_start()
{
	int ret = 0 ;

	//1 init hardware and start mqtt thread
	if(EG_service_init()==EG_SUCCESS)
	{	
	}
}




