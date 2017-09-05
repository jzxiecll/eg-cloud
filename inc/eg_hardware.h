#ifndef __EG_HARDWARE_H__
#define __EG_HARDWARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "eg_porting_layer.h"
#include "eg_time.h"
#include "inet.h"
#include "dhcp.h"
#include "stats.h"
#include "netif.h"

#include "wifi_api.h"

typedef enum{

	EG_DEVICE_OF_WIFI = 0,
	EG_DEVICE_OF_HOUSEHOLD	
}EG_DEVICE_OF_TYPE;

typedef enum {
	ROUTER_NOT_CONNECTED = 0x00,
	ROUTER_CONNECTED,
	ROUTER_CONNECTTING,
	ROUTER_AUTH_PASSWORD_ERROR,
	ROUTER_NOT_FIND,
	ROUTER_CANNOT_GET_IP,
	ROUTER_NOT_STABLE,
	WIFIMODULE_ERROR,

}WIFIMODULE_ROUTER_STATUS;


typedef enum {
	CLOUD_MQTT_NOT_CONNECTED= 0x00,
	CLOUD_MQTT_CONNECTTING,
	CLOUD_MQTT_CONNECTED,
	CLOUD_MQTT_CONNECT_NOT_STABLE,
	CLOUD_MQTT_ADDRESS_PARSER_ERROR,
	CLOUD_MQTT_CONNECT_REFUSE,
	CLOUD_MQTT_MAINTAINNING,
	CLOUD_MQTT_FAULT,

}WIFIMODULE_CLOUD_SERVICE_STATUS;

typedef enum {

	WIFIMODULE_EASYCONNECT_MODE = 0x00,
	WIFIMODULE_SOFTAP_MODE,
	WIFIMODULE_AIRKISS_MODE,
	WIFIMODULE_JOYLINK_MODE,
	WIFIMODULE_ALI_MODE,
	WIFIMODULE_CLIENT_MODE,
	WIFIMODULE_UNKNOW_MODE,

}WIFIMODULE_CONFIG_MODE;


typedef enum {

	WIFIMODULE_DEVICEID_FAILED = 0x00,
	WIFIMODULE_DEVICEID_SUCCESS,

}WIFIMODULE_DEVICEID_RSTAT;

typedef enum {

	WIFIMODULE_UDPBROADCAST_NOT_RUNNING = 0x00,
	WIFIMODULE_UDPBROADCAST_RUNNING,
	WIFIMODULE_UDPBROADCAST_RUNNED,
}WIFIMODULE_UDPBROADCAST_STATUS;


typedef struct {

	uint8_t configStatus;
	uint8_t deviceIDRequestStatus;
	uint8_t deviceConnectStatus;
	uint8_t workMode;
	uint8_t signalIntensity;
	uint8_t routerStatus;
	uint8_t cloudServiceStatus;
	uint8_t connectedStatus;
	uint8_t tcpConnectedCount;
	uint8_t ipaddr[4];
	uint8_t isHomeAPConfig;
	uint8_t isLanModuleRunning;
	uint8_t isUdpBroadcastRunning;

}WifiModuleStatus;


#define EG_ATTR_MACADDR  				"macaddr"
#define EG_ATTR_UUID      				"uuid"
#define EG_ATTR_DEVICEID      	        "deviceid"
#define EG_ATTR_CONFIGMODE      		"configmode"
#define EG_ATTR_SERVERADDR   			"serveraddr"
#define EG_ATTR_SERVERPORT  			"serverport"
#define EG_ATTR_SERVERDOMAIN            "serverdomain"
#define EG_ATTR_SERVERINFO              "serverinfo"
#define EG_ATTR_VERSION                 "version"


typedef enum {
    TGW_ATTR_J = 1, 
    TGW_ATTR_A    
}E_EG_DEV_ATTR_TYPE;


typedef int (*E_EG_DEV_ATTR_GET_CB)(char *buf, unsigned int buf_sz);
typedef int (*E_EG_DEV_ATTR_SET_CB)(const char *json_in);


typedef enum _ret_code{
    E_RET_ERROR_PKG_SAME = -1001,
    E_RET_ERROR_PKG_NUM_BREAK_MAX = -1002,
    E_RET_ERROR_PKG_BREAK_ARRAY = -1003,
    E_RET_ERROR_PARAM_INVAID = -3,
    E_RET_ERROR_NO_EXIST = -2,
    E_RET_ERROR = -1,
    E_RET_OK = 0,
    E_RET_ALL_DATA_HIT = 1001,
    E_RET_DATA_COMING = 1002
}E_JLRetCode_t;

typedef struct __attr_{
    char name[128];
    E_EG_DEV_ATTR_GET_CB get;
    E_EG_DEV_ATTR_SET_CB set;
}Attr_t;

typedef struct _attr_manage_{
    Attr_t uuid;
    Attr_t deviceid;
    Attr_t configmode;
    Attr_t serveraddr;
    Attr_t serverport;
    Attr_t serverdomain;
    Attr_t macaddr;
	Attr_t server_info;	
	Attr_t version;	
}WIFIManage_t;

