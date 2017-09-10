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
	#include <windows.h>
	#include <tchar.h>
	// TCHAR is either char or wchar_t, so a
	// typedef basic_string<TCHAR>   tstring;
#endif
#include <vector>
#include <string>


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
    static std::string getMainMonitorResolutionAsText(void);
    
	static void sleep(const unsigned int msec);
    
    // portable / cross platform C function for Windows, OSX returns the computer name
	static const std::string getComputerName(void);
    static const std::string getUserName(void);
    
    static std::string& readProgramVersion(void);
    
#ifdef _WIN32
    static std::string getWindowsVersionNumber(void);
#endif
    
    static std::string GetFileVersion( const char* aFilePath);
    static std::string getOSnameAndVersion(void);
    static std::string getOSnameVersionAndBuildNumber(void);
	static bool			is64bit(void);
    
#ifdef __APPLE__
    static std::string readProgramVersionByPrincipalClass(const char *aClassName);
    static std::string getBundleIDfromAppName(const char *aAppName);
    static std::string getBundleID(void);
#endif

    static std::string getPreferredLanguage(void);

#ifdef _WIN32
    static const HWND getWindowHandleOfProgram(const TCHAR *aClassName);
    
	/// returns the textual verison of the windows error code.
	/// Parameter: the number given by GetLastError()
    static std::string getWindowsErrorText(const DWORD anErrorCode);
	static std::string getWindowsErrorCodeAndText(const char *failedFunctionName, const DWORD anErrorCode);

#endif

};
