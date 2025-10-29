#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_string.h"
#include "apad_time.h"

// @TODO - Use UTC time instead of local to avoid issues when travelling between different time zones
#include <time.h>
exported_function date GetDate(si32 offsetDays) {
	time_t timeNowSecs = time(NULL) + offsetDays * 24 * 60 * 60;
	auto*  timeNow = localtime(&timeNowSecs); // Non-UTC time
	
	date ret = {};
	ret.dayOfTheWeek = timeNow->tm_wday == 0 ? 7 : timeNow->tm_wday;
	ret.year = 1900 + timeNow->tm_year;
	ret.month = 1 + timeNow->tm_mon;
	ret.day = timeNow->tm_mday;
	return ret;
}

exported_function string DateToString(date d) {
	string ret = "dd/mm/yyyy";
	if(ErrorIsSet() == true)
		return ret;
	
	auto temp = ToString(d.day);
	if(ErrorIsSet() == true)
		return ret;
	if(d.day <= 9) {
		ret[0] = '0';
		ret[1] = temp[0];
	}
	else {
		ret[0] = temp[0];
		ret[1] = temp[1];
	}
	
	temp = ToString(d.month);
	if(d.month <= 9) {
		ret[3] = '0';
		ret[4] = temp[0];
	}
	else {
		ret[3] = temp[0];
		ret[4] = temp[1];
	}
	
	temp = ToString(d.year);
	if(ErrorIsSet() == true)
		return ret;
	ret[6] = temp[0];
	ret[7] = temp[1];
	ret[8] = temp[2];
	ret[9] = temp[3];
	
	return ret;
}