#include "eg_packet.h"
EG_CLOUD_CMD_PROCESSCB mqttCommands[MAX_MQTT_COMMANDS_COUNT];
eg_queue_t *cloud2deviceQueue, *device2cloudQueue;
static eg_queue_pool_define(cloud2deviceQueueData, 10 * sizeof(EG_DEV_Packet *));
static eg_queue_pool_define(device2cloudQueueData, 10 * sizeof(EG_MSG_Packet *));

uint8_t EG_msg_queue_create()
{
  uint8_t ret = 0;

  cloud2deviceQueue = (eg_queue_t *)EG_mem_malloc(sizeof(eg_queue_t));
  device2cloudQueue = (eg_queue_t *)EG_mem_malloc(sizeof(eg_queue_t));

  if (EG_queue_create(cloud2deviceQueue, "cloud2deviceQueue", sizeof(EG_DEV_Packet *) , &cloud2deviceQueueData) != EG_SUCCESS) {
    ret = 1;
    EG_E(" EG_queue_create cloud2deviceQueue failed.\r\n");
  }

  if (EG_queue_create(device2cloudQueue, "device2cloudQueue", sizeof(EG_MSG_Packet *), &device2cloudQueueData) != EG_SUCCESS) {
    ret = 1;
    EG_E("EG_queue_create device2cloudQueue failed.\r\n");
  }

  return ret;
}


eg_queue_t *EG_msg_queue_get(EG_CLOUD_MSG_TYPE type)
{
	eg_queue_t* r_queue=NULL;
	
 	switch(type)
 	{
		case EG_CLOUD_MSG_IN:
			r_queue = cloud2deviceQueue ;
			break;
		case EG_CLOUD_MSG_OUT:
			r_queue = device2cloudQueue;
			break;
		default:
			break;
	}

	return r_queue;
}


void EG_cloud_msg_callback_init()
{
	uint8_t i = 0;


	for (i = 0; i < MAX_MQTT_COMMANDS_COUNT; i++)
	{
		mqttCommands[i].commandID = 0x00;
		mqttCommands[i].func_cb = NULL;
	}
}

uint8_t EG_cloud_msg_callback_register(uint32_t commandID, EG_CLOUD_MSG_CB func)
{
	uint8_t i = 0;
	uint8_t ret = 0;

	for (i = 0; i < EG_CLOUDMSG_COMMANDS_COUNT; i++)
	{
		if (mqttCommands[i].commandID == 0x0000)
		{
			break;
		}
	}

	if (i < (EG_CLOUDMSG_COMMANDS_COUNT - 1))
	{
		mqttCommands[i].commandID = commandID;
		mqttCommands[i].func_cb = func;
		ret = 0x01;
	}
	return ret;
}


EG_CLOUD_MSG_CB EG_cloud_msg_callback_get(uint32_t commandID)
{
	uint8_t i = 0;

	for (i = 0; i < EG_CLOUDMSG_COMMANDS_COUNT; i++)
	{
		if (mqttCommands[i].commandID == commandID)
		{
			return mqttCommands[i].func_cb;
		}
	}

	return NULL;
}




static uint8_t eg_makecrc(uint8_t *data, uint8_t length)
{
	uint8_t i = 0;
	int crc32 = 0;
	uint8_t crc = 0;
	for (i = 0; i < length; i++) {
		crc32 += data[i];
	}
	crc = crc32 & 0x000000FF;
	crc = ~crc;
	return crc + 1;
}


 void EG_cloud_msg_fill_dataID(EG_MSG_Packet *pPacket, int dataID)
{
	pPacket->dataID[3] = (uint8_t)((dataID & 0xff000000) >> 24);
	pPacket->dataID[2] = (uint8_t)((dataID & 0x00ff0000) >> 16);
	pPacket->dataID[1] = (uint8_t)((dataID & 0x0000ff00) >> 8);
	pPacket->dataID[0] = (uint8_t)((dataID & 0x000000ff) >> 0);
}

 void EG_cloud_msg_fill_deviceID(EG_MSG_Packet *pPacket)
{

	EG_device_deviceid_get((uint8_t*)pPacket->deviceID);
}


 void  EG_cloud_msg_fill_timestamp(EG_MSG_Packet *pPacket)
{
	uint8_t ret = 0;
	uint8_t pTimeArray[7] = {0};
	if (pPacket) {

		    if(!EG_wlan_get_time(pTimeArray))
			{
			 
			 pPacket->timeStamp[0] = 0;
			 pPacket->timeStamp[1] = pTimeArray[0] & 0x3F;
			 pPacket->timeStamp[2] = pTimeArray[1];
			 pPacket->timeStamp[3] = pTimeArray[2];
			 pPacket->timeStamp[4] = pTimeArray[4];
			 pPacket->timeStamp[5] = pTimeArray[5];
			 pPacket->timeStamp[6] = 0x0F;
			 pPacket->timeStamp[7] = 0x14;
			 
		    }else{

			 pPacket->timeStamp[0] = 0;
			 pPacket->timeStamp[1] = 0 & 0x3F;
			 pPacket->timeStamp[2] = 0;
			 pPacket->timeStamp[3] = 0;
			 pPacket->timeStamp[4] = 0;
			 pPacket->timeStamp[5] = 0;
			 pPacket->timeStamp[6] = 0x0F;
			 pPacket->timeStamp[7] = 0x14;

			}
	}
	
	return ;
}


