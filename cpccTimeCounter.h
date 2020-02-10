/*  *****************************************
 *  File:		cpccTimeCounter.h
 *  Version:	see function getClassVersion()
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

#include <chrono>
#include <thread>

#include "cpccTesting.h"

/*
	windows solutions:
	• High performance timer on Windows https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408%28v=vs.85%29.aspx 
	  It gives highest possible level of precision (<1us).
	• GetTickCount - 10 to 16 milliseconds of resolution
	• timeGetTime - Uses system clock (so the same resolution as GetTickCount), 
	  but resolution can be increased up to even 1ms (via timeBeginPeriod). 
	• std::chrono - 
		◦ system_clock - System time
		◦ steady_clock - Monotonic clock
		◦ high_resolution_clock - Highest possible resolution, multiplatform. 
		  It might be alias for system or steady clock… depending on the system capabilities.
		  Has problems with VS2013. Ok in VS2015.
*/


/*
    #include <ctime>
    
    std::clock()
    https://en.cppreference.com/w/cpp/chrono/c/clock 
    The value returned by clock() may wrap around on some implementations. 
    For example, on a machine with 32-bit std::clock_t, it wraps after 2147 seconds or 36 minutes. 

    On POSIX-compatible systems, clock_gettime with clock id CLOCK_PROCESS_CPUTIME_ID offers better resolution.

    See also
    https://en.cppreference.com/w/cpp/chrono
*/

/*
#if defined(__APPLE__)
	#include <sys/time.h>
typedef struct timeval	  cpccStructTimeval;

#elif defined(_WIN32)

	struct cpccStructTimeval 
	{
		long tv_sec;
		long tv_usec;
	};

#endif

*/



class cpccTimeCounter
{
private:
    std::chrono::time_point<std::chrono::steady_clock>  mStartTime;

public: // ctor

	explicit cpccTimeCounter() { resetTimer(); }

	
public: // functions

	void		resetTimer(void)	{ mStartTime = std::chrono::steady_clock::now();  }

    double		getSecondsElapsed(void) const
    {
        auto mEndTime = std::chrono::steady_clock::now();
        return (std::chrono::duration_cast<std::chrono::milliseconds>(mEndTime - mStartTime).count() / 1000.0 );
    }
	
};


// /////////////////////////////////////////////////////////////////
//
//		class cpccTimeCounter_lowRes
//
// /////////////////////////////////////////////////////////////////


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

    inline void    restart(void) { mStartTime = getTime(); }

    inline double  getSecondsElapsed(void)
    {
        // Computes difference between two calendar times as std::time_t objects
        // (time_end - time_beg) in seconds.
        return std::difftime(getTime(), mStartTime);
    }

};

// /////////////////////////////////////////////////////////////////
//
//		class cpccTimeCounter_days
//
// /////////////////////////////////////////////////////////////////


class cpccTimeCounter_days
{

private:
    std::time_t         mStartDate = 0;

public: // functions, data

    static inline std::time_t  getDaysSince1970(void) { return (std::time(nullptr) / 3600) / 24; }

    // resets the counter to today's date. This will be used for long running sessions (more than a day)
    inline void         restart(void) { mStartDate = getDaysSince1970(); }
    inline void			setStartDate(const std::time_t aDate) { mStartDate = aDate; }
    inline std::time_t  getDaysElapsed(void) { return getDaysSince1970() - mStartDate; }

};



TEST_RUN_ASYNC(cpccTimeCounter_test)
{
    cpccTimeCounter myCounter;

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    double timePassed = myCounter.getSecondsElapsed();
    TEST_EXPECT(timePassed > 1 && timePassed < 2, _T("#7361: cpccTimeCounter"));
}