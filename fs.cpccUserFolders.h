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
    https://developer.apple.com/library/content/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html


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
// class cpccUserFolders
////////////////////////////////////////////////

class cpccUserFolders
{
public:
    static std::basic_string<TCHAR> getUserHomeDir(std::basic_string<TCHAR>* err = NULL);
    static std::basic_string<TCHAR> getUserData(std::basic_string<TCHAR>* err = NULL);
    static std::basic_string<TCHAR> getDesktop(std::basic_string<TCHAR>* err = NULL);
    static std::basic_string<TCHAR> getUsersCacheDir(std::basic_string<TCHAR>* err = NULL);
    static std::basic_string<TCHAR> getUsersTempDir(std::basic_string<TCHAR>* err = NULL);
    static std::basic_string<TCHAR> getCurrentDir(std::basic_string<TCHAR>* err = NULL);

};


    
//////////////////////////////////////////
// class cpccUserFolders implementation
//////////////////////////////////////////    

#ifdef _WIN32

    inline std::basic_string<TCHAR> cpccUserFolders::getUsersTempDir(std::basic_string<TCHAR>* err)
    {
        TCHAR buffer[MAX_PATH + 1];
        GetTempPath(MAX_PATH, buffer);	// this is the user's temp
        return  buffer;
    }

    inline std::basic_string<TCHAR>  cpccUserFolders::getUsersCacheDir(std::basic_string<TCHAR>* err)
    {
        // is there no cache folder in windows. Return the user's temp
        return  getUsersTempDir(err);
    }


    inline std::basic_string<TCHAR> cpccUserFolders::getDesktop(std::basic_string<TCHAR> *err)
    {
            TCHAR szPath[MAX_PATH];
        
            // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szPath)))
                return szPath;
        
            if (err)
                *err = _T("Error #4231a in getDesktopFolder()");
        
            return _T("");
    }


    inline std::basic_string<TCHAR> cpccUserFolders::getUserData(std::basic_string<TCHAR>* err)
    {
        TCHAR szPath[MAX_PATH];
        
        // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
        // e.g C:\Users\JohnSmith\AppData\Roaming
        if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) 
            {
            return szPath;
            }

        if (err)
            err->append(_T("Error #6531 in getFolder_AppData::getFolder_UserData\n"));

        return _T("");
    }
    




#endif // _WIN32
    
   



inline std::basic_string<TCHAR> cpccUserFolders::getUserHomeDir(std::basic_string<TCHAR>* err)
{
#ifdef _WIN32
    TCHAR szPath[MAX_PATH];

    // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
    // e.g. C:\ProgramData
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, szPath)))
        return szPath;
    
    if (err)
        err->append(_T("Error #6531 in getFolder_UserHome()\n"));
    return _T("");

#else
    const char* home = getpwent()->pw_dir;
    if (home)
        return home;
    return "";

    /*
    struct passwd* pwd = getpwuid(getuid());
    if (pwd)
        return std::string(pwd->pw_dir);
    return  std::string("~");
    */
#endif	
    
}



inline std::basic_string<TCHAR> cpccUserFolders::getCurrentDir(std::basic_string<TCHAR>* err)
{
    
    #ifdef _WIN32
		#ifdef UNICODE
			#define getcwd  _wgetcwd 
		#else
			#define getcwd  _getcwd 
		#endif
    #endif
    
    TCHAR buff[FILENAME_MAX];
	TCHAR *result = getcwd( buff, FILENAME_MAX );
    
    if (result)
        return result;

    if (err)
        err->append(_T("Error #6531b in getCurrentDir()\n"));

    return _T("");
}





