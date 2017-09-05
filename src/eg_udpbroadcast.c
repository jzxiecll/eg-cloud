#include "eg_udpbroadcast.h"
#include "eg_hardware.h"
static eg_thread_t UDPBroadcastThread_thread;
static eg_thread_stack_define(UDPBroadcastThread_stack, 2048);
static int rxsock = -1;
static int txsock = -1;
static struct sockaddr_in from;
static struct sockaddr_in addrto;

EG_UDPBROADCAST_PACKET_T EG_prase_udp_packet_type(char *p_packet)
{
	EG_UDPBROADCAST_PACKET_T ret = EG_UDPBROADCAST_PACKET_TUNKOWN;
	
	if(strcmp(p_packet,"T_call") == 0) 
	{
		ret = EG_UDPBROADCAST_PACKET_TCALL;
	}else if(strcmp(p_packet,"T_gatewayInfo") == 0)
	{
		ret = EG_UDPBROADCAST_PACKET_TGATEWAYINFO;
	}else if(strcmp(p_packet,"T_getdid") == 0)
	{
		ret = EG_UDPBROADCAST_PACKET_TGATEWAYINFO;
	}
	return ret;
}
static void split( char **arr, char *str, const char *del)//字符分割函数的简单定义和实现
{
	char *s =NULL; 
	static char *psplitTmp = NULL;
	s=strtok_r(str,del,&psplitTmp);
	while(s!=NULL)
	{
		*arr++ = s;
		s = strtok_r(NULL,del,&psplitTmp);
	}
}

static int strsplinum(char *str, const char*del)   //判断总共有多少个分隔符，目的是在main函数中构造相应的arr指针数组
{
	 char *first = NULL;
	 char *second = NULL;
	 int num = 0;
	 first = strstr(str,del);
	 while(first != NULL)
	 {
	 second = first+1;
	 num++;
	 first = strstr(second,del);
	 }
	 return num;
}

static void GetBroadcastResponsePacket(PakcetWifiModuleBroadcastResponse* discoverResponse)
{

	
	memset(discoverResponse,0,sizeof(PakcetWifiModuleBroadcastResponse));
	//uuid
	memcpy(discoverResponse->uuid,eg_get_dev_info_uuid(),6);
	if(!strlen(discoverResponse->uuid))
	{
		memcpy(discoverResponse->uuid,"NULL",strlen("NULL"));
	}
	//did

	uint8_t *did=eg_get_dev_info_deviceid();
	if(did[0]|did[1]|did[2]|did[3]|did[4]|did[5])
	{
		snprintf(discoverResponse->did,13,"%02X%02X%02X%02X%02X%02X",did[0],did[1],did[2],did[3],did[4],did[5]);

	}else{
		sprintf(discoverResponse->did,"NULL");
	}	

	/*
	if(eg_get_dev_info_wifistatus_deviceIDRequestStatus()==WIFIMODULE_DEVICEID_SUCCESS)
	{
		
		memcpy(discoverResponse->did,eg_get_dev_info_deviceid(),12);
	}else
	{
		memcpy(discoverResponse->did,"NULL",strlen("NULL"));
	}
	*/
	
	//mac
	memcpy(discoverResponse->mac,eg_get_dev_info_macaddr(),12);

	//ipaddr
	//EJ_Wlan_get_ip_address(discoverResponse->ipaddr);
	//memcpy(discoverResponse->ipaddr,eg_get_dev_info_macaddr(),12);
	EG_wlan_get_ip_address(discoverResponse->ipaddr);
}


void EG_pub_udpbroadcast_msg(void *payload, int dataLen)
{
	EG_DEBUG("EG_pub_udpbroadcast_msg:%s,datalen=%d",(char*)payload,dataLen);
	if(sendto(txsock, payload, dataLen, 0, (struct sockaddr*)&addrto, sizeof(addrto))<0)  
	{  
		EG_LOG_ERROR("sendto failed.\r\n");  
	} 
}


