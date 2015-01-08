
/*  *****************************************
 *  File:		core.cpccOS.h
 *	Purpose:	Portable (cross-platform), light-weight, OS functions
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once



	
class cpccOS
{
public:
	static void sleep(const int msec)	
	{
	#ifdef _WIN32
		Sleep(msec);
	#elif __APPLE__
		usleep(1000* msec);
	#else
		#error #8724: Unknown platform for cpccOS
	#endif
	}
    
    
};
