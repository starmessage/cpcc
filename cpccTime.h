/*  *****************************************
 *  File:		cpccTime.h
 *	Purpose:	Portable (cross-platform), simple time counter in milliseconds
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <time.h>
#include <ctime>
#include <atomic>
#include <sstream>
#include <math.h>       /* floor */

#include <chrono> // for sleep_for
#include <thread> // for sleep_for



#include "cpccUnicodeSupport.h"

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
	static inline const time_t UTCsecondToCompleteDays(const time_t seconds)	{ return (seconds / 3600) / 24; }

    static inline const time_t UTCsecondToCompleteHours(const time_t seconds)	{ return (seconds / 3600); }
    
	static time_t getUTC2LocalTimeOffset_inSeconds(void)
	{
		static bool isFirstCall = true;
		static time_t result;

		if (isFirstCall)
		{
			struct tm *gmTime;
			time_t localEpoch, gmEpoch;

			/*First get local epoch time*/
            // The value returned generally represents the number of seconds since 00:00 hours, Jan 1, 1970 UTC 
			localEpoch = time(NULL);

			#pragma warning(suppress : 4996)
			gmTime = gmtime(&localEpoch); // Using local time epoch get the GM Time 

			/* Convert gm time in to epoch format */
			gmEpoch = mktime(gmTime);

			/* get the absolute different between them */
			result = localEpoch - gmEpoch;
			isFirstCall = false;
		}
		return result;
	}


	inline static struct tm getCurrentLocalTime(void)	{ return getLocalTimeFromUTCtime(time(NULL)); }

	inline static int getMinutesOfCurrentTime(void)
	{
		time_t	a = time(NULL);
		#pragma warning(suppress : 4996)
		return localtime(&a)->tm_min;
	}

	inline static int getHoursOfCurrentTime(void)
	{
		time_t	a = time(NULL);
		#pragma warning(suppress : 4996)
		return localtime(&a)->tm_hour;
	}

	static struct tm getLocalTimeFromUTCtime(const time_t a)
	{
		struct tm * timeinfo;
		#pragma warning(suppress : 4996)
		timeinfo = localtime(&a); // The returned value points to an internal object whose validity or value may be altered by any subsequent call to gmtime or localtime.

		//printf("Current local time and date: %s", asctime(timeinfo));
		struct tm result;
		if (timeinfo!=NULL) 
			result = *timeinfo;
		return result;
	}

	static bool isSameDay(const struct tm a, const struct tm b) { return	(a.tm_year == b.tm_year) && (a.tm_yday == b.tm_yday); }


    static const cpcc_char* intToMonth(const int index_0to11)
	{
		if ((index_0to11 < 0) || (index_0to11 >= 12))
			return _T("intToMonth() called with index out of 0..11");
		
		const cpcc_char* allMonths[] = 
			{	_T("January"), _T("February"), _T("March"), 
				_T("April"), _T("May"), _T("June"), 
				_T("July"), _T("August"), _T("September"), 
				_T("October"), _T("November"), _T("December") };
		return allMonths[index_0to11];
	}

	static cpcc_string secondsToDDHHMMSSasString(const time_t seconds)
	{
		cpcc_ostringstream result;
		cpcc_string glue;

		time_t days = seconds / 86400;
		if (days > 0)
		{
			result << days << _T(" days");
			glue = _T(", ");
		}

		time_t hours = (seconds / 3600) - (days * 24);
		if (hours > 0)
		{
			result << glue << hours << _T(" hours");
			if (glue.length()==0)
				glue = _T(", ");
		}

		time_t minutes = (seconds / 60) - (days * 1440) - (hours * 60);
		if (minutes > 0)
		{
			result << glue << minutes << _T(" minutes");
			if (glue.length() == 0)
				glue = _T(", ");
		}

		time_t secondsRem = seconds % 60;
		if (secondsRem > 0)
		{
			result << glue << secondsRem << _T(" seconds");
		}

		return result.str();
	}


	static cpcc_string secondsToHHMMSSasString(const double seconds)
	{
		long hours = (long) (seconds / 3600);
		long minutes = (long) ((seconds / 60) - (hours * 60));
		long secondsRem = (long) ((long) seconds % 60);
		double secondsFloat = seconds - floor(seconds);
		/*
		#include <iomanip>
		std::cout << std::setfill('0') << std::setw(2) << hour << ":"; 
		std::cout << std::setfill('0') << std::setw(2) << min << ":" 
		std::cout << std::setfill('0') << std::setw(2) << sec;
		*/

		cpcc_char buffer[300];
		#pragma warning(suppress : 4996)
		cpcc_sprintf(buffer, _T("%02ld:%02ld:%02ld.%i"), hours, minutes, secondsRem, (int) (10*(secondsFloat+0.05)));
		return buffer;

		/*
		cpcc_ostringstream result;
		cpcc_string glue;
		
		if (hours > 0)
		{
			result << hours;
			glue = _T(":");
		}

		if (minutes > 0)
		{
			result << glue << minutes;
			if (glue.length() == 0)
				glue = _T(":");
		}
		
		if (seconds > 0)
		{
			result << glue << seconds;
		}

		return result.str();
		*/
	}

    static void sleep(const time_t msec)
    {
        /* old code */
        /*
        #ifdef _WIN32
	        Sleep(msec);
        #elif __APPLE__
	        usleep(1000* msec);
        #endif
        */
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
    }
};

///////////////////////////////////////////////////////////////////
//		class cpccTimeCounter_lowRes
///////////////////////////////////////////////////////////////////


// fast time counter with low resolution of 1 sec
// https://en.cppreference.com/w/cpp/chrono/c/difftime
class cpccTimeCounter_lowRes
{

private:
    std::time_t mStartTime;

public: // ctor

    explicit cpccTimeCounter_lowRes() : mStartTime(getTime()) {   };

public: // functions

    inline time_t  getTime(void) { return std::time(nullptr); }

    inline void    reset(void) { mStartTime = getTime(); }

    inline time_t  getSecondsElapsed(void) { return (time_t)std::difftime(getTime(), mStartTime); }

};


///////////////////////////////////////////////////////////////////
//		class cpccInterruptableSleep
///////////////////////////////////////////////////////////////////


class cpccInterruptableSleep
{
private:
	const time_t dt_msec = 50;
	std::atomic<bool> 		 &m_interruptRequested;

public:
	explicit cpccInterruptableSleep(std::atomic<bool>  &interruptRequested) : m_interruptRequested(interruptRequested)
	{	}


	void sleep(const time_t sleepFor_msec)
	{
		time_t t = 0;
		while ((!m_interruptRequested) && (t < sleepFor_msec))
		{
            cpccTime::sleep(dt_msec);
			t += dt_msec;
		}
	}
};
