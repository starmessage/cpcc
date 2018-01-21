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

// PimplIdiom to move the inclusion of winsock2.h into the .cpp file
struct timeval;

class cpccTimeCounter
{
private:
	struct timeval *mStartTime;
	void		gettimeofdayCrossPlatform(struct timeval *currentTime);

public: // ctor
	explicit cpccTimeCounter(); 		
	~cpccTimeCounter();
	
public: // functions

	void		resetTimer(void)	{ gettimeofdayCrossPlatform( mStartTime);  }
	double		getSecondsElapsed(void);
	
};