int EG_request_deviceid(uint8_t *d_mac, uint8_t *d_uuid)
{

	EG_MSG_Packet *pPacket = NULL;
	pPacket = (EG_MSG_Packet *)EG_mem_malloc(sizeof(EG_MSG_Packet));

	if (pPacket) {

		pPacket->head[0] = 0x5A;
		pPacket->head[1] = 0x5A;
		pPacket->version = 0x04;
		pPacket->crypt = 0x11;
		pPacket->dataType[0] = 0x15;
		pPacket->dataType[1] = 0x00;
		
		EG_cloud_msg_fill_dataID(pPacket,0x55);
		EG_cloud_msg_fill_timestamp(pPacket);
		/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
		memset(pPacket->deviceID, 0, 6);
		pPacket->data = (uint8_t *)EG_mem_malloc(12 + 6);
		if(pPacket->data!=NULL)
		{
			memcpy(pPacket->data, d_mac, 12);
			memcpy(pPacket->data + 12, d_uuid, 6);

			int datalen = 44 + (12 + 6);
			pPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);

			memset(pPacket->signature, 0xA5, 16);
			if (eg_list_push(GetWifi2cloudList(), pPacket) != 0x01)
			{
				EG_E("add packet to wifi2cloudlist failed.\r\n");
				EG_msg_packet_free(pPacket);
			}		
		}else
		{
			EG_msg_packet_free(pPacket);
		}
	}
}


EG_MSG_Packet *EG_cloud_msg_create(EG_DEV_Packet *pDevicePacket,unsigned char* rDeviceId)
{

	EG_MSG_Packet *pWifi2CloudPacket = NULL;
	if(!pDevicePacket){
		return pWifi2CloudPacket;
	}
	pWifi2CloudPacket = (EG_MSG_Packet *)EG_mem_malloc(sizeof(EG_MSG_Packet));				
	if (pWifi2CloudPacket) 
	{
		pWifi2CloudPacket->head[0] = 0x5A;
		pWifi2CloudPacket->head[1] = 0x5A;
		pWifi2CloudPacket->version = 0x04;
		pWifi2CloudPacket->crypt = 0x11;	
		if (pDevicePacket->type == EG_DEVICE_MSG_DATA) 
		{
			pWifi2CloudPacket->dataType[0] = 0x20;
			pWifi2CloudPacket->dataType[1] = 0x00;
		}else if(pDevicePacket->type == EG_DEVICE_MSG_STAT)
		{
			pWifi2CloudPacket->dataType[0] = 0x21;
			pWifi2CloudPacket->dataType[1] = 0x00;
		}
		
		EG_cloud_msg_fill_timestamp(pWifi2CloudPacket);
		/*fill device ID, because this field is produce by cloud, so this place should fill zero.*/
		memset(pWifi2CloudPacket->deviceID, 0, 6);
		memcpy(pWifi2CloudPacket->deviceID, rDeviceId, 6);
		//EG_cloud_msg_fill_deviceID(pWifi2CloudPacket);
		
		
		pWifi2CloudPacket->data = (uint8_t *)EG_mem_malloc(pDevicePacket->lenth);
		if (pWifi2CloudPacket->data) {
			memcpy(pWifi2CloudPacket->data, pDevicePacket->data, pDevicePacket->lenth);	
			int datalen = EG_CLOUDMSG_PERFIX_LENTH + (pDevicePacket->lenth);
			pWifi2CloudPacket->dataLen[3] = (uint8_t)((datalen & 0xff000000) >> 24);
			pWifi2CloudPacket->dataLen[2] = (uint8_t)((datalen & 0x00ff0000) >> 16);
			pWifi2CloudPacket->dataLen[1] = (uint8_t)((datalen & 0x0000ff00) >> 8);
			pWifi2CloudPacket->dataLen[0] = (uint8_t)((datalen & 0x000000ff) >> 0);
			
		}else 
		{
			
			EG_mem_free(pWifi2CloudPacket);
			pWifi2CloudPacket = NULL;
		}

					

	}
	

	return pWifi2CloudPacket;
}

