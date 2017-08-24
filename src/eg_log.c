
#include "eg_log.h"

void EG_HEX(char *buf,int len)
{
	int i = 0 ;
	for(i=0; i<len;i++)
	{
		EG_P("%02X ",buf[i]);
	}
	EG_P("\r\n");
}
