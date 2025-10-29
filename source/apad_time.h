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

imported_function string DateToString(date d);
imported_function date 	 GetDate(si32 offsetDays);

#endif