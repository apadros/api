#include <time.h>
#include "apad_array.h"
#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_intrinsics.h"
#include "apad_string.h"
#include "apad_time.h"

// ******************** Local API start ******************** //

program_external 			 ui64 cpuCounterFrequencyKHz = Null;

program_local 	 const char* DateFormatShort  = "dd/mm";
program_local 	 const char* DateFormatMedium = "dd/mm/yy";
program_local 	 const char* DateFormatLong   = "dd/mm/yyyy"; // Default format
program_local    const char* Days[] 					= { "mon", "tue", "wed", "thu", "fri", "sat", "sun" };

program_local date ConvertCSLTimeToDate(struct tm* time) {
	date ret = {};
	ret.dayOfTheWeek = time->tm_wday == 0 ? 7 : time->tm_wday;
	ret.year = 1900 + time->tm_year;
	ret.month = 1 + time->tm_mon;
	ret.day = time->tm_mday;
	return ret;
}

// ******************** Local API end ******************** //

exported_function bool IsDate(const char* s) {
	AssertRetType(s != Null, false);
	
	ConvertStringToLowerCase(s);
	
	if(StringIsEqualToAny(s, Days, GetArrayLength(Days)) == true)
		return true;
	
	
	// Check against allowed formats
	auto length = GetStringLength(s);
	if(length != GetStringLength(DateFormatShort) && length != GetStringLength(DateFormatMedium) && length != GetStringLength(DateFormatLong))
		return false;
	if(s[2] != '/')
			return false;
	if(length >= GetStringLength(DateFormatMedium) && s[5] != '/')
		return false;
	
	// @TODO - Check date viability e.g. 31st feb
	
	// Check day
	{
		auto day = StringToInt(s, 2);
		if(day < 0 || day > 31)
			return false;
	}
	
	// Check month
	{
		auto month = StringToInt(s + 3, 2);
		if(month < 0 || month > 12)
			return false;
	}
	
	// Allow whatever year is desired so long as it fits the format
	
	return true;
}

exported_function date StringToDate(const char* s) {
	AssertRetType(s != Null, date());
	
	ConvertStringToLowerCase(s);
	
	if(StringIsEqualToAny(s, Days, GetArrayLength(Days)) == true) {
		ui8 argDay = 0; // 1 -> 7 to match the date struct
		{
			ForAll(GetArrayLength(Days)) {
				if(StringsAreEqual(s, Days[it]) == true) {
					argDay = it + 1;
					break;
				}
			}
		}
		
		si8 offset = argDay - GetDate(0).dayOfTheWeek;
		if(offset < 0)
			offset += 7;
		
		return GetDate(offset);
	}
	else { // Date in short, medium or long format
		ui8 day = StringToInt(s, 2);
		ui8 month = StringToInt(s + 3, 2);
		
		ui16 year = 0;
		if(GetStringLength(s) == GetStringLength(DateFormatShort)) { // If no year is supplied
			auto currentDate = GetDate(0);
			if(month < currentDate.month || month == currentDate.month && day < currentDate.day)
				year = currentDate.year + 1;
			else
				year = currentDate.year;
		}
		else if(GetStringLength(s) == GetStringLength(DateFormatMedium)) {
			char string[] = { '2', '0', s[6], s[7] };
			year = StringToInt(string, 4);
		}
		else // Long date format
			year = StringToInt(s + 6, Null);
		
		// Get the offset between today and target date, then return
		{
			// Setup content of tm struct, for some reason simply supplying day, month and year
			// without doing this first yields incorrect results
			time_t timeNowSecs = time(NULL);
			auto*  time = localtime(&timeNowSecs); // Non-UTC time
			
			// Set desired day, month and year
			time->tm_mday = day;
			time->tm_mon = month - 1;
			time->tm_year = year - 1900;
			
			// Convert and return
			mktime(time); // Will set w_day
			return ConvertCSLTimeToDate(time);
		}
	}
	
	InvalidCodePath;
	return date();
}

// @TODO - Use UTC time instead of local to avoid issues when travelling between different time zones
exported_function date GetDate(si32 offsetDays) {
	time_t timeNowSecs = time(NULL) + offsetDays * 24 * 60 * 60;
	auto*  timeNow = localtime(&timeNowSecs); // Non-UTC time	
	return ConvertCSLTimeToDate(timeNow);
}

exported_function char* DateToString(date d) {
	char* ret = AllocateString(DateFormatLong, Null);
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
	FreeString(temp);
	
	temp = ToString(d.month);
	if(d.month <= 9) {
		ret[3] = '0';
		ret[4] = temp[0];
	}
	else {
		ret[3] = temp[0];
		ret[4] = temp[1];
	}
	FreeString(temp);
	
	temp = ToString(d.year);
	if(ErrorIsSet() == true)
		return ret;
	ret[6] = temp[0];
	ret[7] = temp[1];
	ret[8] = temp[2];
	ret[9] = temp[3];
	FreeString(temp);
	
	return ret;
}

exported_function time_marker GetTimeMarker() {
	exported_function time_marker Win32GetTimeMarker();
	return Win32GetTimeMarker();
}

exported_function f32 GetTimeElapsedMilli(time_marker markerStart, time_marker markerEnd) {
  AssertRetType(markerStart != Null, Null);
  AssertRetType(markerEnd != Null, Null);
  AssertRetType(cpuCounterFrequencyKHz != Null, Null);
  return (((f64)(markerEnd - markerStart) / cpuCounterFrequencyKHz) * 1000);
}