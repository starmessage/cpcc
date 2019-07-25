/*  *****************************************
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



#if defined(__APPLE__)
	// #include <sys/time.h>
#elif defined(_WIN32)
	/*
	If you get " 'AF_IPX' : macro redefinition"
	This problem is caused when including <windows.h> before <winsock2.h>.
	Try arrange your include list that <windows.h> is included after <winsock2.h> or
	define _WINSOCKAPI_ first in all .cpp files that include cpccTimeCounter.h

	#define _WINSOCKAPI_    // stops windows.h including winsock.h
	#include <windows.h>
	// ...
	#include "MyClass.h"    // Which includes <winsock2.h>
	*/

	#include <winsock2.h> //  struct timeval under Windows is part of winsock2.h

#endif


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


cpccTimeCounter::cpccTimeCounter()
{	
	resetTimer(); 
}


double cpccTimeCounter::getSecondsElapsed(void) const
{	
	const cpccStructTimeval mEndTime = gettimeofdayCrossPlatform();
	// return difftime( time(0), mStartTime);
	//return mEndTime.tv_sec + mEndTime.tv_usec/1e6 - mStartTime.tv_sec - mStartTime.tv_usec/1e6;
	return mEndTime.tv_sec + mEndTime.tv_usec / 1e6 - mStartTime.tv_sec - mStartTime.tv_usec / 1e6;
}


cpccStructTimeval cpccTimeCounter::gettimeofdayCrossPlatform(void) const
{
    cpccStructTimeval result;

	
#if defined(__APPLE__)
	
	gettimeofday(&result, NULL);

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
	result.tv_sec = (long) floor(time_in_sec);
	result.tv_usec = (long) (1e6* (time_in_sec - result.tv_sec));
#else
	#error #9523: Unsupported platform
#endif

	return result;
}


