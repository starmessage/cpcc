/*  *****************************************
 *  File:       fs.cpccSystemFolders.h
 *  *****************************************
 *  Library:    Cross Platform C++ Classes (cpcc)
 *  Copyright:  2019 StarMessage software.
 *  License:    Free for opensource projects.
 *              Commercial license exists for closed source projects.
 *  Web:        http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:   https://github.com/starmessage/cpcc
 *  email:    sales -at- starmessage.info
 *  *****************************************
 */

#pragma once

/*
    Notes: On MAC, add also the .mm file in your sources to compile
*/

/*
    https://developer.apple.com/library/content/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html
UNIX-specific directories.
The directories in this category are inherited from traditional UNIX installations.
They are an important part of the system’s BSD layer but are more useful to software developers than end users.
Some of the more important directories that are hidden include:
    /bin — Contains essential command-line binaries. Typically, you execute these binaries from command-line scripts.
    /dev — Contains essential device files, such as mount points for attached hardware.
    /etc — Contains host-specific configuration files.
    /sbin — Contains essential system binaries.
    /tmp — Contains temporary files created by apps and the system.
    /usr — Contains non-essential command-line binaries, libraries, header files, and other data.
    /var — Contains log files and other files whose content is variable. (Log files are typically viewed using the Console app.)


MAC directories
    On the Mac, the correct location to store persistent user-related files for your application is in a directory
    with the same name as your application in the Application Support directory for the current user.
    /Users/person/Library/Application Support/ExampleApp/
    If you want to store user-related preferences, it is generally better to store them in the NSUserDefaults.
    The correct way to get the path to the Application Support directory is to use the
    NSSearchPathForDirectoriesInDomains function passing NSApplicationSupportDirectory for the search path
    and NSUserDomainMask for the domain.
*/

#include <string>
#include <stdio.h>


#ifdef    _WIN32
    #include <tchar.h>
    #include <windows.h>
	#include <Shlobj.h> // for SHGetFolderPath()
	#include <direct.h> // for _getcwd
	#pragma comment(lib,"Shell32.lib") // instruct visual studio to link the library
#else
    #include <pwd.h>    // for getpwent()
    #include <unistd.h> // for getcwd()
#endif


#ifdef __APPLE__    // define the _T() macro that is a MS VC macro
    #ifndef _T
        #define _T(s) s
    #endif
    #ifndef TCHAR
        #define  TCHAR char
    #endif

    
#endif

// #include "alpha/cpccTesting.h" // circular reference

    
////////////////////////////////////////////////
// class cpccSystemFolders 
////////////////////////////////////////////////



class cpccSystemFolders
{
public:
    
    static std::basic_string<TCHAR> getCommonAppData(std::basic_string<TCHAR>* err = NULL);

    static std::basic_string<TCHAR> getFontsDir(std::basic_string<TCHAR>* err = NULL);
    // static std::basic_string<TCHAR> getSystemsTempDir(std::basic_string<TCHAR>* err = NULL);
#ifdef _WIN32
    static std::basic_string<TCHAR> getWindowsDir(std::basic_string<TCHAR>* err = NULL);
#endif
};

    
//////////////////////////////////////////
// class cpccSystemFolders implementation
//////////////////////////////////////////    

#ifdef _WIN32

    
	inline std::basic_string<TCHAR> cpccSystemFolders::getWindowsDir(std::basic_string<TCHAR>* err)
	{
		TCHAR buffer[MAX_PATH];
		UINT rc = GetWindowsDirectory(buffer, MAX_PATH-1);
		if ((rc == 0) || (rc >= MAX_PATH))
		{
			if (err)
				err->append(_T("c:\\error-no3852-getting-windows-dir"));
			return _T("");
		}
		return buffer;
	}

    
    
    /*
    inline std::basic_string<TCHAR> cpccSystemFolders::getSystemsTempDir(std::basic_string<TCHAR>* err)
    {
        // std::error_code errCode;
        // return std::filesystem::temp_directory_path(errCode);
        TCHAR buffer[MAX_PATH + 1];
        GetTempPath(MAX_PATH, buffer); // this is the user's temp
        return buffer;
    }
    */

#endif // _WIN32
    
   

inline std::basic_string<TCHAR> cpccSystemFolders::getFontsDir(std::basic_string<TCHAR>* err)
{
#ifdef _WIN32
    TCHAR szPath[MAX_PATH];

    // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, szPath)))
        return szPath;
    
    if (err)
        err->append(_T("Error #4824 in cpccFileSystemMini::getFolder_Fonts\n"));
    return _T("");

#elif defined(__APPLE__)
    return  "/library/fonts/";
#endif	
}



inline std::basic_string<TCHAR> cpccSystemFolders::getCommonAppData(std::basic_string<TCHAR>* err) 
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];

	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g. C:\ProgramData
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath))) 
		{
		// cpcc_string result(szPath);
		return szPath;
		}

    if (err)
        err->append(_T("Error #6531 in getFolder_CommonAppData\n"));

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
	#pragma Error #5735: unsupported platform for getFolder_AppData

#endif	
	return _T("");
}




