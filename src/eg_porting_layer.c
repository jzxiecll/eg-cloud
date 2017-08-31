#include "eg_porting_layer.h"

#define is_isr_context() (__get_IPSR() != 0)

int EG_timer_change(eg_timer_t *timer_t, eg_timer_tick ntime,
				  eg_timer_tick block_time)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = egFALSE;
	if (!timer_t || !(*timer_t))
		return -EG_E_INVAL;
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerChangePeriodFromISR(*timer_t, ntime,
						&xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		/* Fixme: What should be value of xBlockTime? */
		ret = xTimerChangePeriod(*timer_t, ntime, 100);
	return ret == egPASS ? EG_SUCCESS : -EG_FAIL;
}


 int EG_timer_deactivate(eg_timer_t *timer_t)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = egFALSE;
	if (!timer_t || !(*timer_t))
		return -EG_E_INVAL;
	/* Note:
	 * XTimerStop, seconds argument is xBlockTime which means, the time,
	 * in ticks, that the calling task should be held in the Blocked
	 * state, until timer command succeeds.
	 * We are giving as 0, to be consistent with threadx logic.
	 */
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerStopFromISR(*timer_t, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xTimerStop(*timer_t, 0);
	return ret == egPASS ? EG_SUCCESS : -EG_FAIL;
}



 bool EG_timer_is_running(eg_timer_t *timer_t)
{
	int ret;
	if (!timer_t || !(*timer_t))
		return false;
	ret = xTimerIsTimerActive(*timer_t);
	return ret == egPASS ? true : false;
}



int EG_timer_activate(eg_timer_t *timer_t)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = egFALSE;
	if (!timer_t || !(*timer_t))
		return -EG_E_INVAL;
	/* Note:
	 * XTimerStart, seconds argument is xBlockTime which means, the time,
	 * in ticks, that the calling task should be held in the Blocked
	 * state, until timer command succeeds.
	 * We are giving as 0, to be consistent with threadx logic.
	 */
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerStartFromISR(*timer_t, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xTimerStart(*timer_t, 0);
	return ret == egPASS ? EG_SUCCESS : -EG_FAIL;
}

