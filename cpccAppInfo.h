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

/* you need to allocate these global variables somewhere in one of your cpp files like this:

	const cpcc_char *cpccAppInfo::CompanyName			= "StarMessage software";
	const cpcc_char *cpccAppInfo::ProgramName			= "StarMessage screensaver";
	const cpcc_char *cpccAppInfo::Version				= "5.0";
	const cpcc_char *cpccAppInfo::ProgramNameAndVersion = "StarMessage screensaver v5.0";
	const cpcc_char *cpccAppInfo::WebSiteNoHttp			= "www.StarMessageSoftware.com";
	const cpcc_char *cpccAppInfo::WebSite				= "www.StarMessageSoftware.com";
	const char		*cpccAppInfo::EmailSales			= "xxx@ccc.aaa";
	const char		*cpccAppInfo::EmailSupport			= "yyy@ccc.aaa";

*/

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
		*EmailSales,
		*EmailSupport;
};