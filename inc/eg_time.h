#ifndef __EG_TIME_H__
#define __EG_TIME_H__

#include "eg_porting_layer.h"

struct eg_tm {
	/** seconds [0-59] */
	int tm_sec;
	/** minutes [0-59] */
	int tm_min;
	/** hours [0-23] */
	int tm_hour;
	/** day of the month [1-31] */
	int tm_mday;
	/** month [0-11] */
	int tm_mon;
	/** year. The number of years since 1900 */
	int tm_year;
	/** day of the week [0-6] 0-Sunday...6-Saturday */
	int tm_wday;
	int	tm_yday;
	int tm_isdst;
};


/**
 * Init date and time
 */

int EG_time_init();

/**
 * Set date and time
 */
int EG_time_set(const struct eg_tm *tm);

/**
 * Get date and time
 */
int EG_time_get(struct eg_tm *tm);
int EG_wlan_set_time(uint8_t *pTimeArray);
int EG_wlan_get_time(uint8_t *pTimeArray);


#endif

