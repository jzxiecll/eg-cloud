#include "eg_mqtt.h"

#define EG_MQTT_TEXT_LENTH 256
unsigned char ethersignStr[] = "qHUnA0xB%`[a#},HZLkK.jQ|;6qxzb5NoJdZ";
unsigned char sendbuf[256], readbuf[256];
unsigned char cipher[256];
unsigned char eg_mqtt_text[EG_MQTT_TEXT_LENTH];

static eg_thread_t MQTTSendThread_thread = NULL;
static eg_thread_stack_define(MQTTSendThread_stack, 2048);
static eg_thread_t MQTTReceiveThread_thread = NULL;
static eg_thread_stack_define(MQTTReceiveThread_stack, 2048);

opts_struct *opts = NULL;
static int mqtt_thread_send_flag = 0;
static int mqtt_thread_recv_flag = 0;

uint8_t Process_RequestDeviceIDFromCloudResponseCB(EG_MSG_Packet *pPacket)
{
	if (pPacket) 
	{
		if (pPacket->data) 
		{
			uint8_t result = pPacket->data[0];
			uint8_t *pdid  = (uint8_t *)pPacket->data+2;
			if (result == 0) {
				uint8_t temp[13]={0};
				sprintf(temp,"%02X%02X%02X%02X%02X%02X",pdid[0],pdid[1],pdid[2],pdid[3],pdid[4],pdid[5]);
				EG_device_deviceid_set(temp);
				//_g_pam->deviceid.set(temp);//storeDeviceInfoDeviceid(tmp);
				eg_set_dev_info_deviceid(pdid,6);
				eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_SUCCESS);
				//memcpy(_g_pdevinfo->deviceid,pdid,6);
			}else {
				eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_FAILED);
				EG_LOG_ERROR("request deviceID failed.\r\n");

			}
		}
	}

	return 0x00;
}

void EG_msg_cmd_register()
{

	EG_cloud_msg_callback_init();
	
	EG_cloud_msg_callback_register(0x8015, Process_RequestDeviceIDFromCloudResponseCB); 
	//EG_cloud_msg_callback_register(0x8022, Process_WifiModuleNetworkStateRequestCB); 
	//EG_cloud_msg_callback_register(0x8041, Process_getSystemTimeFromCloudResponseCB); 
	//EG_cloud_msg_callback_register(0x0058, Process_GetWifiModuleUpgradeResponseCB); 
	//EG_cloud_msg_callback_register(0x8051, Process_ReportWifiModuleInfoToCloudRequest);
	//EG_cloud_msg_callback_register(0x0061, Process_QueryDeviceFirmwareVersionRequest);
	//EG_cloud_msg_callback_register(0x0062, Process_DeviceFirmwareVersionUpdateRequest);
	//EG_cloud_msg_callback_register(0x0070, Process_ModifyCloudServiceAddrRequest);
	//EG_cloud_msg_callback_register(0x007B, Process_heartBeatRequest);	
	//EG_cloud_msg_callback_register(0x0090, Process_WifiModuleConfigRequest);
	//EG_cloud_msg_callback_register(0x80B1, Process_WifiModuleRebootRequest);
	//EG_cloud_msg_callback_register(0x80B2, Process_WifiModuleResetToFactoryRequest);
	//registerMQTTCommandCallback(0x00E1, Process_WifiModuleDiscoverRequest);	
	
}




int EG_device_check_deviceid()
{
	int ret = 0;
	uint8_t t_did=0 ;
	uint8_t *p_did = eg_get_dev_info_deviceid();
	
	t_did = p_did[0]|p_did[1]|p_did[2]|p_did[3]|p_did[4]|p_did[5];
	EG_LOG_INFO("Check device id is %d",t_did);
	if(t_did==0&&eg_get_dev_info_wifistatus_cloudServiceStatus()== CLOUD_MQTT_CONNECTED)
	{
		EG_send_event_sem(EJ_EVENT_reApplyIDSem);
	}else
	{
		eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_SUCCESS);
	}	
	return ret;
}


static void EG_PublishMQTTMessage(char *topicName, int qos, int retained, void *payload, int dataLen)
{
	int rc = 0;
	MQTTMessage message;
	message.qos = qos;
	message.retained = retained;
	message.payload = payload;
	message.payloadlen = dataLen;

	if ((rc = MQTTPublish(&opts->client, topicName, &message)) != 0)
	{
		EG_LOG_ERROR(" Unable to publish topic.\r\n");
	}

}

