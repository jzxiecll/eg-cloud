#include "eg_hardware.h"

WIFIManage_t _g_am, *_g_pam = &_g_am;
DeviceInfo_t _g_devinfo, *_g_pdevinfo = &_g_devinfo;

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int  EG_device_uuid_get(char *buf, unsigned int buf_sz)
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_uuid", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}
int  EG_device_uuid_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_uuid", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
	if(status!=NVDM_STATUS_OK)
	{
		EG_E("nvdm_write_data_item uuid failed");
	}
   	
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int  EG_device_deviceid_get(char *buf, unsigned int buf_sz)
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_uuid", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}
int  EG_device_deviceid_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_deviceid", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int  EG_device_macaddr_get(char *buf, unsigned int buf_sz )
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_macaddr", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_macaddr_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_macaddr", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}



int  EG_device_configmode_get(char *buf, unsigned int buf_sz )
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_configmode", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_configmode_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_configmode", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}
int  EG_device_serveraddr_get(char *buf, unsigned int buf_sz )
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_serveraddr", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_serveraddr_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_serveraddr", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}
int  EG_device_serverport_get(char *buf, unsigned int buf_sz )
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_serverport", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_serverport_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_serverport", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int  EG_device_serverdomain_get(char *buf, unsigned int buf_sz )
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_serverdomain", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_serverdomain_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_serverdomain", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_version_get(char *buf, unsigned int buf_sz )
{
	nvdm_status_t status;        
    status = nvdm_read_data_item(
        "User", 
        "eg_version", 
        (uint8_t *)buf, 
        (uint32_t *)&buf_sz);
    
    if('\0' == *buf)
        return 1;
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


int  EG_device_verison_set(const char *json_in)
{
	nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "eg_version", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (const uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}


static int  EG_device_psm_init()
{
   nvdm_init();
   EG_device_register_attr_cb(EG_ATTR_UUID,  EG_device_uuid_get, EG_device_uuid_set);
   EG_device_register_attr_cb(EG_ATTR_DEVICEID,  EG_device_deviceid_get, EG_device_deviceid_set);
   EG_device_register_attr_cb(EG_ATTR_MACADDR,  EG_device_macaddr_get, EG_device_macaddr_set);
   EG_device_register_attr_cb(EG_ATTR_CONFIGMODE,  EG_device_configmode_get, EG_device_configmode_set);
   EG_device_register_attr_cb(EG_ATTR_SERVERADDR,  EG_device_serveraddr_get, EG_device_serveraddr_set);
   EG_device_register_attr_cb(EG_ATTR_SERVERPORT,  EG_device_serverport_get, EG_device_serverport_set);
   EG_device_register_attr_cb(EG_ATTR_SERVERDOMAIN,  EG_device_serverdomain_get, EG_device_serverdomain_set);
   //_device_register_attr_cb(EG_ATTR_SERVERPORT, EG_device_version_get, EG_device_version_set);	 
}



int   EG_wlan_get_connection_state(unsigned char *state)
{
	if(state == NULL)
			return -EG_E_INVAL;
	unsigned char cur = 5 ;
	if(wifi_connection_get_link_status(&cur)<0)
	{
		return -EG_FAIL;
	}
	
	switch(cur){

			case  0://disconnected 
				*state = ROUTER_NOT_CONNECTED;
				break;
			case  1://connected
				*state = ROUTER_CONNECTED;
				break;
			case  2:
			case  3:
			default: 
				*state  = cur ;
				break;
	}


	return EG_SUCCESS;

}




static  void  eg_wifi_reboot()
{

	hal_cache_disable();
	hal_cache_deinit();
	hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);

}
static  void  eg_household_reboot()
{

}

int  EG_device_reboot(EG_DEVICE_OF_TYPE type)
{

	switch(type)
	{
		case EG_DEVICE_OF_WIFI:
			eg_wifi_reboot();
			break;
		case EG_DEVICE_OF_HOUSEHOLD:
			eg_household_reboot();
			break;
		default:
			break;
	}
	return 0;
}



int EG_device_time_set(uint8_t *pTimeArray)
{
	int ret = 0 ;
	struct eg_tm h_tm;
	h_tm.tm_sec  = pTimeArray[0];
	h_tm.tm_min  = pTimeArray[1];
	h_tm.tm_hour = pTimeArray[2];
	h_tm.tm_wday = pTimeArray[3];
	h_tm.tm_mday = pTimeArray[4];
	h_tm.tm_mon  = pTimeArray[5];
	h_tm.tm_year = pTimeArray[6];
	if (EG_time_set(&h_tm) == -1) {
		EG_E(" set time error.\r\n");
		ret = 0x01;
	}
	return ret ;
}


int EG_device_time_get(uint8_t *pTimeArray)
{
	int ret = 0 ;
	struct eg_tm h_tm;
	if (EG_time_get(&h_tm) == -1) {
		EG_E(" get time error.\r\n");
		ret = 0x01;
	}
	pTimeArray[0] = h_tm.tm_sec;
	pTimeArray[1] = h_tm.tm_min;
	pTimeArray[2] = h_tm.tm_hour;
	pTimeArray[3] = h_tm.tm_wday;
	pTimeArray[4] = h_tm.tm_mday;
	pTimeArray[5] = h_tm.tm_mon;
	pTimeArray[6] = h_tm.tm_year;

	return ret ;
}


int EG_str2hex(char s[],char bits[])
{
    int i,n = 0;
    for(i = 0; s[i]; i += 2) {
        if(s[i] >= 'A' && s[i] <= 'F')
            bits[n] = s[i] - 'A' + 10;
        else bits[n] = s[i] - '0';
        if(s[i + 1] >= 'A' && s[i + 1] <= 'F')
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
        ++n;
    }
    return n;
}


int EG_device_register_attr_cb(const char *name, E_EG_DEV_ATTR_GET_CB attr_get_cb, E_EG_DEV_ATTR_SET_CB attr_set_cb)
{
    if(NULL == name){
        return -1;
    }
    int ret = -1;
    EG_D("regster %s", name);
    if(!strcmp(name, EG_ATTR_MACADDR)){
       _g_pam->macaddr.get = attr_get_cb; 
       _g_pam->macaddr.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_UUID)){
       _g_pam->uuid.get = attr_get_cb; 
       _g_pam->uuid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_DEVICEID)){
       _g_pam->deviceid.get = attr_get_cb; 
       _g_pam->deviceid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_CONFIGMODE)){
       _g_pam->configmode.get = attr_get_cb; 
       _g_pam->configmode.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_SERVERADDR)){
       _g_pam->serveraddr.get = attr_get_cb; 
       _g_pam->serveraddr.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_SERVERPORT)){
       _g_pam->serverport.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_SERVERDOMAIN)){
       _g_pam->serverport.get = attr_get_cb; 
       ret = 0;
    }else if(!strcmp(name, EG_ATTR_SERVERINFO)){
       _g_pam->server_info.get = attr_get_cb; 
       _g_pam->server_info.set = attr_set_cb; 
       ret = 0;
    } 

    EG_D("regster %s:ret:%d", name, ret);
    return ret;
}




