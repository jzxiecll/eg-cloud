#include "eg_list.h"


eg_list *cloud2wifiList, *wifi2cloudList, *wifi2deviceList, *device2wifiList, *cloud2deviceList, *device2cloudList;
eg_list *lan2deviceList, *device2lanList, *lan2wifiList, *wifi2lanList;
eg_list *udp2wifiList, *wifi2udpList;
eg_list *wifi2deviceAckList, *device2wifiAckList,  *device2wifiFirmwareOtaAckList, *device2wifiFirmwareOtaactiveAckList;


unsigned char new_eg_list(eg_list *list, int length)
{
	unsigned char ret = 0x00;

	list->data = EG_mem_malloc(sizeof(void *)* length);

	if (list->data != NULL)
	{
		list->r_cursor = 0;
		list->w_cursor = 0;
		list->length = length;

		ret = 0x01;
	}

	return ret;
}

unsigned char free_eg_list(eg_list *list)
{
	unsigned char ret = 0x00;

	if (list->data != NULL)
	{
		EG_mem_free(list->data);

		list->data = NULL;
	}

	return ret;
}

unsigned char eg_list_push(eg_list *list, void *data)
{
	int w_cursor = 0;
	unsigned char ret = 0x00;

	w_cursor = (list->w_cursor + 1) % list->length;

	if (w_cursor != list->r_cursor)
	{
		((void **)list->data)[list->w_cursor] = data;
		list->w_cursor = w_cursor;

		ret = 0x01;
	}

	return ret;

}

unsigned char eg_list_pop(eg_list *list, void **data)
{
	unsigned char ret = 0x00;


	if (list->r_cursor != list->w_cursor)
	{
		*data = ((void **)list->data)[list->r_cursor];
		list->r_cursor = (list->r_cursor + 1) % list->length;

		ret = 0x01;
	}

	return ret;

}
int eg_list_clear(eg_list *list)  
{  
  	void *data;
    while(eg_list_pop(list, &data))
	{
		EG_mem_free(data);
	};
    return 1;  
}  

eg_list * GetCloud2wifiList()
{
	return cloud2wifiList;
}
eg_list * GetWifi2cloudList()
{
	return wifi2cloudList;
}
eg_list * GetCloud2deviceList()
{
	return cloud2deviceList;
}


unsigned char EG_mqtt_list_create()
{
	unsigned char ret = EG_SUCCESS;

	cloud2wifiList = (eg_list *)EG_mem_malloc(sizeof(eg_list));
	if (!cloud2wifiList) 
	{
		return EG_FAIL;
	}
	wifi2cloudList = (eg_list *)EG_mem_malloc(sizeof(eg_list));
	if (!wifi2cloudList) 
	{
		return EG_FAIL;
	}
	
	if(new_eg_list(cloud2wifiList, EG_MQTTLIST_IN_SIZE)!=0x01)
	{
		EG_mem_free(cloud2wifiList);
		EG_mem_free(wifi2cloudList);
		return EG_FAIL;
	}
	if(new_eg_list(wifi2cloudList, EG_MQTTLIST_OUT_SIZE)!=0x01)
	{
		EG_mem_free(cloud2wifiList);
		EG_mem_free(wifi2cloudList);
		return EG_FAIL;
	}
	return ret;
}