int EG_timer_reset(eg_timer_t *timer_t)
{
	int ret;
	portBASE_TYPE xHigherPriorityTaskWoken = egFALSE;

	if (!timer_t || !(*timer_t))
		return -EG_E_INVAL;
	/* Note:
	 * XTimerStop, seconds argument is xBlockTime which means, the time,
	 * in ticks, that the calling task should be held in the Blocked
	 * state, until timer command succeeds.
	 * We are giving as 0, to be consistent with threadx logic.
	 */
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xTimerResetFromISR(*timer_t, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else
		ret = xTimerReset(*timer_t, 0);
	return ret == egPASS ? EG_SUCCESS : -EG_FAIL;
}


int EG_timer_create(eg_timer_t *timer_t, const char *name, eg_timer_tick ticks,
		    void (*call_back)(eg_timer_arg_t), void *cb_arg,
		    eg_timer_reload_t reload,
		    eg_timer_activate_t activate)
{
	int auto_reload = (reload == EG_TIMER_ONE_SHOT) ? egFALSE : egTRUE;
	*timer_t = xTimerCreate(name, ticks, auto_reload, cb_arg, call_back);
	if (*timer_t == NULL)
		return -EG_FAIL;
	if (activate == EG_TIMER_AUTO_ACTIVATE)
		return EG_timer_activate(timer_t);
	return EG_SUCCESS;
}


/*
* *****************************************************          
*   QUEUE   MODULE
*******************************************************
*/

int EG_queue_create(eg_queue_t *qhandle, const char *name, int msgsize, eg_queue_pool_t *poolname)
{

	/** The size of the pool divided by the max. message size gives the
	 * max. number of items in the queue. */	
	*qhandle = xQueueCreate(poolname->size/msgsize, msgsize);	
	if (*qhandle) {
		return EG_SUCCESS;
	}
	return -EG_FAIL;
}


/** Receive an item from queue
 *
 *
 */

int EG_queue_send_Overwrite(eg_queue_t *qhandle, const void *msg,
				unsigned long wait)
{
	int ret;
	signed portBASE_TYPE xHigherPriorityTaskWoken = egFALSE;
	if (!qhandle || !(*qhandle))
		return -EG_E_INVAL;
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xQueueOverwriteFromISR(*qhandle, msg,
					      &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else{
		ret = xQueueOverwrite(*qhandle, msg);
		
	}
	return ret == egTRUE ? EG_SUCCESS : -EG_FAIL;
}



int EG_queue_send(eg_queue_t *qhandle, const void *msg,
				unsigned long wait)
{
	int ret;
	signed portBASE_TYPE xHigherPriorityTaskWoken = egFALSE;
	if (!qhandle || !(*qhandle))
		return -EG_E_INVAL;
	if (is_isr_context()) {
		/* This call is from Cortex-M3 handler mode, i.e. exception
		 * context, hence use FromISR FreeRTOS APIs.
		 */
		ret = xQueueSendToBackFromISR(*qhandle, msg,
					      &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	} else{
		ret = xQueueSendToBack(*qhandle, msg, wait);
		
	}
	return ret == egTRUE ? EG_SUCCESS : -EG_FAIL;
}


/** Receive an item from queue
 *
 * 
 */
 int EG_queue_recv(eg_queue_t *qhandle, void *msg,
				unsigned long wait)
{
	int ret;
	if (!qhandle || !(*qhandle))
		return -EG_E_INVAL;
	ret = xQueueReceive(*qhandle, msg, wait);
	return ret == egTRUE ? EG_SUCCESS : -EG_FAIL;
}

/** Delete queue
 *
 * 
 */
 int EG_queue_delete(eg_queue_t *qhandle)
{
	
	vQueueDelete(*qhandle);
	//sem_debug_delete((const xSemaphoreHandle)*qhandle);
	*qhandle = NULL;
	return EG_SUCCESS;
}


 int EG_queue_get_msgs_waiting(eg_queue_t *qhandle)
{
	int nmsg = 0;
	if (!qhandle || !(*qhandle))
		return -EG_E_INVAL;
	nmsg = uxQueueMessagesWaiting(*qhandle);
	return nmsg;
}



/*
* *****************************************************          
*   MEMMERY   MODULE
*******************************************************
*/

void* EG_mem_malloc(size_t sz)
{  
    void *buf = NULL;
    buf = pvPortMalloc(sz);
    if(NULL == buf)
        EG_P("eg_malloc() Error! size:%d", sz);
    return buf;
}

void EG_mem_free(void *ptr)
{   
    if(ptr)
        vPortFree(ptr);
}






unsigned long EG_msec_to_ticks(unsigned long msecs)
{
	return (msecs) / (portTICK_RATE_MS);
}

void EG_thread_sleep(int ticks)
{

	vTaskDelay(ticks);
	return;
}



 
int EG_thread_create(eg_thread_t *thandle, const char *name,
		  void (*main_func)(eg_thread_arg_t arg),
		  void *arg, eg_thread_stack_t *stack, int prio)
{
 int ret;
 ret = xTaskCreate(main_func, name, stack->size, arg, prio, thandle);
 return ret == egPASS ? EG_SUCCESS : -EG_FAIL;
}


void EG_thread_Suspend(eg_thread_t *thandle)
{
 /* Suspend self until someone calls delete. This is required because in
  * freeRTOS, main functions of a thread cannot return.
  */
 if (thandle != NULL) { 	 
	 vTaskSuspend(*thandle);
 } else {		 
	 vTaskSuspend(NULL);
 }

 /*
  * We do not want this function to return ever.
  */
}

int EG_thread_delete(eg_thread_t *thandle)
{

 if (thandle == NULL) { 	 
	 vTaskDelete(NULL);
 } else {		 
	 vTaskDelete(*thandle);
 }
 *thandle = NULL;
 return EG_SUCCESS;
}

void EG_thread_self_complete(eg_thread_t *thandle)
{
	/* Suspend self until someone calls delete. This is required because in
	 * freeRTOS, main functions of a thread cannot return.
	 */
	if (thandle != NULL) {		
		vTaskSuspend(*thandle);
	} else {
		vTaskSuspend(NULL);
	}
	/*
	 * We do not want this function to return ever.
	 */
	while (1)
		EG_thread_sleep(EG_msec_to_ticks(60000));
}

void EJ_task_Resume(eg_thread_t xTaskToResume)
{
 	vTaskResume(xTaskToResume);
}

/*
*
*
*
*
*/
static  int eg_recursive_mutex_create(eg_mutex_t *mhandle,
					    const char *name)
{
	if (!mhandle)
		return -EG_E_INVAL;
	*mhandle = xSemaphoreCreateRecursiveMutex();
	if (!*mhandle)
		return -EG_FAIL;
	return EG_SUCCESS;
}


static  int eg_recursive_mutex_get(eg_mutex_t *mhandle,
					 unsigned long wait)
{	
	int ret = xSemaphoreTakeRecursive(*mhandle, wait);
	return ret == egTRUE ? EG_SUCCESS : -EG_FAIL;
}


static  int eg_recursive_mutex_put(eg_mutex_t *mhandle)
{
	
	int ret = xSemaphoreGiveRecursive(*mhandle);
	return ret == egTRUE ? EG_SUCCESS : -EG_FAIL;
}


static  int eg_mutex_delete(eg_mutex_t *mhandle)
{
	vSemaphoreDelete(*mhandle);	
	*mhandle = NULL;
	return EG_SUCCESS;
}

void MutexInit(Mutex* m)
{
    if (!m)
    {
        return;
    }
    eg_recursive_mutex_create(&(m->mtx), "mutex");	
}

void MutexDeinit(Mutex* m)
{
    if (!m)
    {
        return;
    }
	eg_mutex_delete(&(m->mtx));
}


int MutexLock(Mutex* m)
{
    if (!m)
    {
        return -1;
    }   
	eg_recursive_mutex_get(&(m->mtx), EG_WAIT_FOREVER);
    return 0;
}


int MutexUnlock(Mutex* m)
{
    if (!m)
    {
        return -1;
    }
	eg_recursive_mutex_put(&(m->mtx));
    return 0;
}


