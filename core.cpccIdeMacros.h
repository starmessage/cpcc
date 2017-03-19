
/*  *****************************************
 *  File:		cpccDefines.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

 // Visual Studio defines _DEBUG when you specify the /MTd or /MDd option, NDEBUG disables standard-C assertions
 // Use them when apropriate, 
 // ie _DEBUG if you want your debugging code to be consistent with the MS CRT debugging techniques and 
 // NDEBUG if you want to be consistent with assert().

#ifdef _DEBUG
	#define cpccDEBUG
#endif

#ifdef __APPLE__
	#ifndef NDEBUG
		#define cpccDEBUG
	#endif
#endif