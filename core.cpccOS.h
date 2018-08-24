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

struct cpccMonitorInfoT
{
	int left, top, right, bottom;
};


typedef std::vector<cpccMonitorInfoT> cpccMonitorList;


class cpccOS 
{

public:
    
	/*	cross platform (windows and MAC OSX) function that finds and enumerates the monitors and their coordinates
		return value: number of monitors,
		list parameter: details about the monitors
	*/
	static size_t getListOfMonitors(cpccMonitorList &list);

    
    static void getMainMonitorResolution(int &width, int &height);
    static cpcc_string getMainMonitorResolutionAsText(void);
    
	static void sleep(const unsigned int msec);
    
    // portable / cross platform C function for Windows, OSX returns the computer name
	static const cpcc_string getComputerName(void);
    static const cpcc_string getUserName(void);
    
    static cpcc_string& readProgramVersion(void);
    
    static cpcc_string	getOSNameAndVersion(void);
    static cpcc_string	getOSNameVersionAndBuild(void);
	static bool			is64bit(void);
	static long			getSystemMemory_inMb(void);

#ifdef __APPLE__
    static std::string readProgramVersionByPrincipalClass(const char *aClassName);
    static std::string getBundleIDfromAppName(const char *aAppName);
    static std::string getBundleID(void);
#endif

    static cpcc_string getPreferredLanguage(void);

#ifdef _WIN32
    static const HWND getWindowHandleOfProgram(const TCHAR *aClassName);
    
	
	
#endif

};
