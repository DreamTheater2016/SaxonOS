#include <common.h>

uint32_t get_year(void);
uint8_t get_month(void);
uint8_t get_day(void);
uint8_t get_hour(void);
uint8_t get_minute(void);
uint8_t get_second(void);

#define UNIX_TIME_2017 1483228800

#define MINUTE_TO_SECOND(a) (a * 60)
#define HOUR_TO_SECOND(a) (a * 3600)
#define DAY_TO_SECOND(a) (a * 86400)
#define MONTH_TO_SECOND(a) (a * 2592000)
#define YEAR_TO_SECOND(a) (a * 31536000)

int sys_time(time_t *tloc)
{
	const uint32_t year = get_year() - 1970;
	const uint8_t month = get_month();
	const uint8_t day = get_day();
	const uint8_t hour = get_hour();
	const uint8_t minute = get_minute();
	const uint8_t second = get_second();

        int ret = YEAR_TO_SECOND(year) + MONTH_TO_SECOND(month)
		+ DAY_TO_SECOND(day) + HOUR_TO_SECOND(hour)
		+ MINUTE_TO_SECOND(minute) + second;
	if (tloc != 0)
		*tloc = (time_t)ret;
	return ret;
}
