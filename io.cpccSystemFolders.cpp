/*  *****************************************
 *  File:		io.cpccSystemFolders.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

// do not compile this if added in the xcode project files.
// the .mm must be included as well
#if defined(_WIN32) || defined (IMPORTED_BY_io_cpccSystemFolders_mm)

#include <iostream>	 // for std::cerr
#include "io.cpccSystemFolders.h"


#ifdef _WIN32
	#include	<io.h> // for _access on windows
	#include	<Shlobj.h>
	#include	<Shlwapi.h>
	#pragma comment(lib,"Shlwapi.lib") // instruct visual studio to link the library
	#include	<tchar.h>
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<direct.h>
#elif defined(__APPLE__)
    #include <CoreFoundation/CoreFoundation.h>
	#include <Carbon/Carbon.h> // for the DialogRef
	#include <unistd.h>

#endif


cpcc_string cpccSystemFolders::getFolder_CurrentDir(void)
{
    
    #ifdef _WIN32
		#ifdef UNICODE
			#define getcwd  _wgetcwd 
		#else
			#define getcwd  _getcwd 
		#endif
    #else
        
    
    #endif
    
    cpcc_char buff[FILENAME_MAX];
	cpcc_char *result = getcwd( buff, FILENAME_MAX );
    
    if (!result)
        return _T("");
    else
        return result;
}


cpcc_string  cpccSystemFolders::getFolder_CommonAppData(void) 
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];

	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g. C:\ProgramData
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath))) 
		{
		// std::string result(szPath);
		cpcc_string result(szPath);
		return result;
		}
	std::cerr << "Error #6531 in getFolder_CommonAppData\n";

#elif defined(__APPLE__)
	// https://developer.apple.com/library/mac/documentation/General/Conceptual/MOSXAppProgrammingGuide/AppRuntime/AppRuntime.html#//apple_ref/doc/uid/TP40010543-CH2-SW9

    /*
     https://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Miscellaneous/Foundation_Constants/index.html#//apple_ref/doc/c_ref/NSSearchPathDomainMask
      NSSearchPathDirectory :
        NSApplicationDirectory = 1,             /Applications
        NSDemoApplicationDirectory,
        NSDeveloperApplicationDirectory,
        NSAdminApplicationDirectory,
        NSLibraryDirectory,                     /Library
        NSDeveloperDirectory,
        NSUserDirectory,                        /Users
        NSDocumentationDirectory,
        NSDocumentDirectory,                    /Users/username/Documents
                                                sandboxed: 
                                                /Users/username/Library/Containers/com.yourcompany.YourApp/Documents
     
        NSCoreServiceDirectory,                 System/Library/CoreServices
        NSAutosavedInformationDirectory = 11,   Library/Autosave Information
        NSDesktopDirectory = 12,
        NSCachesDirectory = 13,                 Library/Caches
        NSApplicationSupportDirectory = 14,     Library/Application Support
        NSDownloadsDirectory = 15,
        NSInputMethodsDirectory = 16,
        NSMoviesDirectory = 17,
        NSMusicDirectory = 18,
        NSPicturesDirectory = 19,
        NSPrinterDescriptionDirectory = 20,
        NSSharedPublicDirectory = 21,           ~/Public
        NSPreferencePanesDirectory = 22,        Library/PreferencePanes
        NSApplicationScriptsDirectory = 23,     ~/Library/Application Scripts/<code-signing-id>
        NSItemReplacementDirectory = 99,
        NSAllApplicationsDirectory = 100,       /Applications/Demos
        NSAllLibrariesDirectory = 101,
        NSTrashDirectory = 102
     
     NSSearchPathDomainMask:
        NSUserDomainMask = 1,
        NSLocalDomainMask = 2,
        NSNetworkDomainMask = 4,
        NSSystemDomainMask = 8,
     */
    
    // todo:
    return  std::string("/users/shared/");
    
    /*
    std::string ph;
    
    // NSApplicationSupportDirectory is read-only
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSLocalDomainMask, YES);
    if ([paths count]>0)
    {
        ph = [[paths objectAtIndex:0] UTF8String];
        ph.append("/Preferences");
    }
    
	// std::cout << "getFolder_CommonAppData() returned:" << ph << std::endl;
    return ph;
     */


#else
	assert(false && "Error #5735: unsupported platform for getFolder_AppData()");	
#endif	
	cpcc_string emptyResult;
	return emptyResult;
}


cpcc_string cpccSystemFolders::getFolder_UserData(void) 
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g C:\Users\JohnSmith\AppData\Roaming
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) 
		{
		cpcc_string result(szPath);
		return result;
		}
	std::cerr << "Error #6531 in getFolder_AppData::getFolder_UserData\n";
	
#elif defined(__APPLE__)
    std::string ph;
    
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    if ([paths count]>0)
    {
        ph = [[paths objectAtIndex:0] UTF8String];
        ph.append("/Preferences");
    }
    
    //std::cout << "getFolder_UserData() returned:" << ph << std::endl;
    return ph;
    // cpccPathHelper ph;
	// return ph.pathCat(getFolder_UserHome().c_str(), "/Library/Preferences/");
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_AppData()");	
#endif	
	cpcc_string emptyResult;
	return emptyResult;
}




#endif  // of #if defined(_WIN32) || defined (IMPORTED_BY_io_cpccFileSystemMini_mm)
