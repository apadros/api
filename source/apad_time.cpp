#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_string.h"
#include "apad_time.h"

// ******************** Local API start ******************** //

program_external ui64 cpuCounterFrequencyKHz = Null;

program_local 	 string DateFormatShort  = "dd/mm";
program_local 	 string DateFormatMedium = "dd/mm/yy";
program_local 	 string DateFormatLong   = "dd/mm/yyyy"; // Default format

// ******************** Local API end ******************** //

exported_function bool IsDate(const char* s) {
	AssertRetType(s != Null, false);
	
	string date = ConvertStringToLowerCase(s);
	
	if(date == "mon" || date == "tue" || date == "wed" || date == "thu" || 
		 date == "fri" || date == "sat" || date == "sun")
		 return true;
	
	if(date.length != DateFormatShort.length && date.length != DateFormatMedium.length && date.length != DateFormatLong.length)
		return false;
	
	// Check day
	{
		if(date[2] != '/')
			return false;
		
		date[2] = '\0';
		auto day = StringToInt(date);
		if(day < 0 || day > 31)
			return false;
	}
	
	// Check month
	{
		if(date.length >= DateFormatMedium.length) {
			if(date[5] != '/')
				return false;
			date[5] = '\0';
		}
		
		auto month = StringToInt(date.chars + 3);
		if(month < 0 || month > 12)
			return false;
	}
	
	// Check year
	if(date.length >= DateFormatMedium.length) {
		auto year = StringToInt(date.chars + 6);
		
		if(date.length == DateFormatMedium.length && (year < 0 || year > 99))
			return false;
		if(date.length == DateFormatLong.length && (year < 0 || year > 2099))
			return false;
	}
	
	// @TODO - Check date viability e.g. 31st feb
	
	return true;
}

exported_function date StringToDate(const char* s) {
	AssertRetType(s != Null, date());
	
	auto dateString = ConvertStringToLowerCase(s);
	
	if(dateString == "mon" || dateString == "tue" || dateString == "wed" || dateString == "thu" || 
		 dateString == "fri" || dateString == "sat" || dateString == "sun")
	{
		ui8 argDay = 0; // 1 -> 7 to match the date struct
		{
			const char* days[] = { "mon", "tue", "wed", "thu", "fri", "sat", "sun" };
			ForAll(7) {
				if(dateString == days[it]) {
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
		ui8 day = 0;
		ui8 month = 0;
		ui16 year = 0;
		
		// Day
		{
			dateString[2] = '\0';
			auto d = StringToInt(dateString);
			AssertRetType(d >= 1 && d <= 31, date());
			day = d;
		}
		
		// Month
		{
			if(dateString.length >= DateFormatMedium.length)
				dateString[5] = '\0';
			
			auto m = StringToInt(dateString.chars + 3);
			Assert(m >= 1 && m <= 12)
			month = m;
		}
		
		// Year
		if(dateString.length == DateFormatShort.length) { // If no year is supplied
			auto currentDate = GetDate(0);
			if(month < currentDate.month || month == currentDate.month && day < currentDate.day)
				year = currentDate.year + 1;
			else
				year = currentDate.year;
		}
		else {
			auto y = StringToInt(dateString.chars + 6);
			
			auto currentYear = GetDate(0).year;
			AssertRetType(y >= (currentYear - 2000) && y <= 99 || y >= currentYear && y <= 2099, date());
			
			if(y < 100)
				y += 2000;
			year = y;
		}
		
		// Get the offset between today and target date, then return
		{
			auto currentDate = GetDate(0);
			
			// Check various sizes first in case of API mods
			AssertRetType(sizeof(currentDate.year) == sizeof(ui16), date());
			AssertRetType(sizeof(currentDate.month) == sizeof(ui8), date());
			AssertRetType(sizeof(currentDate.day) == sizeof(ui8), date());
			
			// Get offset between dates
			ui32 currentDateUI32 = currentDate.year << 16 | currentDate.month << 8 | currentDate.day; // Pack into ui32
			ui32 targetDateUI32 = year << 16 | month << 8 | day;
			
			if(targetDateUI32 >= currentDateUI32)
				return GetDate(targetDateUI32 - currentDateUI32);
			else
				return GetDate(-(currentDateUI32 - targetDateUI32));
		}
	}
	
	InvalidCodePath;
	return date();
}

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
	string ret = DateFormatLong;
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