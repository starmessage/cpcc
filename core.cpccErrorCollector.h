
/*  *****************************************
 *  File:		core.cpccErrorCollector.h
 *	Purpose:	Portable (cross-platform), light-weight, class
 *				to help with the error reporting from inside your classes
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
private:


public:	
	std::stringstream	m_errorDump;
	
	void				clearErrors(void) { m_errorDump.str(""); }
	
	bool				hasErrors(void) const { return m_errorDump.str().length()>0; }

	const std::string	getErrorTextAndClearErrors(void)  
	{	std::string txt = m_errorDump.str(); 
		clearErrors(); 
		return txt; 
	}
	
};