void EG_msg_pub(EG_MSG_Packet *pPacket, char *topicName)
{
	uint32_t pack_len = (pPacket->dataLen[0])|(pPacket->dataLen[1]<<8)|(pPacket->dataLen[2]<<16)|(pPacket->dataLen[3]<<24);

	if (pPacket) {			
		uint8_t *buf = (uint8_t *)EG_mem_malloc(pPacket->dataLen[0] - 16 -8);
		uint8_t *temp = (uint8_t *)pPacket;
		if (buf) {

			memcpy(buf, temp + 8, 20);
			memcpy(buf + 20, pPacket->data, pPacket->dataLen[0] - 20 - 16 - 8);	
			
			//aes_result_dump(buf, pPacket->dataLen[0] - 16 - 8);
			int cipherSize = EG_Aes_Encrypt(buf, (unsigned int)pPacket->dataLen[0] - 16 - 8, cipher);
			//aes_result_dump(cipher, cipherSize);
			if (cipherSize) {
				
				uint8_t *payload = (uint8_t *)EG_mem_malloc(8 + cipherSize + strlen(ethersignStr));
				if (payload) {

					memcpy(payload, pPacket, 8);
					memcpy(payload + 8, cipher, cipherSize);
					memcpy(payload + 8 + cipherSize, ethersignStr, strlen(ethersignStr));

					uint8_t tk[16];
					EG_hash_md5(payload, 8 + cipherSize + strlen(ethersignStr), tk, 16);
					memcpy(payload + 8 + cipherSize, tk, 16);
					
					EG_PublishMQTTMessage(topicName, opts->qos, opts->retained, payload, 8 + cipherSize + 16);
					EG_mem_free(payload);

				}else {
					EG_LOG_ERROR("EG_mem_malloc failed.\r\n");
				}

			}

			EG_mem_free(buf);
			
		}else {
			EG_LOG_ERROR(" EG_mem_malloc failed.\r\n");
		}

		
	}
}


void EG_msg_arrived(MessageData* m)
{
		 EG_P("EG_messageArrived \r\n");
		 uint8_t *data = (uint8_t *)(m->message->payload);
		 uint32_t pack_len = (data[4])|(data[5]<<8)|(data[6]<<16)|(data[7]<<24);
		 EG_MSG_Packet *pPacket = (EG_MSG_Packet*)EG_mem_malloc(sizeof(EG_MSG_Packet));
		 if (pPacket) { 

			 pPacket->head[0] = data[0];
			 pPacket->head[1] = data[1];
			 pPacket->version = data[2];
			 pPacket->crypt = data[3];
			 pPacket->dataLen[0] = data[4];
			 pPacket->dataLen[1] = data[5];
			 pPacket->dataLen[2] = data[6];
			 pPacket->dataLen[3] = data[7];
			 //EJ_Printf("EJ_messageArrived dataLen[0]=%d ,payloadlen =%d \r\n",pPacket->dataLen[0],m->message->payloadlen);
			 memset(eg_mqtt_text,0,EG_MQTT_TEXT_LENTH);
			 if (EG_Aes_Decrypt((unsigned char *)(data + 8), m->message->payloadlen - 8 - 16, eg_mqtt_text) == 0)
			 {
				 EG_P("error EG_Aes_Decrypt\r\n");
				 EG_mem_free(pPacket);
				 return ;
				 
			 }
			 //EJ_Printf("success EJ_Aes_Decrypt\r\n");
			 pPacket->data = (uint8_t *)EG_mem_malloc(pack_len - EG_CLOUDMSG_PERFIX_LENTH);
			 if (pPacket->data) {
				 pPacket->dataType[0] = eg_mqtt_text[0];
				 pPacket->dataType[1] = eg_mqtt_text[1];
				 pPacket->dataID[0] = eg_mqtt_text[2];
				 pPacket->dataID[1] = eg_mqtt_text[3];
				 pPacket->dataID[2] = eg_mqtt_text[4];
				 pPacket->dataID[3] = eg_mqtt_text[5];
	 
				 memcpy(pPacket->timeStamp, eg_mqtt_text + 6, 8);
				 memcpy(pPacket->deviceID, eg_mqtt_text + 14, 6);
				 memcpy(pPacket->data, eg_mqtt_text + 20, pack_len - EG_CLOUDMSG_PERFIX_LENTH);
				 memcpy(pPacket->signature, (uint8_t*)m->message->payload + m->message->payloadlen - 16, 16);
				 EG_cloud_msg_print(pPacket, "[EG_msg_arrived]");
				 if ((pPacket->dataType[0] == 0x20) && (pPacket->dataType[1] == 0x00)) {
	 
					 /* add this packet to cloud2deviceList. */
					 if(eg_list_push(GetCloud2deviceList(), pPacket)!=1){
						 EG_LOG_ERROR("eg_list_push to Cloud2deviceList failed!\r\n"); 
						 EG_mem_free(pPacket->data);
						 EG_mem_free(pPacket);
					 }
				 }
				 else {
					 /* add this packet to cloud2wifiList. */
					 if(eg_list_push(GetCloud2wifiList(), pPacket)!=1){
						 EG_LOG_ERROR("eg_list_push to Cloud2wifiList failed!\r\n");
						 EG_mem_free(pPacket->data);
						 EG_mem_free(pPacket);
					 }
				 }			 
			 }else {
				 EG_LOG_ERROR("EG_mem_malloc wifi2CloudPacket->data failed.\r\n");
			 }
			 
		 }else{
			 EG_LOG_ERROR("EG_mem_malloc wifi2CloudPacket failed.\r\n");
		 }
	 
}

