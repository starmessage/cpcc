/*  *****************************************
 *  File:		core.cpccOS.h
 *	Purpose:	Portable (cross-platform), light-weight, OS functions
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#ifdef _WIN32
    #include <Windows.h>
#endif
#include <vector>
#include <string>
#include "cpccUnicodeSupport.h"




class cpccOS 
{

public:

    
    static void getMainMonitorResolution(int &width, int &height);
    static cpcc_string getMainMonitorResolutionAsText(void);
    	
    
    // portable / cross platform C function for Windows, OSX returns the computer name
	static const cpcc_string getComputerName(void);
    static const cpcc_string getUserName(void);
    
    static cpcc_string& readProgramVersion(void);
    

	static long			getSystemMemory_inMb(void);

#ifdef __APPLE__
    static std::string readProgramVersionByPrincipalClass(const char *aClassName);
    static std::string getBundleID(void);
    #if !(TARGET_OS_IPHONE)
        static std::string getBundleIDfromAppName(const char *aAppName);
    #endif
    
#endif

    static cpcc_string getPreferredLanguage(void);

#ifdef _WIN32
    static const HWND getWindowHandleOfProgram(const TCHAR *aClassName);
    	
	
#endif

};
