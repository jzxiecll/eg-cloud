#include "eg_time.h"
#define in_digit_range(c, lo, up)  ((uint8_t)c >= lo && (uint8_t)c <= up)
#define isdigit(c)           in_digit_range(c, '0', '9')
#define YEAR0           1900 /* The first year */
#define EPOCH_YR        1970 /* EPOCH = Jan 1 1970 00:00:00 */

unsigned int EJ_SEC_PER_YR[2] = { 31536000, 31622400 };
unsigned int EJ_SEC_PER_MT[2][12] = {
	{2678400, 2419200, 2678400, 2592000, 2678400, 2592000,
	 2678400, 2678400, 2592000, 2678400, 2592000, 2678400}
	,
	{2678400, 2505600, 2678400, 2592000, 2678400, 2592000,
	 2678400, 2678400, 2592000, 2678400, 2592000, 2678400}
	,
};

unsigned int EJ_SEC_PER_DY = 86400;
unsigned int EJ_SEC_PER_HR = 3600;

#define STD_ASCTIME_BUF_SIZE 30
static char asctime_buf[STD_ASCTIME_BUF_SIZE];
static const char wday_name[][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char mon_name[][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static int month_from_string_short(const char *month)
{
	if (strncmp(month, "Jan", 3) == 0)
		return 0;
	if (strncmp(month, "Feb", 3) == 0)
		return 1;
	if (strncmp(month, "Mar", 3) == 0)
		return 2;
	if (strncmp(month, "Apr", 3) == 0)
		return 3;
	if (strncmp(month, "May", 3) == 0)
		return 4;
	if (strncmp(month, "Jun", 3) == 0)
		return 5;
	if (strncmp(month, "Jul", 3) == 0)
		return 6;
	if (strncmp(month, "Aug", 3) == 0)
		return 7;
	if (strncmp(month, "Sep", 3) == 0)
		return 8;
	if (strncmp(month, "Oct", 3) == 0)
		return 9;
	if (strncmp(month, "Nov", 3) == 0)
		return 10;
	if (strncmp(month, "Dec", 3) == 0)
		return 11;
	/* not a valid date */
	return 12;
}




/**
 * Returns 1 if current year id a leap year
 */
static inline int is_leap(int yr)
{
	if (!(yr % 100))
		return (yr % 400 == 0) ? 1 : 0;
	else
		return (yr % 4 == 0) ? 1 : 0;
}

static unsigned char day_of_week_get(unsigned char month, unsigned char day,
				     unsigned short year)
{
	/* Month should be a number 0 to 11, Day should be a number 1 to 31 */

	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	year -= month < 3;
	return (year + year / 4 - year / 100 + year / 400 + t[month - 1] +
		day) % 7;
}

int EG_time_deinit()
{
		return hal_rtc_deinit();
}

/**
 * Init date and time
 */

int EG_time_init()
{

	return hal_rtc_init();
}


/**
 * Set date and time
 */
int EG_time_set(const struct eg_tm *tm)
{
	//printf("EJ_time_set .....\n");
	if (validate_date_time(tm) == 0) {

		hal_rtc_time_t  hal_t;
		hal_t.rtc_sec = tm->tm_sec;                                  /**< Seconds after the minute   - [0,59]  */
	    hal_t.rtc_min = tm->tm_min;                                  /**< Minutes after the hour     - [0,59]  */
	    hal_t.rtc_hour= tm->tm_hour;                                 /**< Hours after the midnight   - [0,23]  */
	    hal_t.rtc_day = tm->tm_mday;                                  /**< Day of the month           - [1,31]  */
	    hal_t.rtc_mon = tm->tm_mon+1;                                  /**< Months                     - [1,12]  */
	    hal_t.rtc_week= tm->tm_wday;                                 /**< Days in a week             - [0,6]   */
	    hal_t.rtc_year= tm->tm_year;   
		//printf("EJ_time_set validate\n");
		return hal_rtc_set_time(&hal_t);
		
	} else {
		printf("EJ_time_set invalidate\n");
		return -1;
	}
}

/**
 * Get date and time
 */
int EG_time_get(struct eg_tm *tm)
{
	//time_t curtime;
	//curtime = EJ_time_get_posix();
	//if (ej_gmtime_r((const time_t *)&curtime, tm) == NULL) {
	//	return -1;
	//}
	hal_rtc_time_t  hal_t;
	int ret = hal_rtc_get_time(&hal_t);
	if(!ret)
	{
		tm->tm_sec   = hal_t.rtc_sec;
		tm->tm_min   = hal_t.rtc_min;
		tm->tm_hour  = hal_t.rtc_hour;
		tm->tm_mday  = hal_t.rtc_day;
		tm->tm_mon   = hal_t.rtc_mon-1;
		tm->tm_wday  = hal_t.rtc_week;
		tm->tm_year  = hal_t.rtc_year;
		return ret ;
	}
	//printf("EJ_time_get error............ret= %d.\n",ret);
	return ret;
}

int EG_wlan_set_time(uint8_t *pTimeArray)
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
		EG_E("set time error.\r\n");
		ret = 0x01;
	}
	return ret ;
}


int EG_wlan_get_time(uint8_t *pTimeArray)
{
	int ret = 0 ;
	struct eg_tm h_tm;
	if (EG_time_get(&h_tm) == -1) {
		EG_E("get time error.\r\n");
		ret = 0x01;
	}
	pTimeArray[0] = h_tm.tm_sec;
	pTimeArray[1] = h_tm.tm_min;
	pTimeArray[2] = h_tm.tm_hour;
	pTimeArray[3] = h_tm.tm_wday;
	pTimeArray[4] = h_tm.tm_mday;
	pTimeArray[5] = h_tm.tm_mon;
	pTimeArray[6] = h_tm.tm_year;

	//printf("EJ_Wlan_get_time  %d:%d:%d:%d:%d:%d\n",pTimeArray[0],pTimeArray[1],pTimeArray[2],\
		//pTimeArray[3],pTimeArray[4],pTimeArray[5]);
	return ret ;
}