static void EG_msg_queue_clear()
 {
 	 int i=0;
 	 int packetCount = EG_queue_get_msgs_waiting(EG_msg_queue_get(EG_CLOUD_MSG_OUT));
	 EG_DEBUG("Clear Device2cloudQueue Count :[%d]\r\n",packetCount);
 	 for(i = 0; i < packetCount; i++) 
 	 {	 
 		 EG_MSG_Packet *pDevice2CloudPacket = NULL;
 		 if (EG_queue_recv(EG_msg_queue_get(EG_CLOUD_MSG_OUT), &pDevice2CloudPacket, EG_msec_to_ticks(1000)) == EG_SUCCESS) 
 		 {
 			 EG_msg_packet_free(pDevice2CloudPacket);	 
 		 }
 	 }
 }	


void EG_msg_send(void* arg)
{
		int rc = 0;
		int dataLen = 0;
		EG_DEBUG("EG_msg_send thread start.\r\n");
		EG_msg_queue_clear();
		for (;;)
		{
			/* process wifi to cloud.*/ 	
			/*read wifi2cloud list and send */
			EG_MSG_Packet *pPacket = NULL;
			eg_list_pop(GetWifi2cloudList(), (void **)&pPacket);
			if (pPacket) {
	
				EG_msg_pub(pPacket, opts->pubTopic);
				EG_cloud_msg_print(pPacket, "[EG_MSG_Packet	]");
				EG_msg_packet_free(pPacket);
			}
	
			/* process device to cloud.*/
			/*read device2cloud list and send */
			int packetCounts = EG_queue_get_msgs_waiting(EG_msg_queue_get(EG_CLOUD_MSG_OUT));
			int i = 0;
			for(i = 0; i < packetCounts; i++) {
				EG_DEV_Packet *pDevPacket = NULL;
				if (EG_queue_recv(EG_msg_queue_get(EG_CLOUD_MSG_OUT), &pDevPacket, EG_msec_to_ticks(100)) == EG_SUCCESS) { 	
					/* decode uart2WifiPacket and encode wifi2CloudPacket ...*/
#if 1					
					EG_MSG_Packet *pWifi2CloudPacket = EG_cloud_msg_create(pDevPacket,eg_get_dev_info_deviceid());
					if (pWifi2CloudPacket) {

						EG_msg_pub(pWifi2CloudPacket,opts->pubTopic);
						EG_cloud_msg_print(pWifi2CloudPacket,"[EG_cloud_msg_create]");
						EG_msg_packet_free(pWifi2CloudPacket);
					}else {
						EG_LOG_ERROR("EG_mem_malloc wifi2CloudPacket failed!\r\n"); 
					}	
#endif
					EG_device_packet_free(pDevPacket);
				}
			}
			EG_thread_sleep(EG_msec_to_ticks(10));
		}
	
		
}

