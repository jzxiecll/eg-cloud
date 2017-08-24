#ifndef __EG_LIST_H__
#define __EG_LIST_H__



#ifdef __cplusplus
extern "C" {
#endif
#include "eg_porting_layer.h"


#define EG_MQTTLIST_IN_SIZE 10
#define EG_MQTTLIST_OUT_SIZE 10


typedef enum {
	EG_OPERATION_FAILED = 0x00,
	EG_OPERATION_SUCCESS,
};

typedef struct {
	int length;
	int r_cursor;
	int w_cursor;
	void *data;
}eg_list;

unsigned char new_eg_list(eg_list *list, int length);
unsigned char free_eg_list(eg_list *list);
unsigned char eg_list_push(eg_list *list, void *data);
unsigned char eg_list_pop(eg_list *list, void **data);

unsigned char EG_mqtt_list_create();

eg_list * GetCloud2wifiList();
eg_list * GetWifi2cloudList();
eg_list * GetCloud2deviceList();


#ifdef __cplusplus
}
#endif

#endif // __EG_LIST_H__

