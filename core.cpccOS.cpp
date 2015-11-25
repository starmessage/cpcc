
/*  *****************************************
 *  File:		core.cpccOS.cpp
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



// do not compile this if added in the xcode project files.
// the .mm must be included as well
#if defined(_WIN32) || defined (IMPORTED_BY_core_cpccOS_mm)


#ifdef __APPLE__
	#if !defined __OBJC__
		#error File core.cpccOS.cpp must be compiled as obc++ under MAC. Did you include the cpccColor.cpp instead of cpccColor.mm ?
	#endif
#endif


#include "core.cpccOS.h"
#ifdef _WIN32
	# pragma warning (disable: 4005)
	#include <windows.h>
	# pragma warning (default: 4005)

#elif __APPLE__
	// #include <Cocoa/Cocoa.h>
    #include <AppKit/AppKit.h>
#endif



#ifdef _WIN32
static BOOL CALLBACK util_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	//lprcMonitor holds the rectangle that describes the monitor position and resolution)
	if (!lprcMonitor)
		return false;

	cpccMonitorList *listPtr = (cpccMonitorList *)dwData;
	cpccMonitorInfoT<void *, void *> info;

	info.bottom = lprcMonitor->bottom;
	info.top = lprcMonitor->top;
	info.left = lprcMonitor->left;
	info.right = lprcMonitor->right;
	listPtr->push_back(info);

	/*
	LPMONITORINFO lpmi;	MONITORINFOEX info;
	info.cbSize = sizeof(info);
	if (!::GetMonitorInfo(hMonitor, (LPMONITORINFO)&info))
	return FALSE;  // stop enumeration if error

	((cpccMonitorList *)dwData).
	monitors.Add(info);
	return TRUE;
	*/

	return TRUE;
}
#endif

	

static void util_RemoveSuffixFromString(cpcc_string &str, cpcc_char* suffixToRemove)
{
    // std::cout << "to remove " << suffixToRemove << " from " << str << std::endl;
    const size_t pos = str.rfind(suffixToRemove);
    if (pos == std::string::npos)
        return; // not found
    // std::cout << "Found " << suffixToRemove << " in " << str << std::endl;
        
    if (pos != str.length() - cpcc_strlen(suffixToRemove) )
        return; // text found but not at the end of the string
        
    str.erase(pos , std::string::npos );
}
    
    


	/*	cross platform (windows and MAC OSX) function that finds and enumerates the monitors and their coordinates
		return value: number of monitors,
		list parameter: details about the monitors
	*/
int cpccOS::getListOfMonitors(cpccMonitorList &list)
{
	list.clear();

#ifdef _WIN32
	/*	Explaation of coordinates in multiple monitors under windows
		http://www.flounder.com/virtual_screen_coordinates.htm

		EnumDisplayMonitors()
		https://msdn.microsoft.com/en-us/library/dd162610%28v=vs.85%29.aspx

	*/
	if (!EnumDisplayMonitors(NULL, NULL, util_MonitorEnumProc, (LPARAM) &list))
		return 0;

#elif __APPLE__
	/*	sample code:
		CGDisplayCount nDisplays;
		CGGetActiveDisplayList(0,0, &nDisplays);
		log.printf("Displays connected: %d",(int)nDisplays);
	*/

	NSArray *screenArray = [NSScreen screens];
		
	unsigned screenCount = [screenArray count];
	for (unsigned int index=0; index < screenCount; index++)
	{
  		NSScreen *screen = [screenArray objectAtIndex: index];
  		NSRect screenRect = [screen frame];
  		cpccMonitorInfoT<void *, void *> info;
		
        info.top	= screenRect.origin.y;
        info.bottom = screenRect.origin.y + screenRect.size.height;
		info.left	= screenRect.origin.x;
		info.right	= screenRect.origin.x + screenRect.size.width;
		list.push_back(info);
	}

#endif

	return list.size();
}


void cpccOS::sleep(const int msec)
{
#ifdef _WIN32
	Sleep(msec);
#elif __APPLE__
	usleep(1000* msec);
#else
	#error #8724: Unknown platform for cpccOS
#endif
}
    
    
    // portable / cross platform C function for Windows, OSX
    // returns the computer name of the computer
const cpcc_string cpccOS::getComputerName(void)
{
#ifdef _WIN32
    cpcc_char name[255]; DWORD size;
    size = sizeof(name) - 1;
    GetComputerName(name, &size);
    return cpcc_string(name);
#endif
#ifdef __APPLE__
    cpcc_char name[_POSIX_HOST_NAME_MAX + 1];
    if (gethostname(name, sizeof name) == -1 )
        return cpcc_string("getComputerName failed.");
        
    // remove the .local or .lan from the computer name as reported by OSX
    cpcc_string ComputerNameTrimmed(name);
        
    util_RemoveSuffixFromString(ComputerNameTrimmed, (cpcc_char *)".lan");
    util_RemoveSuffixFromString(ComputerNameTrimmed, (cpcc_char *)".local");
    return ComputerNameTrimmed;
        
    /*
        [[NSDictionary
        dictionaryWithContentsOfFile:@"/var/db/SystemConfiguration/preferences.xml"
        ] valueForKeyPath:@"System.System.ComputerName"];
        */
        
    /*
        [[NSProcessInfo processInfo] hostName]
        */
#endif
        
}
    

#endif