static int EG_check_mqtt_connect_stat()
{
		int rc = 0;
		if ((rc = MQTTYield(&opts->client, 200)) == FAILURE) {
			EG_LOG_ERROR(" MQTTYield failed.\r\n");
		}
		else if (rc == CONNECTION_LOST) {
			EG_send_event_sem(EJ_EVENT_MQTTConnectionLostSem);
			EG_LOG_ERROR(" MQTT connection lost.\r\n");
		}

		return rc;

}


void EG_msg_recv(void* arg)
{
	//process GetCloud2wifiList()
	EG_MSG_Packet  *pCloud2WifiPacket=NULL;
	EG_CLOUD_MSG_CB f_cb = NULL;
	uint32_t commandID = 0;
	int rc = 0;
	for (;;)
	{

			//EG_check_mqtt_connect_stat();
			if(CONNECTION_LOST==EG_check_mqtt_connect_stat())
			{
				break;
			}
			if(eg_list_pop(GetCloud2wifiList(), (void **)&pCloud2WifiPacket)==0x01)
			{
				commandID = (uint32_t)(pCloud2WifiPacket->dataType[1] << 8 | pCloud2WifiPacket->dataType[0]);
				f_cb= EG_cloud_msg_callback_get(commandID);
				if(f_cb!=NULL)
				{
					f_cb(pCloud2WifiPacket);
					if (commandID == 0x8015) 
					{
							/* report wifimodule info to cloud. */
							
							/* report wifimodule status to cloud. */

							/* report device info to cloud. */
							
							/* report device status to cloud. */	
					}

				}else
				{
					EG_LOG_ERROR(" NULL MQTTCommand %x Callback.\r\n", commandID);
				}
			}

			
			EG_msg_packet_free(pCloud2WifiPacket);
			pCloud2WifiPacket=NULL;
			f_cb = NULL;
			commandID = 0;
			EG_thread_sleep(EG_msec_to_ticks(10));
			
	}

	EG_DEBUG("EG_msg_recv finish.....");
	EG_mqtt_stop();
	//EG_thread_self_complete(NULL);
	//EG_thread_Suspend(&MQTTReceiveThread_thread);
	

}


opts_struct *EG_msg_opts_of_connection_generate()
{

	opts_struct *opt = (opts_struct *)EG_mem_malloc(sizeof(opts_struct));
	if(!opt){
		EG_LOG_ERROR("GenerateMQTTConnectionerOpts failed!\r\n");
		return NULL;
	}
	memset(opt, 0, sizeof(opts_struct));
	opt->clientid = (char*)EG_mem_malloc(13);
	if(!opt->clientid){
		EG_mem_free(opt);
		return NULL;
	}
	memset(opt->clientid,0,13);
	memcpy(opt->clientid,eg_get_dev_info_macaddr(),12);
	//_device_mac_get(opt->clientid);
	EG_P("opt->clientid ==>%s\r\n",opt->clientid);
	opt->nodelimiter = 0;
	opt->delimiter = "\n";
	opt->qos = 1;	
	opt->username = NULL;
	opt->password = NULL;
#if 0
	bool userDomain = false;

	char *cloud_domain = GetWifiConfigCloudDomainAddr();
	if (cloud_domain) {
		int a, b, c, d;
		int ret=sscanf(cloud_domain,"%d.%d.%d.%d",&a,&b,&c,&d); 
		if(ret==4&&(a>=0&&a<=255)&&(b>=0&&b<=255)&&(c>=0&&c<=255)&&(d>=0&&d<=255))  
		{  
			opt->host = cloud_domain;
		}else {

			struct hostent *host;
			host = gethostbyname(cloud_domain);
		
			if (host != NULL) {
				if (opt->host == NULL) {
					opt->host = (char *)EJ_mem_malloc(16);
				}
				if (inet_ntop(AF_INET, (void *)(host->h_addr), opt->host, 16)) {					
					userDomain = true;
				}else {
					EJ_ErrPrintf(("[GenerateMQTTConnectionerOpts]][ERROR]: inet_ntop failed!\r\n"));
				}
				opt->port = 1883;
			}else {
				EJ_ErrPrintf(("[GenerateMQTTConnectionerOpts]][ERROR]: gethostbyname failed!\r\n"));
			}
		}
	}else {
		EJ_ErrPrintf(("[GenerateMQTTConnectionerOpts]][ERROR]: null cloud domain addr failed!\r\n"));
	}

	if (!userDomain) {
		uint8_t defaultConnectServer = GetWifiConfigDefaultConnectServer();
		if (defaultConnectServer == 0) {		
			opt->host = GetWifiConfigMasterServerAddr();
			opt->port = GetWifiConfigMasterServerPort();	
		
		}else if (defaultConnectServer == 1){
			opt->host = GetWifiConfigBackupServer1Addr();
			opt->port = GetWifiConfigBackupServer1Port();		
		}else if (defaultConnectServer == 2){
			opt->host = GetWifiConfigBackupServer2Addr();
			opt->port = GetWifiConfigBackupServer2Port();
		}else {
		
		}

	}

#else

	if(EJ_TEST_SERVER){
		opt->host = SERVER_ADDR_TEST; //test
		opt->pubTopic = SERVER_TOPIC_TEST;

	}else{
		opt->host = SERVER_ADDR_MASTER;   
   		opt->pubTopic =SERVER_TOPIC_MASTER;
	}


#endif
	opt->port = 1883;
	opt->showtopics = 1;
	opt->topic = "#";
	opt->retained = 1;
	opt->subTopic = (char *)EG_mem_malloc(13);

	if(!opt->subTopic){
		EG_mem_free(opt->clientid);
		EG_mem_free(opt);
		return NULL;
	}

	//_device_mac_str_get(opt->subTopic);
	memset(opt->subTopic,0,12);
	memcpy(opt->subTopic,eg_get_dev_info_macaddr(),6);
	
	//sprintf(opt->subTopic + 0, "%c", _ej_dev.mac[0]);
	//sprintf(opt->subTopic + 1, "%c", _ej_dev.mac[1]);
	//sprintf(opt->subTopic + 2, "%c", _ej_dev.mac[2]);
	//sprintf(opt->subTopic + 3, "%c", _ej_dev.mac[3]);
	//sprintf(opt->subTopic + 4, "%c", _ej_dev.mac[4]);
	//sprintf(opt->subTopic + 5, "%c", _ej_dev.mac[5]);
	EG_P("mqtt conntect opts: ip= %s port= %d user=%s pass=%s subTopic=%s.\r\n", opt->host, opt->port, opt->username, opt->password,opt->subTopic);

	return opt;
}



