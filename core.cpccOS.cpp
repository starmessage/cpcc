
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
unsigned long cpccOS::getListOfMonitors(cpccMonitorList &list)
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
		
	unsigned long screenCount = [screenArray count];
	for (unsigned int index=0; index < screenCount; index++)
	{
  		NSScreen *screen = [screenArray objectAtIndex: index];
  		NSRect screenRect = [screen frame];
  		cpccMonitorInfoT<void *, void *> info;
		
        info.top	= (int) screenRect.origin.y;
        info.bottom = (int) (screenRect.origin.y + screenRect.size.height);
		info.left	= (int) screenRect.origin.x;
		info.right	= (int) (screenRect.origin.x + screenRect.size.width);
		list.push_back(info);
	}

#endif

	return list.size();
}


/*
void cpccOS::keepAwakeTrigger(void)
{
#ifdef _WIN32
	
#elif __APPLE__
 create an NSTimer that fires a function with this
 UpdateSystemActivity(OverallAct);
 
    // removed document:
    // https://developer.apple.com/library/mac/qa/qa1160/_index.html
    // 'UpdateSystemActivity' is deprecated: first deprecated in OS X 10.8
    // all these parameters stop the screensaver from running
	// UpdateSystemActivity(OverallAct);
    // UpdateSystemActivity(IdleActivity);
    // UpdateSystemActivity(HDActivity);
    // UpdateSystemActivity(UsrActivity);
    UpdateSystemActivity(NetActivity);
    
#else
    #error #8724f: Unknown platform for cpccOS
	
#endif
}
*/


const bool cpccOS::preventMonitorSleep(const cpcc_char *textualReason)
{
#ifdef _WIN32
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa373208%28v=vs.85%29.aspx
	return SetThreadExecutionState( ES_AWAYMODE_REQUIRED | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED) != NULL;
	

#elif __APPLE__
    // IOPMAssertionCreateWithName is new API available in Mac OS X 10.6 Snow Leopard. IOPMAssertionCreateWithName allows an application to return a return a brief string to the user explaining why that application is preventing sleep.
    //  https://developer.apple.com/library/mac/qa/qa1340/_index.html
    //  kIOPMAssertionTypePreventUserIdleDisplaySleep (only available on 10.7 or later)
    //  kIOPMAssertionTypeNoDisplaySleep - prevents display sleep AND idle sleep,
    //  kIOPMAssertionTypeNoIdleSleep - prevents idle sleep of the MAC (the screen can turn off)
    //  NOTE: IOPMAssertionCreateWithName limits the string to 128 characters.
    CFStringRef reasonForActivity;
    reasonForActivity = CFStringCreateWithCString(NULL, textualReason, kCFStringEncodingASCII);
    IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                                   kIOPMAssertionLevelOn,
                                                   reasonForActivity,
                                                   &preventSleepAssertionID);
    
    CFRelease(reasonForActivity);
    return (success == kIOReturnSuccess);

#else
    #error #8724: Unknown platform for cpccOS
	return false;
#endif
}


const bool cpccOS::restoreMonitorSleep()
{
#ifdef _WIN32
    // ToDo:
	return false; 
#elif __APPLE__
    if (preventSleepAssertionID==0) return
        false;
    IOReturn success = IOPMAssertionRelease(preventSleepAssertionID);
    preventSleepAssertionID =0 ;
    return (success == kIOReturnSuccess);
#else
    #error #8724: Unknown platform for cpccOS
	return false;
#endif
}



void cpccOS::sleep(const unsigned int msec)
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


#ifdef __APPLE__
cpcc_string cpccOS::readProgramVersionByPrincipalClass(const cpcc_char *aClassName)
{
    NSString *aNSClassName = [NSString stringWithCString: aClassName encoding:NSASCIIStringEncoding ];
    // NSString *tmpVersion = [[NSBundle bundleForClass: aNSClassName] objectForInfoDictionaryKey:@"CFBundleVersion"];
    NSString *tmpVersion = [[NSBundle bundleForClass: aNSClassName] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    
    cpcc_string ver = [tmpVersion UTF8String];
    // [tmpVersion release];
    return ver;
}
#endif


cpcc_string& cpccOS::readProgramVersion(void)
{
    static cpcc_string ver(_T("Version N/A"));
    if (ver!= _T("Version N/A"))
        return ver;
    
#ifdef __APPLE__
    
    // how to get the .saver bundle:
    // http://stackoverflow.com/questions/8490562/using-a-system-plug-in-saver-bundle-inside-a-cocoa-application-as-a-resource
    // http://stackoverflow.com/questions/17391380/macosx-screensaver-how-to-get-the-path-to-saver-bundle
    /*
     cpccFileSystemMini fs;
     
     NSString *appBundlePath = [NSString stringWithCString:fs.getAppBundlePath().c_str() encoding:NSASCIIStringEncoding];
     NSBundle *bundle = [NSBundle bundleWithPath:appBundlePath ] ;
     NSString *tmpVersion = [bundle objectForInfoDictionaryKey:@"CFBundleVersion"];
     [bundle release];
     //[appBundlePath release]; // already deallocated
     */
    
    NSString *tmpVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
    ver = [tmpVersion UTF8String];
    [tmpVersion release];
    
    
#endif
    
    return ver;
}



#endif