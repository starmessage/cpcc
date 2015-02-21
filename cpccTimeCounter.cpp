﻿/*  *****************************************
 *  File:		cpccTimeCounter.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	(see header file)
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#include "cpccTimeCounter.h"
#include <math.h>	// for floor()


/*
	struct timeval start, end;
	gettimeofday(&start, NULL);
	// benchmark code
	gettimeofday(&end, NULL);
	long long time =   (end.tv_sec * (unsigned int)1e6 +   end.tv_usec) - 
					 (start.tv_sec * (unsigned int)1e6 + start.tv_usec);

	This should give 1-2µs resolution on modern Linux systems

	------------------
	#include <sys/types.h>    
	int gettimeofday(struct timeval *tv, struct timezone *tz);

	struct timeval {
		time_t      tv_sec;     // seconds 
		suseconds_t tv_usec;    // microseconds 
		} 
	------------------
*/

double cpccTimeCounter::getSecondsElapsed(void)
{	struct timeval mEndTime;

	gettimeofdayCrossPlatform(&mEndTime);
	// return difftime( time(0), mStartTime);
	return mEndTime.tv_sec + mEndTime.tv_usec/1e6 - mStartTime.tv_sec - mStartTime.tv_usec/1e6;
}


void cpccTimeCounter::gettimeofdayCrossPlatform(struct timeval *currentTime)
{
	if (!currentTime)
		return;

#if defined(__APPLE__)
	gettimeofday(currentTime, NULL);
#elif defined(_WIN32)
	FILETIME ft;
	ULARGE_INTEGER ui;

	/*
	A 64-bit FILETIME structure receives the system time as FILETIME in 100ns units, 
	which have been expired since Jan 1, 1601. 
	GetSystemTimeAsFileTime returns the current time as the number of 100-nanosecond intervals 
	that have passed since January 1, 1601. 
	*/
	GetSystemTimeAsFileTime(&ft);
	ui.LowPart=ft.dwLowDateTime;
	ui.HighPart=ft.dwHighDateTime;
	// ui.QuadPart contains 100ns units
	// To convert to the Unix epoch, add 116444736000000000LL to reach Jan 1, 1970.
	double time_in_sec = (ui.QuadPart -116444736000000000LL)/1e7;
	currentTime->tv_sec = (long) floor(time_in_sec);
	currentTime->tv_usec = (long) (1e6* (time_in_sec - currentTime->tv_sec));
#else
	#error #9523: Unsupported platform
#endif
}