#define EG_MAX_UUID_LEN  			8
#define EG_MAX_MAC_LEN  			20
#define EG_MAX_SERVERADDR_LEN  		20
#define EG_MAX_SERVERPORT_LEN  		6
#define EG_MAX_SERVERDOMAIN_LEN  	20
#define EG_MAX_CONFIGMODE_LEN  		20
#define EG_MAX_DEVICEID_LEN  		12


typedef struct {		
	char macaddr[EG_MAX_MAC_LEN];
	char uuid[EG_MAX_UUID_LEN];
	char deviceid[EG_MAX_DEVICEID_LEN];
	char serveraddr[EG_MAX_SERVERADDR_LEN];
	char serverport[EG_MAX_SERVERPORT_LEN];
	char serverdomain[EG_MAX_SERVERDOMAIN_LEN];
	char configmode[EG_MAX_CONFIGMODE_LEN];
	WifiModuleStatus wifiStatus;
}DeviceInfo_t;

int EG_wlan_get_ip_address(uint8_t* sIpaddr);

int EG_device_uuid_set(const char *json_in);
int EG_device_uuid_get(char *buf, unsigned int buf_sz);

int EG_device_macaddr_set(const char *json_in);
int EG_device_macaddr_get(char *buf, unsigned int buf_sz);

int EG_device_deviceid_set(const char *json_in);
int EG_device_deviceid_get(char *buf, unsigned int buf_sz);

int EG_device_configmode_set(const char *json_in);
int EG_device_configmode_get(char *buf, unsigned int buf_sz);

int EG_device_serveraddr_set(const char *json_in);
int EG_device_serveraddr_get(char *buf, unsigned int buf_sz);

int EG_device_serverport_set(const char *json_in);
int EG_device_serverport_get(char *buf, unsigned int buf_sz);

int EG_device_serverdomain_set(const char *json_in);
int EG_device_serverdomain_get(char *buf, unsigned int buf_sz);

int EG_device_version_set(const char *json_in);
int EG_device_version_get(char *buf, unsigned int buf_sz);

int EG_str2hex(char s[],char bits[]);


int EG_device_reboot(EG_DEVICE_OF_TYPE type);
int EG_device_time_set(uint8_t *pTimeArray);
int EG_device_time_get(uint8_t *pTimeArray);
int EG_device_register_attr_cb(const char *name, E_EG_DEV_ATTR_GET_CB attr_get_cb, E_EG_DEV_ATTR_SET_CB attr_set_cb);

uint8_t* eg_get_dev_info_macaddr();
uint8_t* eg_get_dev_info_uuid();
uint8_t* eg_get_dev_info_deviceid();
uint8_t* eg_get_dev_info_configmode();
uint8_t* eg_get_dev_info_serveraddr();
uint8_t* eg_get_dev_info_serverport();
uint8_t* eg_get_dev_info_serverdomain();

void eg_set_dev_info_macaddr(uint8_t* buff ,uint8_t len);
void eg_set_dev_info_uuid(uint8_t* buff ,uint8_t len);
void eg_set_dev_info_deviceid(uint8_t* buff ,uint8_t len);
void eg_set_dev_info_configmode(uint8_t* buff ,uint8_t len);
void eg_set_dev_info_serveraddr(uint8_t* buff ,uint8_t len);
void eg_set_dev_info_serverport(uint8_t* buff ,uint8_t len);
void eg_set_dev_info_serverdomain(uint8_t* buff ,uint8_t len);




WIFIMODULE_CONFIG_MODE eg_get_dev_info_wifistatus_configStatus();
uint8_t eg_get_dev_info_wifistatus_workMode();
WIFIMODULE_ROUTER_STATUS eg_get_dev_info_wifistatus_routerStatus();
WIFIMODULE_CLOUD_SERVICE_STATUS eg_get_dev_info_wifistatus_cloudServiceStatus();
WIFIMODULE_DEVICEID_RSTAT eg_get_dev_info_wifistatus_deviceIDRequestStatus();
WIFIMODULE_UDPBROADCAST_STATUS eg_get_dev_info_wifistatus_udpbroadcastStatus();






void eg_set_dev_info_wifistatus_configStatus(WIFIMODULE_CONFIG_MODE stat);
void eg_set_dev_info_wifistatus_workMode(uint8_t stat);
void eg_set_dev_info_wifistatus_routerStatus(WIFIMODULE_ROUTER_STATUS  stat);
void eg_set_dev_info_wifistatus_cloudServiceStatus(WIFIMODULE_CLOUD_SERVICE_STATUS stat);
void eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_RSTAT stat);
void eg_set_dev_info_wifistatus_udpbroadcastStatus(WIFIMODULE_UDPBROADCAST_STATUS stat);



void EG_device_init(const char* uuid,const char* macaddr);



#ifdef __cplusplus
}
#endif

#endif // __EG_HARDWARE_H__

