/*  *****************************************
 *  File:		cpccPlatformAPI.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				cpccPlatformAPI is a class abstracting the raw plaform functions 
 *				(except the filesystem related ones)
 *				Further grouping and enapsulation of these API functions are done
 *				by other classes (e.g. miniFileSystem) that in their turn use the platformAPI
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

#include <string>
#include "cpccUnicodeSupport.h"




class cpccPlatformAPI
{
public:	// class metadata and selftest
	const cpcc_char *	getClassVersion(void) { return _T("1.00"); };
	static void			selfTest(void) { };

protected:
	
public:
	// time functions --------------------------------
	
	




	// Other functions --------------------------------

};



