#ifndef __EG_LOG_H__
#define __EG_LOG_H__

#include "eg_porting_layer.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define EG_LOG_LEVEL  EG_LOG_LEVEL_DEBUG

#define log_printf printf
#define log_fflush fflush
#define EG_P       log_printf

#define EG_LOG_LEVEL_FATAL      (0)
#define EG_LOG_LEVEL_NOTICE     (1)
#define EG_LOG_LEVEL_INFO       (2)
#define EG_LOG_LEVEL_ERROR      (3)
#define EG_LOG_LEVEL_WARN       (4)
#define EG_LOG_LEVEL_DEBUG      (5)

#define Black   0;30
#define Red     0;31
#define Green   0;32
#define Brown   0;33
#define Blue    0;34
#define Purple  0;35
#define Cyan    0;36

#define EG_F(format, ...) \
    do{\
        if(EG_LOG_LEVEL >= EG_LOG_LEVEL_FATAL){\
            log_printf("\033[0;31m[FATAL][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            log_fflush(stdout);\
            log_printf("\033[0m"); \
        }\
    }while(0)

#define EG_N(format, ...) \
    do{\
        if(EG_LOG_LEVEL >= EG_LOG_LEVEL_NOTICE){\
            log_printf("\033[0;36m[NOTICE][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            log_fflush(stdout);\
            log_printf("\033[0m"); \
        }\
    }while(0)

#define EG_I(format, ...) \
    do{\
        if(EG_LOG_LEVEL >= EG_LOG_LEVEL_INFO){\
            log_printf("\033[1;36m[INFO][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            log_fflush(stdout);\
            log_printf("\033[0m"); \
        }\
    }while(0)

#define EG_E(format, ...) \
    do{\
        if(EG_LOG_LEVEL >= EG_LOG_LEVEL_ERROR){\
            log_printf("\033[0;31m[ERROR][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            log_fflush(stdout);\
            log_printf("\033[0m"); \
        }\
    }while(0)

#define EG_W(format, ...) \
    do{\
        if(EG_LOG_LEVEL >= EG_LOG_LEVEL_WARN){\
            log_printf("\033[1;33m[WARN][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            log_fflush(stdout);\
            log_printf("\033[0m"); \
        }\
    }while(0)

#define EG_D(format, ...) \
    do{\
        if(EG_LOG_LEVEL >= EG_LOG_LEVEL_DEBUG){\
            log_printf("\033[1;32m[DEBUG][%s][%s][%d]\n" format "\r\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            log_fflush(stdout);\
            log_printf("\033[0m"); \
        }\
    }while(0)


void EG_HEX(char *buf,int len);

#ifdef __cplusplus
}
#endif




#endif // __EG_LOG_H__

