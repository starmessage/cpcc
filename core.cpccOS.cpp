
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

#include <iostream>
#include <sstream>
#include <mutex>
#include "core.cpccOS.h"
#include "core.cpccIdeMacros.h"


#ifdef _WIN32
	#include "core.cpccOSWin.h"

#elif __APPLE__

    #ifdef cpccTARGET_IOS
        #include <Foundation/Foundation.h>
        #include <UIKit/UIKit.h>
    #else 
	    #include <AppKit/AppKit.h>
    #endif

#endif


long cpccOS::getSystemMemory_inMb(void)
{
	#ifdef _WIN32
		return cpccOSWin::getSystemMemory_inMb();

	#elif __APPLE__
	
		return 0;
	#endif
}



/*
cpcc_string cpccOS::getOSNameVersionAndBuild(void)
{
#ifdef __APPLE__
    
    // In the documentation: "this string is not appropriate for parsing."
	NSString * operatingSystemVersionString = //    @"Версія 10.10.5 (складення 14F2511)";
        [[NSProcessInfo processInfo] operatingSystemVersionString];
    
    // returns: Version 10.12.6 (Build 16G29)
    // under ukrainian — 'uk_UA' locale it returns:
    // Версія 10.10.5 (складення 14F2511)
    if (!operatingSystemVersionString)
        return "Mac OS X Version ?";
    
    std::string result = [operatingSystemVersionString cStringUsingEncoding : NSUTF8StringEncoding];

    size_t pos = result.find("10.");
    if (pos!=std::string::npos)
       result.erase(0, pos);
    result.insert(0,"Mac OS X ");
    return result; // returns: Mac OS X 10.12.6 (Build 16G29)
    // todo: change the function to return just the version numbers. No build number ot other text
#else
	return cpccOSWin::getWindowsNameVersionAndBuild();
#endif
}
*/


cpcc_string cpccOS::getPreferredLanguage(void)
{
    
#ifdef _WIN32
    /*
     If you're asking about "Which language the OS menus and dialogs are dispalyed in" (i.e. which MUI - Multilingual User Interface kit - is installed), use the following:
     
     GetSystemDefaultUILanguage to get the original language of the system,

     GetUserDefaultUILanguage 
	 https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getuserdefaultuilanguage
	 to get the current user's selection,
     This function returns only a language identifier. 
	 An application can retrieve the language name using the GetUserPreferredUILanguages function.
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

	LANGID lang_id = GetUserDefaultUILanguage();
    return cpccOSWin::GetLangStringFromLangId(lang_id, true);
    
#elif __APPLE__
    NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
    return [language cStringUsingEncoding:NSUTF8StringEncoding];
#endif
    
} // End getPreferredLanguage


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




cpcc_string cpccOS::getMainMonitorResolutionAsText(void)
{
	cpcc_stringstream result;
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
    

#elif defined(cpccTARGET_IOS)
    width = [UIScreen mainScreen].bounds.size.width;
    height = [UIScreen mainScreen].bounds.size.height;
#elif defined(TARGET_OS_MAC)
    NSScreen *mainScreen = [NSScreen mainScreen];
    NSRect rect = [mainScreen frame];
    width = rect.size.width;
    height = rect.size.height;
#endif
}



#ifdef _WIN32



const HWND cpccOS::getWindowHandleOfProgram(const TCHAR *aClassName)
{
    // e.g. FindWindow("Notepad", "Untitled - Notepad");
    return FindWindow(aClassName, NULL);
}




#endif






    

const cpcc_string cpccOS::getUserName(void)
{
#ifdef _WIN32
    cpcc_char username[200];
    DWORD username_len = sizeof(username)-1;
    GetUserName(username, &username_len);
    return username;
#elif __APPLE__
    NSString *un = NSUserName();
    return [un UTF8String];
#endif
}


// portable / cross platform C function for Windows, OSX
// returns the computer name 
const cpcc_string cpccOS::getComputerName(void)
{
#ifdef _WIN32
    cpcc_char name[255]; DWORD size;
    size = sizeof(name) - 1;
    GetComputerName(name, &size);
    return name;
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

#if !(TARGET_OS_IPHONE)
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
#endif

std::string cpccOS::getBundleID(void)
{
    std::string result = [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
    return result;
}

#endif



cpcc_string& cpccOS::readProgramVersion(void)
{
    // static variable
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
    #if !(__has_feature(objc_arc))
        [tmpVersion release];
    #endif
    
#endif
    
    return ver;
}



#endif