static int  EG_device_get_info(DeviceInfo_t *p_devParma)
{

    /**
     *FIXME:must to do
     */
    int ret = -1;
    char buff[256];
	if(_g_pam->uuid.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->uuid.get(buff, sizeof(buff))){
            EG_D("-->uuid:%s\n", buff);
            memcpy(p_devParma->uuid, buff,strlen(buff));
        }else{
            EG_E("get uuid error");
        }
    }

	if(_g_pam->deviceid.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->deviceid.get(buff, sizeof(buff))){
            EG_D("-->deviceid:%s\n", buff);
			char r_did[6]={0};
			EG_str2hex(buff,r_did);
			memcpy(p_devParma->deviceid, r_did,sizeof(r_did));
        }else{
            EG_E("get deviceid error");
        }
    }

	if(_g_pam->macaddr.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->macaddr.get(buff, sizeof(buff))){
            EG_D("-->macaddr:%s\n", buff);
            memcpy(p_devParma->macaddr, buff,strlen(buff));
        }else{
            EG_E("get macaddr error");
        }
    }

	if(_g_pam->configmode.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->configmode.get(buff, sizeof(buff))){
            EG_D("-->configmode:%s\n", buff);
            memcpy(p_devParma->configmode, buff,strlen(buff));
        }else{
            EG_E("get configmode error");
        }
    }

	if(_g_pam->serveraddr.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->serveraddr.get(buff, sizeof(buff))){
            EG_D("-->serveraddr:%s\n", buff);
            memcpy(p_devParma->serveraddr, buff,strlen(buff));
        }else{
            EG_E("get serveraddr error");
        }
    }

	if(_g_pam->serverport.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->serverport.get(buff, sizeof(buff))){
            EG_D("-->serverport:%s\n", buff);
            memcpy(p_devParma->serverport, buff,strlen(buff));
        }else{
            EG_E("get serverport error");
        }
    }

	if(_g_pam->serverdomain.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->serverdomain.get(buff, sizeof(buff))){
            EG_D("-->serverdomain:%s\n", buff);
            memcpy(p_devParma->serverdomain, buff,strlen(buff));
        }else{
            EG_E("get serverdomain error");
        }
    }


}


