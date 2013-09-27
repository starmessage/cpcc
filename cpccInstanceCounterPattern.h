/*  *****************************************
 *  File:		cpccInstanceCounterPattern.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
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


/** Abstract base class to perform instance counting.
	Counts how many objects of a specific class were created.
	Special functions onFirstCreate(), onLastDestroy can be overwritten 
	to allow clildren classes perform initialization/finalisation.
	
	Dependencies: cpcc_SelfTest.h
 
*/



class cpccInstanceCounterPattern
{
private:
	int * nCountPtr;
	
public:	// class metadata and selftest
	static const char * getClassVersion(void) { return "1.01"; };
	
	static void selfTest(void)
	{
		//
	};

protected:


public:	// ctor/dtor

	cpccInstanceCounterPattern(int *aStaticCounterPtr): nCountPtr(aStaticCounterPtr)
	{
		if (nCountPtr)
			*nCountPtr = *nCountPtr + 1;
		
	};

	~cpccInstanceCounterPattern(void)
	{
	
		if (nCountPtr)
			*nCountPtr = *nCountPtr - 1;
	};
		
public: // functions

	const int getInstancesCount(void)	
	{ 
		if (nCountPtr)
			return *nCountPtr; 
		return -999; 	// error indication
	};


};



