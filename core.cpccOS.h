
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

#include <vector>
#include "cpccUnicodeSupport.h"
#ifdef __APPLE__
    #import <IOKit/pwr_mgt/IOPMLib.h>
#endif

template<typename T1, typename T2>
struct cpccMonitorInfoT
{
	int id, left, top, right, bottom;
	T1 extra1;
	T2 extra2;
};


typedef std::vector<cpccMonitorInfoT<void *, void *> > cpccMonitorList;



class cpccOS
{
private:
#ifdef __APPLE__
    IOPMAssertionID preventSleepAssertionID;
#endif

public:
    cpccOS()
#ifdef __APPLE__
		: preventSleepAssertionID(0)
#endif
    { }
    

	/*	cross platform (windows and MAC OSX) function that finds and enumerates the monitors and their coordinates
		return value: number of monitors,
		list parameter: details about the monitors
	*/
	static unsigned long getListOfMonitors(cpccMonitorList &list);

	static void sleep(const unsigned int msec);
    
	// static void keepAwakeTrigger(void);
    
    // portable / cross platform C function for Windows, OSX returns the computer name
	static const cpcc_string getComputerName(void);
    
    const bool preventMonitorSleep(const cpcc_char *textualReason);
    const bool restoreMonitorSleep(void);
    
    
    static cpcc_string& readProgramVersion(void);
#ifdef __APPLE__
    static cpcc_string readProgramVersionByPrincipalClass(const cpcc_char *aClassName);
#endif


#ifdef _WIN32
	static const HWND getWindowHandleOfProgram(const cpcc_char *aClassName)
	{
		// FindWindow("Notepad", "Untitled - Notepad");
		return FindWindow(aClassName, NULL);
	}
#endif

};