static void UDPBroadcastThread(void *arg)
{

	fd_set rfd;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	ip_addr_t addr;
	char *ip = NULL;
	char *p;
	char *delim = ":";
	 
	char receiveData[256] = {0};  
	char sendData[512] = {0};  
	for (;;) {

		FD_ZERO(&rfd);
		FD_SET(rxsock, &rfd); 
		int nRet = select(rxsock + 1, &rfd, NULL, NULL, &timeout);
		if (nRet < 0){
			EG_LOG_ERROR("Select ERR: %s!\r\n");
			continue;
		}else if (nRet == 0){
		   /*TimeOut*/
			continue;
		}
		if (FD_ISSET(rxsock, &rfd))
		{
			int nlen=sizeof(from); 
			memset(receiveData,0,sizeof(receiveData));
			memset(sendData,0,sizeof(receiveData));
			if(recvfrom(rxsock, receiveData, 256, 0, (struct sockaddr*)&from, (socklen_t*)&nlen) > 0)  
			{
	
					int numTest=1;
					numTest = strsplinum(receiveData,delim); 
					char *arrRecvdata[numTest]; 
					split(arrRecvdata,receiveData,delim);
				
					uint8_t  dataPart[128]={0};
					PakcetWifiModuleBroadcastResponse discoverResponse;
					GetBroadcastResponsePacket(&discoverResponse);
					
					switch(EG_prase_udp_packet_type(arrRecvdata[2]))
					{
						case EG_UDPBROADCAST_PACKET_TCALL:
	
							sprintf(dataPart,"%s,%s,%s,%s,",discoverResponse.uuid,discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr); 
	
							sprintf(sendData,"%s,%s,%s,%s,%s,%d,%d,","Response",arrRecvdata[1],arrRecvdata[2],
											discoverResponse.did,arrRecvdata[4],0,strlen(dataPart));
	
							strcat (sendData,dataPart); 
							EG_pub_udpbroadcast_msg(sendData, strlen(sendData));	
							break;
						case EG_UDPBROADCAST_PACKET_TGATEWAYINFO:
							break;				
						case EG_UDPBROADCAST_PACKET_TGETDID:
							if(strlen(discoverResponse.uuid)!=6||discoverResponse.did=="")
							{
									eg_set_dev_info_uuid(arrRecvdata[7],6);
									//EG_device_uuid_set(arrRecvdata[7]);
									memcpy(discoverResponse.did,eg_get_dev_info_uuid(),6);

							}									
							sprintf(dataPart,"%s,%s,%s,%s,",arrRecvdata[7],discoverResponse.did,discoverResponse.mac,discoverResponse.ipaddr);		
							sprintf(sendData,"%s,%s,%s,%s,%s,%d,%d,","Response",arrRecvdata[1],arrRecvdata[2],
											discoverResponse.did,arrRecvdata[4],0,strlen(dataPart));

							strcat (sendData,dataPart); 
							EG_pub_udpbroadcast_msg(sendData, strlen(sendData));
							break;			
						default:
							break;
					}
			}
				
				
		}
	
		
		EG_thread_sleep(EG_msec_to_ticks(500));
	}
			

}

int EG_udpbroadcast_start()
{
	int ret = INIT_UDPBROADCAST_SUCCESS;
	memset(&addrto, 0,sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	addrto.sin_port = htons(9887);

	memset(&from, 0,sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(IPADDR_ANY);
	from.sin_port = htons(9888);
	
	if ((rxsock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		EG_LOG_ERROR("rxsocket create failed.\r\n");
		return INIT_UDPBROADCAST_SOCKET_CREATE_ERROR;
	}

	if ((txsock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		EG_LOG_ERROR("txsocket create failed.");	
		close(rxsock);
		rxsock = -1;
		return INIT_UDPBROADCAST_SOCKET_CREATE_ERROR;
	}     

	const int opt = 1;
	int nb = 0;
	int timeout = 500;
	if(nb == -1)
	{
		EG_LOG_ERROR("rxsocket set broadcast.\r\n");
		return INIT_UDPBROADCAST_SOCKET_SET_BROADCAST_ERROR;
	}
	int on = 1;
	nb = setsockopt(rxsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	nb = setsockopt(txsock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		EG_LOG_ERROR("txsocket set broadcast.");
		return INIT_UDPBROADCAST_SOCKET_SET_BROADCAST_ERROR;
	}

	if(bind(rxsock,(struct sockaddr *)&(from), sizeof(struct sockaddr_in)) == -1) 
	{   
		EG_LOG_ERROR("[udpbroadcast.c][Init_udpbroadcast][ERROR]: socket bind failed.\r\n");
		close(rxsock);
		rxsock = -1;
		close(txsock);
		txsock = -1;
		return INIT_UDPBROADCAST_SOCKET_BIND_ERROR;
	}

	ret = EG_thread_create(&UDPBroadcastThread_thread,
								"UDPBroadcastThread",
								(void *)UDPBroadcastThread, 0,
								&UDPBroadcastThread_stack, EG_PRIO_3);

	if (ret!=0) {
		EG_LOG_ERROR("Unable to create UDPBroadcastThread.\r\n");
		close(rxsock);
		rxsock = -1;
		close(txsock);
		txsock = -1;
		return INIT_UDPBROADCAST_THREAD_CREATE_ERROR;
    }
	EG_LOG_INFO("EG udpbroadcast started.\r\n"); 
	return ret;
}

int EG_udpbroadcast_stop()
{
	EG_thread_delete(&UDPBroadcastThread_thread);
	
	if (rxsock) {
		close(rxsock);
		rxsock = -1;
	}	
	if (txsock) {
		close(txsock);
		txsock = -1;
	}
	EG_LOG_INFO("EG udpbroadcast stop.\r\n");
	return 0;
}

