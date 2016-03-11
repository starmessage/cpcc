
/*  *****************************************
 *  File:		core.cpccErrorCollector.h
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

#include <iostream>

class cpccErrorCollector
{

public:	
	//std::ostringstream	errorDump;
	std::stringstream	errorDump;
	bool		hasErrors(void) const { return errorDump.str().length() > 0; }	
	const char * getErrorText(void) const { return errorDump.str().c_str(); }
	void		 clearErrors(void) { errorDump.str(""); }
};
