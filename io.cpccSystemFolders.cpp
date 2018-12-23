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

// ToDo: move the functions into the header file and delete the .cpp and .mm files

// do not compile this if added in the xcode project files.
// the .mm must be included as well
#if defined(_WIN32) || defined (IMPORTED_BY_io_cpccSystemFolders_mm)


/*
    https://developer.apple.com/library/content/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html
UNIX-specific directories.
The directories in this category are inherited from traditional UNIX installations.
They are an important part of the system’s BSD layer but are more useful to software developers than end users.
Some of the more important directories that are hidden include:
    /bin—Contains essential command-line binaries. Typically, you execute these binaries from command-line scripts.
    /dev—Contains essential device files, such as mount points for attached hardware.
    /etc—Contains host-specific configuration files.
    /sbin—Contains essential system binaries.
    /tmp—Contains temporary files created by apps and the system.
    /usr—Contains non-essential command-line binaries, libraries, header files, and other data.
    /var—Contains log files and other files whose content is variable. (Log files are typically viewed using the Console app.)


MAC directories
On the Mac, the correct location to store persistent user-related files for your application is in a directory
with the same name as your application in the Application Support directory for the current user.
/Users/person/Library/Application Support/ExampleApp/
If you want to store user-related preferences, it is generally better to store them in the NSUserDefaults.
The correct way to get the path to the Application Support directory is to use the
NSSearchPathForDirectoriesInDomains function passing NSApplicationSupportDirectory for the search path
and NSUserDomainMask for the domain.
*/


#include <iostream>	 // for std::cerr

#include "io.cpccSystemFolders.h"
#include "core.cpccIdeMacros.h"


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
    #include <unistd.h>
#endif

#ifdef cpccTARGET_MACOS
        #include <Cocoa/Cocoa.h>
        #include <Carbon/Carbon.h> // for the DialogRef
#elif  defined(cpccTARGET_IOS)
        #include <Foundation/Foundation.h> // for NSLibraryDirectory

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
    
    // The directory returned by this method may not exist.
    // This method simply gives you the appropriate location for the requested directory.
    // Depending on the application’s needs, it may be up to the developer to create the appropriate directory and any in between.
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
