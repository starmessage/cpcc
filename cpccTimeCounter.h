/*  *****************************************
 *  File:		cpccTimeCounter.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), simple time counter in milliseconds
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once


#if defined(__APPLE__)
	#include <sys/time.h>
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

	#include <winsock2.h> //  timeval under Windows is part of winsock2.h

#endif



class cpccTimeCounter
{
private:
	struct timeval mStartTime;
	
	void		gettimeofdayCrossPlatform(struct timeval *currentTime);

public:	// class metadata and selftest
	const char * getClassVersion(void) { return "1.01"; };
	static void selfTest(void);
	
public: // ctor
	cpccTimeCounter();
	
public: // functions
	void		resetTimer(void);
	double		getSecondsElapsed(void);
	
};


