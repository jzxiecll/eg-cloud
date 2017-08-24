#ifndef __EG_CLOUD_H__
#define __EG_CLOUD_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char(*EG_DEV_MSG_CB)(unsigned char *,int);
void EG_cloud_msg_process(EG_DEV_MSG_CB f_dev_callback);
int EG_device_snap_shot(char *out_snap, unsigned int out_len);
int EG_start();


#ifdef __cplusplus
}
#endif

#endif // __EG_SMARTLINK_H__






