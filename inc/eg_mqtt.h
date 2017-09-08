#ifndef __EG_MQTT_H__
#define __EG_MQTT_H__



#ifdef __cplusplus
extern "C" {
#endif

#include "eg_porting_layer.h"
#include "eg_packet.h"
#include "eg_aes.h"
#include "eg_hardware.h"
#include "eg_event.h"
#include "MQTTClient.h"

#define EJ_TEST_SERVER 1

#define SERVER_ADDR_MASTER	"120.24.170.30"
#define SERVER_ADDR_TEST	"120.77.76.24"

#define SERVER_TOPIC_MASTER "$USR/manager"
#define SERVER_TOPIC_TEST   "$USR/manageriotdevice15"

#define MASTER_SERVER_PORT	1883
#define CLOUD_DOMAIN_ADDR   "ej-test.com"

#define  STR_WIFI_MODULE_BRAND  "CCD" 
#define  STR_WIFI_MODULE_MODE   "MT7687"
#define  STR_WIFI_MODULE_FOTATOPIC  "100/1000/1003"


typedef enum {
	MQTT_CONNECTED_SUCCESS,
	MQTT_CREATE_SOCKET_ERROR,
	MQTT_CONNECTED_ERROR,
	MQTT_CONNECT_ERROR_FINISHED,
};

typedef enum {

	INIT_MQTT_SUCCESS,
	INIT_MQTT_CONNECTION_ERROR,
	INIT_MQTT_OS_THREAD_CREATE_ERROR,
};

typedef enum {

  MECHANISM_3S = 0,
  MECHANISM_10S,
  MECHANISM_25S,
  
}CONNECT_MECHANISM;

int EG_service_init(const char* uuid,const char* macaddr);
int EG_mqtt_start();
int EG_mqtt_stop();

int EG_device_check_deviceid();
void EG_mqtt_thread_delete();

extern _g_pdevinfo;
extern _g_pam;


#ifdef __cplusplus
}
#endif

#endif // __EG_MQTT_H__

