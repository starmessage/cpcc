/*  *****************************************
 *  File:		cpccInstanceCounterPattern.h
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.starmessagesoftware.com/cpcclibrary/
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <atomic>

/** Abstract base class to perform instance counting.
	Counts how many objects of a specific class were created.
	Special functions onFirstCreate(), onLastDestroy can be overwritten 
	to allow clildren classes perform initialization/finalisation.
*/



class cpccInstanceCounterPattern
{
private:
	std::atomic<int> &_nCount;

public:	// ctor/dtor

	explicit cpccInstanceCounterPattern(std::atomic<int> &aStaticCounter): _nCount(aStaticCounter)	{   ++_nCount;  }

	~cpccInstanceCounterPattern(void)   {   --_nCount;  }
		
public: // functions

	const int getInstancesCount(void)   {   return _nCount; }

};



