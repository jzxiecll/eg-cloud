#ifndef __EG_PORTING_LAYER_H__
#define __EG_PORTING_LAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <lwip/sockets.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>

#include <lwip/netdb.h>
#include <lwip/stats.h>
#include <lwip/icmp.h>
#include <lwip/ip.h>
#include <lwip/inet_chksum.h>
//#include <errno.h>
//#include <pthread.h>
//#include <stdio.h>
//#include <stdarg.h>
//#include <time.h>
//#include <locale.h>
//#include <string.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <dirent.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "eg_error.h"
#include "eg_log.h"

#include "nvdm.h"
#include "hal.h"
#include "hal_sys.h"
#include "hal_rtc.h"





typedef unsigned char uint8_t;
//typedef unsigned int uint32_t;
typedef unsigned int size_t;
typedef long EJ_BaseType_t;

#define egPASS			( ( EJ_BaseType_t ) 1 )
#define egFAIL			( ( EJ_BaseType_t ) 0 )
#define egFALSE			( ( EJ_BaseType_t ) 0 )
#define egTRUE			( ( EJ_BaseType_t ) 1 )


#ifndef PACK_START
#define PACK_START
#endif
#ifndef PACK_END
#define PACK_END __attribute__((packed))
#endif


/** Wait Forever */
#define EG_WAIT_FOREVER    portMAX_DELAY
/** Do Not Wait */
#define EG_NO_WAIT         0
#define EG_PRIO_0     4	 /** High **/
#define EG_PRIO_1     3
#define EG_PRIO_2     2
#define EG_PRIO_3     1
#define EG_PRIO_4     0	 /** Low **/

typedef xTaskHandle eg_thread_t;
typedef void *eg_thread_arg_t;
typedef struct eg_thread_stack {
	/** Total stack size */
	int size;
} eg_thread_stack_t;
#define eg_thread_stack_define(stackname, stacksize)		\
	eg_thread_stack_t stackname =				\
		{(stacksize) / (sizeof(portSTACK_TYPE))}




typedef void *eg_timer_arg_t;
typedef xTimerHandle eg_timer_t;
typedef portTickType eg_timer_tick;

/** OS Timer reload Options
 *
 */
typedef enum eg_timer_reload {
	/**
	 * Create one shot timer. Timer will be in the dormant state after
	 * it expires.
	 */
	EG_TIMER_ONE_SHOT,
	/**
	 * Create a periodic timer. Timer will auto-reload after it expires.
	 */
	EG_TIMER_PERIODIC,
} eg_timer_reload_t;

/**
 * OS Timer Activate Options
 */
typedef enum eg_timer_activate {
	/** Start the timer on creation. */
	EG_TIMER_AUTO_ACTIVATE,
	/** Do not start the timer on creation. */
	EG_TIMER_NO_ACTIVATE,
} eg_timer_activate_t;


/** Create timer**********************************************************
 **
 ***This function creates a timer.
 ***********************************************************************/
int EJ_timer_create(eg_timer_t *timer_t, const char *name, eg_timer_tick ticks,
		    void (*call_back) (eg_timer_arg_t), void *cb_arg,
		    eg_timer_reload_t reload, eg_timer_activate_t activate);
int EJ_timer_change(eg_timer_t *timer_t, eg_timer_tick ntime,
				eg_timer_tick block_time);
int EJ_timer_deactivate(eg_timer_t *timer_t);
bool EJ_timer_is_running(eg_timer_t *timer_t);
int EJ_timer_activate(eg_timer_t *timer_t);
int EJ_timer_reset(eg_timer_t *timer_t);





/*
************************************************************************
* Queue Manager 
*
************************************************************************
*/

typedef xQueueHandle eg_queue_t;
/** Structure used for queue definition */
typedef struct eg_queue_pool {
	/** Size of the queue */
	int size;
} eg_queue_pool_t;

#define eg_queue_pool_define(poolname, poolsize)	\
	eg_queue_pool_t poolname = { poolsize };



int EG_queue_create(eg_queue_t *qhandle, const char *name, int msgsize, eg_queue_pool_t *poolname);
int EG_queue_send(eg_queue_t *qhandle, const void *msg, unsigned long wait);
int EG_queue_send_Overwrite(eg_queue_t *qhandle, const void *msg,unsigned long wait);
int EG_queue_recv(eg_queue_t *qhandle, void *msg,unsigned long wait);
int EG_queue_delete(eg_queue_t *qhandle);

/*
************************************************************************
* Memmoy Manager 
*
************************************************************************
*/

void* EG_mem_malloc(size_t sz);
void EG_mem_free(void *ptr);

unsigned long EG_msec_to_ticks(unsigned long msecs);


void EG_thread_sleep(int ticks);
int EG_thread_create(eg_thread_t *thandle, const char *name,
		  void (*main_func)(eg_thread_arg_t arg),
		  void *arg, eg_thread_stack_t *stack, int prio);
void EG_thread_Suspend(eg_thread_t *thandle);
int EG_thread_delete(eg_thread_t *thandle);
void EJ_thread_resume(eg_thread_t *thandle);








/*
************************************************************************
* Mutex Manager 
*
************************************************************************
*/
typedef xSemaphoreHandle eg_mutex_t;
#define xSemaphoreCreateRecursiveMutex() xQueueCreateMutex( queueQUEUE_TYPE_RECURSIVE_MUTEX )
#define xSemaphoreTakeRecursive( xMutex, xBlockTime )	xQueueTakeMutexRecursive( ( xMutex ), ( xBlockTime ) )
#define xSemaphoreGiveRecursive( xMutex )	xQueueGiveMutexRecursive( ( xMutex ) )
#define vSemaphoreDelete( xSemaphore ) vQueueDelete( ( QueueHandle_t ) ( xSemaphore ) )
	
	
typedef struct Mutex
{
   	eg_mutex_t mtx;

} Mutex;
void MutexInit(Mutex* m);
void MutexDeinit(Mutex* m);
int MutexLock(Mutex* m);
int MutexUnlock(Mutex* m);


#ifdef __cplusplus
}
#endif

#endif // __EG_PORTING_LAYER_H__