static void EG_wait_network_up(void)
{

	uint8_t link_status = 0;
 	struct netif *sta_if = NULL;

 	EG_I("%s : start\n", __FUNCTION__);

	 //check wifi link
	 while(!link_status){
		 wifi_connection_get_link_status(&link_status);
		 if(!link_status){
			 /*wait until wifi connect*/
			 // printf("Waiting for STA link up...\n");
			 EG_thread_sleep(1000);
		 }
	 }

	 //check netif up
	 while(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
		 sta_if = netif_find("st2");
		 if(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
			 /*wait until netif up*/
			 // printf("Waiting for netif up...\n");
			 EG_thread_sleep(1000);
		 }
	 }

	 while (!(sta_if->dhcp && (sta_if->dhcp->state == DHCP_STATE_BOUND || sta_if->dhcp->state == DHCP_STATE_RENEWING))) {
		 /*wait until got ip address*/
		 // printf("Waiting for got ip address...\n");
		 EG_thread_sleep(1000);
	 }

	 EG_I("%s : end\n", __FUNCTION__);


}

uint8_t* eg_get_dev_info_macaddr()
{
	return  (uint8_t*)(_g_pdevinfo->macaddr);

}
uint8_t* eg_get_dev_info_uuid()
{

return  (uint8_t*)(_g_pdevinfo->uuid);

}
uint8_t* eg_get_dev_info_deviceid()
{

	return  (uint8_t*)(_g_pdevinfo->deviceid);

}
uint8_t* eg_get_dev_info_configmode()
{
	return	(uint8_t*)(_g_pdevinfo->configmode);

}
uint8_t* eg_get_dev_info_serveraddr()
{
	return	(uint8_t*)(_g_pdevinfo->serveraddr);

}
uint8_t* eg_get_dev_info_serverport()
{
	return	(uint8_t*)(_g_pdevinfo->serverport);

}
uint8_t* eg_get_dev_info_serverdomain()
{
	return	(uint8_t*)(_g_pdevinfo->serverdomain);

}

void eg_set_dev_info_macaddr(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->macaddr),buff,len);
}
void eg_set_dev_info_uuid(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->uuid),buff,len);
}
void eg_set_dev_info_deviceid(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->deviceid),buff,len);
}
void eg_set_dev_info_configmode(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->configmode),buff,len);
}
void eg_set_dev_info_serveraddr(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->serveraddr),buff,len);
}
void eg_set_dev_info_serverport(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->serverport),buff,len);
}
void eg_set_dev_info_serverdomain(uint8_t* buff ,uint8_t len)
{
	memcpy((char*)(_g_pdevinfo->serverdomain),buff,len);
}


WIFIMODULE_CONFIG_MODE eg_get_dev_info_wifistatus_configStatus()
{
	return _g_pdevinfo->wifiStatus.configStatus;
}
uint8_t eg_get_dev_info_wifistatus_workMode()
{
	return _g_pdevinfo->wifiStatus.configStatus;
}
WIFIMODULE_ROUTER_STATUS eg_get_dev_info_wifistatus_routerStatus()
{
	return _g_pdevinfo->wifiStatus.routerStatus;
}
WIFIMODULE_CLOUD_SERVICE_STATUS eg_get_dev_info_wifistatus_cloudServiceStatus()
{
	return _g_pdevinfo->wifiStatus.cloudServiceStatus;
}

WIFIMODULE_DEVICEID_RSTAT eg_get_dev_info_wifistatus_deviceIDRequestStatus()
{

	return _g_pdevinfo->wifiStatus.deviceIDRequestStatus;
}

void eg_set_dev_info_wifistatus_configStatus(WIFIMODULE_CONFIG_MODE stat)
{

	_g_pdevinfo->wifiStatus.configStatus = stat;
}
void eg_set_dev_info_wifistatus_workMode(uint8_t stat)
{

	_g_pdevinfo->wifiStatus.workMode= stat;
}
void eg_set_dev_info_wifistatus_routerStatus(WIFIMODULE_ROUTER_STATUS  stat)
{

	_g_pdevinfo->wifiStatus.routerStatus= stat;
}
void eg_set_dev_info_wifistatus_cloudServiceStatus(WIFIMODULE_CLOUD_SERVICE_STATUS stat)
{

	_g_pdevinfo->wifiStatus.cloudServiceStatus= stat;
}

void eg_set_dev_info_wifistatus_deviceIDRequestStatus(WIFIMODULE_DEVICEID_RSTAT stat)
{
	_g_pdevinfo->wifiStatus.deviceIDRequestStatus= stat;
}
void EG_device_init(const char* uuid,const char* macaddr)
{

	EG_device_psm_init();
	EG_device_get_info(&_g_devinfo);
	//1.init  hardware
	memcpy(_g_pdevinfo->uuid, uuid, strlen(uuid));
	memcpy(_g_pdevinfo->macaddr, macaddr, strlen(macaddr));

}
