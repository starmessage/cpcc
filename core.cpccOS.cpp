
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
		#error File core.cpccOS.cpp must be compiled as obc++ under MAC. Did you include the core.cpccOS.cpp instead of core.cpccOS.mm ?
	#endif
#endif


#include "core.cpccOS.h"
#include <iostream>
#include <sstream>
#include <mutex>

#ifdef _WIN32
	# pragma warning (disable: 4005)
	#include <windows.h>
    // #include <Lmcons.h> / /for GetUserName()
    // #include <VersionHelpers.h> // for version check
    #pragma comment(lib, "user32.lib") // for GetSystemInfo
    #pragma comment(lib, "Version.lib") // for GetFileVersionInfo
#elif __APPLE__
	// #include <Cocoa/Cocoa.h>
    #include <AppKit/AppKit.h>
    
#endif


#ifdef _WIN32

/*
 https://msdn.microsoft.com/en-us/library/ms724429(VS.85).aspx
To obtain the full version number for the operating system, call the GetFileVersionInfo function 
 on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to obtain the 
 \\StringFileInfo\\<lang><codepage>\\ProductVersion 
 subblock of the file version information.
*/
std::string cpccOS::GetFileVersion( const char* aFilePath)
{

    // https://stackoverflow.com/questions/940707/how-do-i-programmatically-get-the-version-of-a-dll-or-exe-file
    DWORD  verHandle = 0;
	UINT   size      = 0;
	LPBYTE lpBuffer  = NULL;
	DWORD  verSize   = GetFileVersionInfoSize(aFilePath, &verHandle);

	std::string result;
	if (verSize == 0)
		return result;

	LPSTR verData = new char[verSize];
	
    if (GetFileVersionInfo(aFilePath, verHandle, verSize, verData))
    {
       	if (VerQueryValue(verData,"\\",(VOID FAR* FAR*)&lpBuffer,&size))
       	{
           	if (size)
           	{
                VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
                if (verInfo->dwSignature == 0xfeef04bd)
                {

                    // Doesn't matter if you are on 32 bit or 64 bit,
                    // DWORD is always 32 bits, so first two revision numbers
                    // come from dwFileVersionMS, last two come from dwFileVersionLS
                    std::stringstream ssresult;
					ssresult << (( verInfo->dwFileVersionMS >> 16 ) & 0xffff );
					ssresult << ".";
					ssresult << (( verInfo->dwFileVersionMS) & 0xffff);
					ssresult << ".";
					ssresult << (( verInfo->dwFileVersionLS >>  16 ) & 0xffff);
					ssresult << ".";
					ssresult << (( verInfo->dwFileVersionLS) & 0xffff);
    				result = ssresult.str();
				}
			}
        }
    }
	delete[] verData;
	return result;
}



/// returns the full versoion and build number, e.g. for windows 10: "10.0.15063.296"
const std::string &cpccOS::getWindowsFullVersionNumber(void)
{
    static std::mutex _mutex;
    static std::string cached_result;
    
    std::lock_guard<std::mutex> autoMutex(_mutex);
    if (cached_result.length()==0)
        cached_result =GetFileVersion("kernel32.dll");
    return cached_result;
}



const std::string &cpccOS::getWindowsShortVersionNumber(void)
{
	static std::string cached_result;
	if (cached_result.length() == 0)
	{
		cached_result = getWindowsFullVersionNumber();	// returns something like: "6.1.7601.23864"

		// remove the last two fragments
		std::size_t found = cached_result.rfind("."); // remove everything after the last .
		if (found != std::string::npos)
			cached_result.erase(found);
		found = cached_result.rfind("."); // remove everything after the last .
		if (found != std::string::npos)
			cached_result.erase(found);
	}
	return cached_result;
}


#endif


std::string cpccOS::getOSnameAndVersion(void)
{
#ifdef _WIN32
	return getOSnameVersionAndBuildNumber();

#elif __APPLE__
    std::string result = getOSnameVersionAndBuildNumber();
    // e.g.: Mac OS X 10.12.6 (Build 16G29)
    
    std::size_t found = result.rfind(" ("); // remove build number
    if (found!=std::string::npos)
        result.erase(found);
    return result;
    
#endif
    
}

