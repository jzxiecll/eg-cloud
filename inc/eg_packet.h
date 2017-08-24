#ifndef __EG_PACKET_H__
#define __EG_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "eg_porting_layer.h"



#if 0
#define EG_SERVER_PAYLOAD_LENTH 1024
typedef PACK_START struct {
	uint32_t lenth;
	uint8_t  payload[EG_SERVER_PAYLOAD_LENTH];
}PACK_END EG_content_packet;
#else
#define EG_SERVER_PAYLOAD_LENTH 1024
typedef struct {
	uint32_t lenth;
	uint8_t  payload[EG_SERVER_PAYLOAD_LENTH];
}EG_content_packet;

#endif

typedef enum{
	EG_DEVICE_MSG_RDID = 0x15,
	EG_DEVICE_MSG_DATA = 0x20,
	EG_DEVICE_MSG_STAT = 0x21,	
}EG_DEVICE_MSG_TYPE;


typedef enum{
	EG_CLOUD_MSG_IN = 0,
	EG_CLOUD_MSG_OUT	
}EG_CLOUD_MSG_TYPE;


#define EG_CLOUDMSG_COMMANDS_COUNT 20
#define EG_CLOUDMSG_PERFIX_LENTH  44

#define EG_CLOUDMSG_OF_HEAD_LENTH  			2
#define EG_CLOUDMSG_OF_DATALEN_LENTH  		4
#define EG_CLOUDMSG_OF_DATATYPE_LENTH 		2
#define EG_CLOUDMSG_OF_DATAID_LENTH 		4
#define EG_CLOUDMSG_OF_DEVICEID_LENTH  		6
#define EG_CLOUDMSG_OF_SIGNATURE_LENTH  	16
#define EG_CLOUDMSG_OF_TIMESTAMP_LENTH  	8

typedef PACK_START struct {

	uint8_t head[EG_CLOUDMSG_OF_HEAD_LENTH];
	uint8_t version;
	uint8_t crypt;
	uint8_t dataLen[EG_CLOUDMSG_OF_DATALEN_LENTH];
	uint8_t dataType[EG_CLOUDMSG_OF_DATATYPE_LENTH];
	uint8_t dataID[EG_CLOUDMSG_OF_DATAID_LENTH];
	uint8_t timeStamp[EG_CLOUDMSG_OF_TIMESTAMP_LENTH];
	uint8_t deviceID[EG_CLOUDMSG_OF_DEVICEID_LENTH];
	uint8_t *data;
	uint8_t signature[EG_CLOUDMSG_OF_SIGNATURE_LENTH];

}PACK_END EG_mqtt_packet;
typedef PACK_START struct {
	
	EG_DEVICE_MSG_TYPE type;
	uint8_t deviceID[EG_CLOUDMSG_OF_DEVICEID_LENTH];
	uint32_t lenth;
	uint8_t *data;
	
}PACK_END EG_device_packet;

#define  EG_MSG_Packet EG_mqtt_packet
#define  EG_DEV_Packet EG_device_packet

typedef uint8_t(*EG_CLOUD_MSG_CB)(EG_MSG_Packet *);
typedef struct {
	uint32_t commandID;
	uint8_t classID;
	uint8_t length;
	EG_CLOUD_MSG_CB func_cb;
}EG_CLOUD_CMD_PROCESSCB;

#define MAX_MQTT_COMMANDS_COUNT 20


EG_MSG_Packet *EG_cloud_msg_create(EG_DEV_Packet *pDevicePacket,unsigned char* rDeviceId);
EG_DEV_Packet *EG_cloud_msg_extract(EG_MSG_Packet *pWifi2CloudPacket);

uint8_t EG_cloud_msg_callback_register(uint32_t commandID, EG_CLOUD_MSG_CB func);
EG_CLOUD_MSG_CB EG_cloud_msg_callback_get(uint32_t commandID);

void EG_device_packet_free(EG_DEV_Packet *pPacket);
void EG_msg_packet_free(EG_MSG_Packet *pPacket);
void EG_device_msg_print(EG_DEV_Packet *pPacket, const char *prefix);
void EG_cloud_msg_print(EG_MSG_Packet *pPacket, const char *prefix);

eg_queue_t *EG_msg_queue_get(EG_CLOUD_MSG_TYPE type);
uint8_t     EG_msg_queue_create();
int EG_request_deviceid(uint8_t *d_mac, uint8_t *d_uuid);



#ifdef __cplusplus
}
#endif

#endif // __EG_PACKET_H__

