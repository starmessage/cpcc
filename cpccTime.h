﻿/*  *****************************************
 *  File:		cpccTime.h
 *	Purpose:	Portable (cross-platform), simple time counter in milliseconds
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <time.h>

/*	------------------	NOTES  ------------------------
time.h in windows CE
http://blogs.msdn.com/cenet/archive/2006/04/29/time-h-on-windows-ce.aspx

Tutorial about time functions
http://www.codeproject.com/Articles/3049/Date-and-Time-in-C

UTC (Coordinated Universal Time) : This is the standard international time
or the Greenwich Mean Time.
epoch : number of seconds elapsed since 00:00:00 on January 1, 1970,
Coordinated Universal Time.

The calendar time information in the Standard Library header file can themselves
be broken into at least two groups:
1. The local time or broken-down time which is represented by the tm structure as follows in the time.h
2. The Calendar Time which is represented by the time_t data type. 
   The value of time_t is typically the number of seconds elapsed since some implementation-specific base time.

There is an array of functions that can be used, all of which use time_t as either their parameters or return values:
double difftime(time_t time1, time_t time0);
time_t mktime(struct tm * timeptr);
time_t time(time_t * timer);
char * asctime(const struct tm * timeptr);
char * ctime(const time_t *timer);

struct tm * gmtime(const time_t *timer);
//converts the calender time represented by  timer
//to a UTC time represented by the tm structure.

//converts the calendar time to the local time
struct tm * localtime(const time_t * timer);

struct tm
http://linux.die.net/man/3/ctime
tm_sec	seconds after the minute	0-61*
tm_min	minutes after the hour	0-59
tm_hour	hours since midnight	0-23
tm_mday	day of the month	1-31
tm_mon	months since January	0-11
tm_year	years since 1900
tm_wday	days since Sunday	0-6
tm_yday	days since January 1	0-365
tm_isdst	Daylight Saving Time flag


time_t
 time_t is always in UTC, by definition
 It is almost universally expected to be an integral value representing the
 number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC. This is due
 to historical reasons, since it corresponds to a unix timestamp, but is
 widely implemented in C libraries across all platforms.


*/




///////////////////////////////////////////////////////////////////
//		class cpccTime
///////////////////////////////////////////////////////////////////
class cpccTime
{
public:
	static time_t UTCsecondToCompleteDays(const time_t seconds)	{ return (seconds / 3600) / 24; }

	static time_t getUTC2LocalTimeOffset_inSeconds(void)
	{
		struct tm *gmTime;
		time_t localEpoch, gmEpoch;

		/*First get local epoch time*/
		localEpoch = time(NULL);

		#pragma warning(suppress : 4996)
		/* Using local time epoch get the GM Time */
		gmTime = gmtime(&localEpoch);

		/* Convert gm time in to epoch format */
		gmEpoch = mktime(gmTime);

		/* get the absolute different between them */
		return localEpoch - gmEpoch;
	}


	static struct tm getCurrentLocalTime(void)	{ return getLocalTimeFromUTCtime(time(NULL)); }


	static struct tm getLocalTimeFromUTCtime(const time_t a)
	{
		struct tm * timeinfo;
		#pragma warning(suppress : 4996)
		timeinfo = localtime(&a); // The returned value points to an internal object whose validity or value may be altered by any subsequent call to gmtime or localtime.

		//printf("Current local time and date: %s", asctime(timeinfo));
		struct tm result = *timeinfo;
		return result;
	}

	static bool isSameDay(const struct tm a, const struct tm b) { return	(a.tm_year == b.tm_year) && (a.tm_yday == b.tm_yday); }

	/*
	static double long difference_inseconds(const struct tm a, const struct tm b)
	{

	}
	*/


};
