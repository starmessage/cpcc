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

// PimplIdiom to move the inclusion of winsock2.h into the .cpp file
struct timeval;

class cpccTimeCounter
{
private:
	struct timeval *mStartTime;
	void		gettimeofdayCrossPlatform(struct timeval *currentTime);

public: // ctor
	cpccTimeCounter(); 		
	~cpccTimeCounter();
	
public: // functions

	void		resetTimer(void)	{ gettimeofdayCrossPlatform( mStartTime);  }
	double		getSecondsElapsed(void);
	
};


