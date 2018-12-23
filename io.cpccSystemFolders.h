/*  *****************************************
 *  File:		cpccFileSystemMini.h
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include "cpccUnicodeSupport.h"

/** A small and portable (cross platform) C++ class with basic system folder functions.
	Simply add the .h and the .cpp in your application.
	On MAC, add also the .mm file
	
	It was created because the boost::filesystem is too complicated and needs considerable effort 
	to be included in your application.
*/
class cpccSystemFolders
{

public:
	static cpcc_string getFolder_CommonAppData(void);
    static cpcc_string getFolder_UserData(void);
    static cpcc_string getFolder_CurrentDir(void);

};