static uint8_t EG_connect_mqtt_server()
{
	int rc = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	NewNetwork(&opts->network);
	MQTTClient(&opts->client, &opts->network, 3000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
	if (NetworkConnect(&opts->network, opts->host, opts->port) != EG_SUCCESS)
	{		
		MQTTClientDeinit(&opts->client);
		return MQTT_CREATE_SOCKET_ERROR;
	}
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = opts->clientid;
	connectData.username.cstring = NULL;
	connectData.password.cstring = NULL;
	connectData.keepAliveInterval = 15;
	connectData.willFlag = 1;
	connectData.will.topicName.cstring = "ERR/ALL/ALL";
	connectData.will.message.cstring = opts->clientid;
	connectData.will.retained = 1;
	connectData.will.qos = 1;

	if ((rc = MQTTConnect(&opts->client, &connectData)) != 0) {
		EG_LOG_ERROR("connnect MQTT server failed.\r\n");
#if 0		
		if (opts->network.my_socket) {
			close(opts->network.my_socket);
			opts->network.my_socket = -1;
		}
#else
		NetworkDisconnect(&opts->network);
#endif
		//MQTTDisconnect(&opts->client);
		MQTTClientDeinit(&opts->client);
		return MQTT_CONNECTED_ERROR;
	}
	else
		EG_LOG_INFO("connnect MQTT server success.\r\n");
#if 0
	//if ((rc = MQTTSubscribe(&opts->client, opts->subTopic, 2, messageArrived)) != 0)
	if ((rc = MQTTSubscribe(&opts->client, "F0AD4E0335E9", 2, EJ_messageArrived)) != 0)
		EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: Unable to subscribe topic.\r\n"));

	//if ((rc = MQTTSubscribe(&opts->client, opts->clientid, 2, messageArrived)) != 0)
	if ((rc = MQTTSubscribe(&opts->client, "XBMBJA", 2, EJ_messageArrived)) != 0)
		EJ_ErrPrintf(("[MQTTThread.c][ConnectToMQTTServer][ERROR]: Unable to subscribe topic.\r\n"));
#else
		//static  uint8_t  TopicUuid[EG_HARDWEAR_UUID_STRTING_LEN+1]={0};
		//static  uint8_t  TopicMac[EG_HARDWEAR_MAC_STRTING_LEN+1]={0};
		//unsigned int len=0 ;
		//EG_device_macaddr_get(TopicMac,len);
		//EG_device_uuid_get(TopicUuid,len);
		//EG_P("%s->uuid:%s,mac:%s\r\n",__FUNCTION__,TopicUuid,TopicMac);
			if ((rc = MQTTSubscribe(&opts->client, STR_WIFI_MODULE_FOTATOPIC, 2, EG_msg_arrived)) != 0)
			{
				EG_LOG_ERROR("Unable to subscribe topic3.\r\n");
				//EG_device_reboot(EG_DEVICE_OF_WIFI);
			}else{
				//EG_LOG_INFO("able to subscribe topic3.\r\n");
			}
		if ((rc = MQTTSubscribe(&opts->client, (const char*)eg_get_dev_info_macaddr(), 2, EG_msg_arrived)) != 0)
			{
				EG_LOG_ERROR("Unable to subscribe topic1. %s\r\n",eg_get_dev_info_macaddr());
				//EG_device_reboot(EG_DEVICE_OF_WIFI);
			}else{
				//EG_LOG_INFO("able to subscribe topic1.\r\n");
			}	
		if ((rc = MQTTSubscribe(&opts->client, (const char*)eg_get_dev_info_uuid(), 2, EG_msg_arrived)) != 0)
			{
				EG_LOG_ERROR("Unable to subscribe topic2.%s\r\n",(const char*)eg_get_dev_info_uuid());
				//EG_device_reboot(EG_DEVICE_OF_WIFI);
				
			}else{
				//EG_LOG_INFO("able to subscribe topic2.\r\n");
			}
		
			
	
#endif
	return MQTT_CONNECTED_SUCCESS;
}

static int EG_user_connectserver()
{
	int ret = INIT_MQTT_CONNECTION_ERROR;
	static uint8_t connectTimes = 0;
	static CONNECT_MECHANISM mechanism = MECHANISM_3S;

	if (EG_connect_mqtt_server() == MQTT_CONNECTED_SUCCESS) {		
		mechanism = MECHANISM_3S;
		connectTimes = 0;
		return MQTT_CONNECTED_SUCCESS;
	}
	
	switch(mechanism) 
	{
		case MECHANISM_3S: 
		{
			connectTimes++;
			if (connectTimes >= 4) 
			{
			  connectTimes = 0;
			  mechanism = MECHANISM_10S;
			  EG_thread_sleep(EG_msec_to_ticks(10000));
			}else 
			{
			  EG_thread_sleep(EG_msec_to_ticks(3000));
			}  
			break;
	  	}

	  	case MECHANISM_10S:
		{
		    connectTimes++;    
		    if (connectTimes >= 2) 
			{
		      connectTimes = 0;
		      mechanism = MECHANISM_25S;
		      EG_thread_sleep(EG_msec_to_ticks(25000));     
		    }else {      
		      EG_thread_sleep(EG_msec_to_ticks(10000));
		    }
		    break;
	  	
	  	}

	  	case MECHANISM_25S: 
		{
	    	connectTimes++;  
		    if (connectTimes >= 2) 
			{
		      	connectTimes = 0;
		      	mechanism = MECHANISM_3S;
		  		EG_device_reboot(EG_DEVICE_OF_WIFI);
				ret = MQTT_CONNECT_ERROR_FINISHED;

		    }else 
		    {
		      EG_thread_sleep(EG_msec_to_ticks(25000)); 
		    }
		    break;
	    }

		default:
			break;

	}

	return ret;
}







int EG_mqtt_start()
{
	static int i = 0;
	int ret;
	if (i == 0) 
	{
		opts = EG_msg_opts_of_connection_generate();
		if(!opts)
		{
			EG_LOG_ERROR("Call EG_msg_opts_of_connection_generate failed \r\n");
			return INIT_MQTT_CONNECTION_ERROR;
		}
		i = 1;
	}
	
	//2.wait for network up
	//EG_wait_network_up();
	ret = EG_user_connectserver();
	if ( ret!= MQTT_CONNECTED_SUCCESS) 
	{
		EG_LOG_ERROR("Call EG_user_connectserver failed! ret=%d\r\n",ret);
		return INIT_MQTT_CONNECTION_ERROR;
	}
	
	if (mqtt_thread_send_flag == 0) 
	{
			ret = EG_thread_create(&MQTTSendThread_thread,"EG_SendThread",
				(void *)EG_msg_send, 0,&MQTTSendThread_stack, EG_PRIO_3);

			if (ret!=EG_SUCCESS) 
			{
				EG_LOG_ERROR(" Unable to create MQTTSendThread.\r\n");
				return INIT_MQTT_OS_THREAD_CREATE_ERROR;
			}
			mqtt_thread_send_flag = 1;
	}
	else 
	{
		EJ_thread_resume(&MQTTSendThread_thread);
	}

	if (mqtt_thread_recv_flag == 0) 
	{
			 ret = EG_thread_create(&MQTTSendThread_thread,"EG_ReceiveThread",
				(void *)EG_msg_recv, 0,&MQTTReceiveThread_stack, EG_PRIO_3);

			if (ret!=EG_SUCCESS) 
			{
				EG_LOG_ERROR(" Unable to create MQTTRecvThread.\r\n");
				return INIT_MQTT_OS_THREAD_CREATE_ERROR;
			}
			mqtt_thread_recv_flag = 1;
	}
	else 
	{
		EJ_thread_resume(&MQTTReceiveThread_thread);
	}

	EG_LOG_INFO("EG_service_start success \r\n");
	return INIT_MQTT_SUCCESS;
}



int EG_service_init(const char* uuid,const char* macaddr)
{
		
		int ret = 0 ;
		if(uuid == NULL||macaddr==NULL)
			return EG_E_INVAL;
		//1.create queue for cloud msg out:device2cloud
		if(EG_msg_queue_create()!=0)
		{
			return EG_FAIL;
		}

		//2.create list for device msg in
		if(EG_mqtt_list_create()!=0)
		{
			return EG_FAIL;
		}

		EG_msg_cmd_register();

		//3.init hardware
		//EG_device_init("POOvcN","112233445566");
		EG_device_init(uuid,macaddr);
		//3.create event queue 
		if(EG_event_init()!=0)
		{
			return EG_FAIL;
		}
		else
		{
			//EG_start_event_machine();
			//EG_start_event_machine1();
		}


		//5.start EG service

		return EG_SUCCESS;
}



int EG_mqtt_stop()
{
#if 0
	/**/
	MQTTDisconnect(&opts->client);

	EG_thread_Suspend(&MQTTSendThread_thread);
	EG_thread_Suspend(&MQTTReceiveThread_thread);

	MQTTClientDeinit(&opts->client);
	
	if (opts->network.my_socket) {

		close(opts->network.my_socket);

		opts->network.my_socket = -1;
	}

#endif	

#if 0
	MQTTDisconnect_v2(&opts->client);
	if (MQTTSendThread_thread != 0) {
		EG_thread_delete(&MQTTSendThread_thread);
		MQTTSendThread_thread = 0;
	}

	if (MQTTReceiveThread_thread != 0) {
		EG_thread_delete(&MQTTReceiveThread_thread);
		MQTTReceiveThread_thread = 0;
	}

	MQTTClientDeinit(&opts->client);
	NetworkDisconnect(&opts->network);
	return 0;
#endif  


    EG_DEBUG("mqtt service stop1...");
	MQTTDisconnect(&opts->client);
	MQTTClientDeinit(&opts->client);
	NetworkDisconnect(&opts->network);
	
	mqtt_thread_send_flag = 0;
	mqtt_thread_recv_flag = 0;
	if(MQTTSendThread_thread!=NULL)
	EG_thread_delete(&MQTTSendThread_thread);
	 EG_DEBUG("mqtt service stop2...");
	if(MQTTReceiveThread_thread!=NULL)
	EG_thread_delete(&MQTTReceiveThread_thread);
	
	EG_DEBUG("mqtt service stop3...");


	return 0;


}

void EG_mqtt_thread_delete()
{
	mqtt_thread_send_flag = 0;
	mqtt_thread_recv_flag = 0;
	EG_DEBUG("mqtt service delete1...");
	if(MQTTSendThread_thread)
	EG_thread_delete(&MQTTSendThread_thread);
	EG_DEBUG("mqtt service delete2...");
	if(MQTTSendThread_thread)
	EG_thread_delete(&MQTTReceiveThread_thread);
	EG_DEBUG("mqtt service delete3...");
 
}
