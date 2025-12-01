#ifndef APAD_TIME_H
#define APAD_TIME_H

#include "apad_base_types.h"
#include "apad_string.h"

struct date {
  ui8  dayOfTheWeek; // 1 -> 7
  ui8  day; 				 // 1 -> 31
  ui8  month; 			 // 1 -> 12
  ui16 year;
};

/* Supported date argument formats are:
 * - dd/mm, dd/mm/yy and dd/mm/yyyy
 * - Day of the week: mon, tue, wed, thu, fri, sat & sun (also be written with a capital first letter (e.g. Mon))
 */

imported_function string DateToString(date d); // Returned string format is dd/mm/yyyy
imported_function date 	 GetDate(si32 offsetDays);
imported_function bool 	 IsDate(const char* s);
imported_function date 	 StringToDate(const char* s); // IsDate() should always be called before this

#endif