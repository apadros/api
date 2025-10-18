#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_string.h"
#include "apad_time.h"

// @TODO - Use UTC time instead of local to avoid issues when travelling between different time zones
#include <time.h>
date GetDate(si32 offsetDays) {
	time_t timeNowSecs = time(NULL) + offsetDays * 24 * 60 * 60;
	auto*  timeNow = localtime(&timeNowSecs); // Non-UTC time
	
	date ret = {};
	ret.dayOfTheWeek = timeNow->tm_wday == 0 ? 7 : timeNow->tm_wday;
	ret.year = 1900 + timeNow->tm_year;
	ret.month = 1 + timeNow->tm_mon;
	ret.day = timeNow->tm_mday;
	return ret;
}

short_string DateToString(date d) {
	const char* format = "dd/mm/yyyy";
	short_string ret = {};
	CopyString(format, -1, ret.string, GetStringLength(format, true), true);
	if(ErrorIsSet() == true)
		return ret;
	
	auto temp = ToString(d.day);
	if(ErrorIsSet() == true)
		return ret;
	if(d.day <= 9) {
		ret.string[0] = '0';
		ret.string[1] = temp.string[0];
	}
	else {
		ret.string[0] = temp.string[0];
		ret.string[1] = temp.string[1];
	}
	
	temp = ToString(d.month);
	if(d.month <= 9) {
		ret.string[3] = '0';
		ret.string[4] = temp.string[0];
	}
	else {
		ret.string[3] = temp.string[0];
		ret.string[4] = temp.string[1];
	}
	
	temp = ToString(d.year);
	if(ErrorIsSet() == true)
		return ret;
	ret.string[6] = temp.string[0];
	ret.string[7] = temp.string[1];
	ret.string[8] = temp.string[2];
	ret.string[9] = temp.string[3];
	
	return ret;
}