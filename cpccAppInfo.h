/*  *****************************************
 *  File:		cpccAppInfo.h
  *	Purpose:	Portable (cross-platform), light-weight, library
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
#include "cpccUnicodeSupport.h"

class cpccAppInfo
{
public:
	const static cpcc_char
		*CompanyName,
		*ProgramName,
		*Version,
		*ProgramNameAndVersion,		
		*WebSiteNoHttp,
		*WebSite;
	const static char
		*EmailSale,
		*EmailSupport;
};