bool cpccOS::is64bit(void)
{
    #ifdef _WIN32
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724423%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
        //  PROCESSOR_ARCHITECTURE_AMD64 -> x64 (AMD or Intel)
        
        SYSTEM_INFO siSysInfo;
        // To retrieve accurate information for an application running on WOW64, call the GetNativeSystemInfo function.
        //GetSystemInfo(&siSysInfo);
    
        // Copy the hardware information to the SYSTEM_INFO structure.
        GetNativeSystemInfo(&siSysInfo);
        // PROCESSOR_ARCHITECTURE_AMD64 means x64 (AMD or Intel)
        return ( siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
        
    
    #elif __APPLE__
    
        return true;
    
    #endif
    
}


std::string cpccOS::getOSnameVersionAndBuildNumber(void)
{
#ifdef _WIN32
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724439%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
    // GetVersion may be altered or unavailable for releases after Windows 8.1. Instead, use the Version Helper functions
    // GetVersionEx may be altered or unavailable for releases after Windows 8.1. Instead, use the Version Helper functions
    
    // all the helpers:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724429(v=vs.85).aspx
    
    // table of version numbers
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx
    
    // Note that a 32-bit application can detect whether it is running under WOW64 by calling the IsWow64Process function.
    // It can obtain additional processor information by calling the GetNativeSystemInfo function.
    
	std::string result(getWindowsShortVersionNumber());
	if (result.compare("10.0") == 0)
		result = "Windows 10"; // or Windows Server 2016
	else
		if (result.compare("6.3") == 0)
			result = "Windows 8.1"; // Windows Server 2012 R2
		else
			if (result.compare("6.2") == 0)
				result = "Windows 8"; // or Windows Server 2012
			else
				if (result.compare("6.1") == 0)
					result = "Windows 7"; // or Windows Server 2008 R2
				else
					if (result.compare("6.0") == 0)
						result = "Windows Vista"; 
					else
						if (result.compare("5.2") == 0)
							result = "Windows XP 64bit"; 
						else
							if (result.compare("5.1") == 0)
								result = "Windows XP";
							else
								if (result.compare("5.0") == 0)
									result = "Windows 2000";

	
    // must be in reverse order
	/*
	char *ver;
	getWindowsFullVersionNumber

    if (IsWindows10OrGreater())
        ver = "Win 10 or newer";
    else 
    if (IsWindows8Point1OrGreater())
        ver = "Win 8.1 or newer";
    else
    if (IsWindows8OrGreater())
        ver = "Win 8";
    else
    if (IsWindows7SP1OrGreater())
        ver = "Win 7 SP1";
    else
    if (IsWindows7OrGreater())
        ver = "Win 7";
    else
    if (IsWindowsVistaSP2OrGreater())
        ver = "Win Vista SP2";
    else
    if (IsWindowsVistaSP1OrGreater())
        ver = "Win Vista SP1";
    else
    if (IsWindowsVistaOrGreater())
        ver = "Win Vista";
    else
    if (IsWindowsXPSP3OrGreater())
        ver = "Win XP SP3";
    else
    if (IsWindowsXPSP2OrGreater())
        ver = "Win XP SP2";
    else
    if (IsWindowsXPSP1OrGreater())
        ver = "Win XP SP1";
    else
    if (IsWindowsXPOrGreater())
        ver = "Win XP";
    else
        ver = "Win 2000 or older";
    */

    
    
    if (is64bit())
        result+=", 64bit";
    else
        result+=", 32bit";
    
	result += " (" + getWindowsFullVersionNumber() + ")";
    return result;
    
    
#elif __APPLE__
    NSString * operatingSystemVersionString = [[NSProcessInfo processInfo] operatingSystemVersionString];
    // returns: Version 10.12.6 (Build 16G29)
    std::string result = [operatingSystemVersionString cStringUsingEncoding:NSASCIIStringEncoding];
    result.erase(0, 8);
    result.insert(0,"Mac OS X ");
    return result; // returns: Mac OS X 10.12.6 (Build 16G29)
    
#endif

    
}


std::string cpccOS::getPreferredLanguage(void)
{
    
#ifdef _WIN32
    /*
     If you're asking about "Which language the OS menus and dialogs are dispalyed in" (i.e. which MUI - Multilingual User Interface kit - is installed), use the following:
     
     GetSystemDefaultUILanguage to get the original language of the system,
     GetUserDefaultUILanguage to get the current user's selection,
     This function returns only a language identifier. An application can retrieve the language name using the GetUserPreferredUILanguages function.
     EnumUILanguages to see which languages are available.

     GetUserPreferredUILanguages
     https://msdn.microsoft.com/en-us/library/windows/desktop/dd318139(v=vs.85).aspx
     
     GetUserPreferredUILanguages(MUI_LANGUAGE_NAME
     */
    
    // from https://msdn.microsoft.com/en-us/library/windows/apps/jj244362(v=vs.105).aspx
	/*
    ULONG numLanguages = 0;
    WCHAR pwszLanguagesBuffer[50];
    DWORD cchLanguagesBuffer = sizeof(pwszLanguagesBuffer) -1;
    BOOL hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, &pwszLanguagesBuffer, &cchLanguagesBuffer);
    if (hr)
        return std::string(pwszLanguagesBuffer);
		*/

    return std::string("en-US");
    
#elif __APPLE__
    NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
    return [language cStringUsingEncoding:NSASCIIStringEncoding];
#endif
    
}


static void util_RemoveSuffixFromString(std::string &str, char* suffixToRemove)
{
    // std::cout << "to remove " << suffixToRemove << " from " << str << std::endl;
    const size_t pos = str.rfind(suffixToRemove);
    if (pos == std::string::npos)
        return; // not found
    // std::cout << "Found " << suffixToRemove << " in " << str << std::endl;
    
    if (pos != str.length() - strlen(suffixToRemove) )
        return; // text found but not at the end of the string
    
    str.erase(pos , std::string::npos );
}


std::string cpccOS::getMainMonitorResolutionAsText(void)
{
    std::stringstream result;
    int mw,mh;
    getMainMonitorResolution(mw, mh);
    result << mw;
    result << "x";
    result << mh;
    return result.str();
}


void cpccOS::getMainMonitorResolution(int &width, int &height)
{
#ifdef _WIN32
    /*
     Size of the primary monitor: GetSystemMetrics SM_CXSCREEN / SM_CYSCREEN (GetDeviceCaps can also be used)
     Size of all monitors (combined): GetSystemMetrics SM_CX/YVIRTUALSCREEN
     Size of work area (screen excluding taskbar and other docked bars) on primary monitor: SystemParametersInfo SPI_GETWORKAREA
     Size of a specific monitor (work area and "screen"): GetMonitorInfo
     
     Remember that a monitor does not always "begin" at 0x0. 
     MonitorFromWindow to find the monitor your window is on and then call GetMonitorInfo.
     */
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
    
#elif __APPLE__
    NSScreen *mainScreen = [NSScreen mainScreen];
    NSRect rect = [mainScreen frame];
    width = rect.size.width;
    height = rect.size.height;
#endif
}



#ifdef _WIN32
static BOOL CALLBACK util_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	//lprcMonitor holds the rectangle that describes the monitor position and resolution)
	if (!lprcMonitor)
		return false;

	cpccMonitorList *listPtr = (cpccMonitorList *)dwData;
	cpccMonitorInfoT info;

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

const HWND cpccOS::getWindowHandleOfProgram(const TCHAR *aClassName)
{
    // e.g. FindWindow("Notepad", "Untitled - Notepad");
    return FindWindow(aClassName, NULL);
}


std::string cpccOS::getWindowsErrorText(const DWORD anErrorCode)
{
	// some ready codes, because the win functions do not always find the error text
	switch(anErrorCode) 
	{
		case 0: return  std::string("No error text for error code 0");
		//  WinINet errors
		// https://msdn.microsoft.com/en-us/library/aa385465(v=vs.85).aspx
		case 12002: return  std::string("The request has timed out.");
		case 12004: return  std::string("ERROR_INTERNET_INTERNAL_ERROR. An internal error has occurred.");
		case 12019: return  std::string("ERROR_WINHTTP_INCORRECT_HANDLE_STATE. The requested operation cannot be carried out because the handle supplied is not in the correct state.");

		// winHttpErrors
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa383770(v=vs.85).aspx


		// System Error Codes (4000-5999)
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms681387(v=vs.85).aspx
		case 4317: return  std::string("ERROR_INVALID_OPERATION. The operation identifier is not valid.");


	}
	

	// find the error code text automatically
	// example at
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa387678(v=vs.85).aspx
    const int bufferSize = 500;
    TCHAR   lpBuffer[bufferSize] = _T("No text for this error code.");
    
    DWORD nChars = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,                 
                                       NULL,                                      // No string to be formatted needed
                                       anErrorCode,                               
                                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
                                       lpBuffer,              // Put the message here
                                       bufferSize - 1,        // Number of bytes to store the message
                                       NULL);
        
    if (nChars == 0)
	{
		// The error code did not exist in the system errors.
		// Try Ntdsbmsg.dll for the error code.
		HINSTANCE hInst;

		// Load the library.
		hInst = LoadLibrary(_T("Ntdsbmsg.dll"));
		if (hInst == NULL)
			return  std::string("getWindowsErrorText() did not find the error description because it could not load Ntdsbmsg.dll\n");
			
		
		// Try getting message text from ntdsbmsg.
		nChars = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
					hInst, anErrorCode, 0, lpBuffer, bufferSize - 1, NULL);

		// Free the library.
		FreeLibrary(hInst);
	}
		
	if (nChars == 0)
		return std::string("FormatMessage() failed inside getWindowsErrorText()\n");
    
    return std::string(lpBuffer);
}


