
/*  *****************************************
 *  File:		app.cpccApp.h
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

#if defined(__APPLE__) 
	#include "app.cpccAppMac.h"
	typedef cpccAppMac	cpccAppImpl;

#elif defined(_WIN32) 
	#include "app.cpccAppWin.h"
	typedef cpccAppWin	cpccAppImpl;

#else
	#error #4567: Unknown platform for cpccApp
#endif

	
class cpccApp: public cpccAppImpl
{
public:
	cpccApp(void) 
	{ 
		infoLog().addf( _T("creating cpccApp"));
		logInformation(); 
	}

	~cpccApp(void) 
	{ 
		infoLog().addf( _T("destroying cpccApp"));
	}
};