EG_DEV_Packet *EG_cloud_msg_extract(EG_MSG_Packet *pWifi2CloudPacket)
{
	EG_DEV_Packet *pDevicePacket=NULL;
	if(!pWifi2CloudPacket){
		return pDevicePacket;
	}
	uint32_t length = (pWifi2CloudPacket->dataLen[0] | (pWifi2CloudPacket->dataLen[1] << 8)| (pWifi2CloudPacket->dataLen[2] << 16)| (pWifi2CloudPacket->dataLen[3] << 24));
	pDevicePacket = (EG_DEV_Packet *)EG_mem_malloc(sizeof(EG_DEV_Packet));	
	
	if (pDevicePacket) {
		pDevicePacket->lenth = 0;
		pDevicePacket->data = NULL;
		memcpy(pDevicePacket->deviceID,pWifi2CloudPacket->deviceID,EG_CLOUDMSG_OF_DEVICEID_LENTH);
		if(length>EG_CLOUDMSG_PERFIX_LENTH)
		{
			pDevicePacket->data=(uint8_t *)EG_mem_malloc(length);
			if(pDevicePacket->data)
			{
				memcpy((uint8_t *)pDevicePacket->data, pWifi2CloudPacket->data, length-EG_CLOUDMSG_PERFIX_LENTH);
				pDevicePacket->lenth = length-EG_CLOUDMSG_PERFIX_LENTH;
			}else
			{
				EG_mem_free(pDevicePacket);
				pDevicePacket = NULL;
			}
		}else
		{
			EG_mem_free(pDevicePacket);
			pDevicePacket = NULL;
		}
		
	}
	return pDevicePacket;
}


void EG_cloud_msg_encrpyt(EG_MSG_Packet *pWifi2CloudPacket)
{

}


void EG_cloud_msg_decrpyt(EG_MSG_Packet *pWifi2CloudPacket)
{

}



void EG_msg_packet_free(EG_MSG_Packet *pPacket)
{
	if (pPacket) {
		if (pPacket->data) {
			EG_mem_free(pPacket->data);
		}
		EG_mem_free(pPacket);
		pPacket = NULL;
	}
}

void EG_device_packet_free(EG_DEV_Packet *pPacket)
{
	if (pPacket) {
		if (pPacket->data){
			EG_mem_free(pPacket->data);
		}
		EG_mem_free(pPacket);
		pPacket = NULL;
	}
}


void EG_cloud_msg_print(EG_MSG_Packet *pPacket, const char *prefix)
{

	if (pPacket) {
		EG_P("%s DT:0x%02X%02X DL:%02X", prefix,  pPacket->dataType[1], pPacket->dataType[0],pPacket->dataLen[0]);
		if (pPacket) {
			EG_P(" Packet:");
			uint8_t i = 0;
			uint8_t *buf = (uint8_t *)pPacket->data;		
			for(i = 0; i < pPacket->dataLen[0] - 44; i++) {
				EG_P("%02X ", buf[i]);
			}
		}
		EG_P("\r\n");
	}
}


void EG_device_msg_print(EG_DEV_Packet *pPacket, const char *prefix)
{

	if (pPacket) {
		EG_P("%s ,lenth:%02X", prefix,  pPacket->lenth);
		if (pPacket) {
			EG_P(" Packet:");
			uint8_t i = 0;
			uint8_t *buf = (uint8_t *)pPacket->data;		
			for(i = 0; i < pPacket->lenth; i++) {
				EG_P("%02X ", buf[i]);
			}
		}
		EG_P("\r\n");
	}
}