std::string cpccOS::getWindowsErrorCodeAndText(const char *failedFunctionName, const DWORD anErrorCode)
{
	std::ostringstream s;
	s << failedFunctionName << "() failed.\nWindows error code " << anErrorCode << ": ";
	s << getWindowsErrorText(anErrorCode) << std::endl;
	return s.str();
}


#endif



	/*	cross platform (windows and MAC OSX) function that finds and enumerates the monitors and their coordinates
		return value: number of monitors,
		list parameter: details about the monitors
	*/
size_t cpccOS::getListOfMonitors(cpccMonitorList &list)
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
  		cpccMonitorInfoT info;
		
        info.top	= (int) screenRect.origin.y;
        info.bottom = (int) (screenRect.origin.y + screenRect.size.height);
		info.left	= (int) screenRect.origin.x;
		info.right	= (int) (screenRect.origin.x + screenRect.size.width);
		list.push_back(info);
	}

#endif

	return list.size();
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
    

const std::string cpccOS::getUserName(void)
{
#ifdef _WIN32
    char username[200];
    DWORD username_len = sizeof(username)-1;
    GetUserName(username, &username_len);
    return std::string(username);
#elif __APPLE__
    NSString *un = NSUserName();
    return std::string([un UTF8String]);
#endif
}


// portable / cross platform C function for Windows, OSX
// returns the computer name 
const std::string cpccOS::getComputerName(void)
{
#ifdef _WIN32
    char name[255]; DWORD size;
    size = sizeof(name) - 1;
    GetComputerName(name, &size);
    return std::string(name);
#endif
#ifdef __APPLE__
    char name[_POSIX_HOST_NAME_MAX + 1];
    if (gethostname(name, sizeof name) == -1 )
        return std::string("getComputerName failed.");
        
    // remove the .local or .lan from the computer name as reported by OSX
    std::string ComputerNameTrimmed(name);
        
    util_RemoveSuffixFromString(ComputerNameTrimmed, (char *)".lan");
    util_RemoveSuffixFromString(ComputerNameTrimmed, (char *)".local");
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
std::string cpccOS::readProgramVersionByPrincipalClass(const char *aClassName)
{
    NSString *aNSClassName = [NSString stringWithCString: aClassName encoding:NSASCIIStringEncoding ];
    // NSString *tmpVersion = [[NSBundle bundleForClass: aNSClassName] objectForInfoDictionaryKey:@"CFBundleVersion"];
    NSString *tmpVersion = [[NSBundle bundleForClass: aNSClassName] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    
    std::string ver = [tmpVersion UTF8String];
    // [tmpVersion release];
    return ver;
}


std::string cpccOS::getBundleIDfromAppName(const char  *aAppName)
{
    // [[NSBundle mainBundle] bundleIdentifier]
    

    // return an application's Bundle Identifier for a named application:
    NSString *appName = [NSString stringWithCString: aAppName encoding:NSASCIIStringEncoding ];
    
    NSWorkspace * workspace = [NSWorkspace sharedWorkspace];
    NSString * appPath = [workspace fullPathForApplication:appName];
    std::string result;
    
    if (appPath) {
            NSBundle * appBundle = [NSBundle bundleWithPath:appPath];
            NSString *bundleID = [appBundle bundleIdentifier];
            result = [bundleID UTF8String];
        }
    return result;
}


std::string cpccOS::getBundleID(void)
{
    std::string result = [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
    return result;
}

#endif



std::string& cpccOS::readProgramVersion(void)
{
    static std::string ver("Version N/A");
    if (ver!= "Version N/A")
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
