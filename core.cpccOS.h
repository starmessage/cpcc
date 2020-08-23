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
    #include <Windows.h>
#endif
#include <vector>
#include <string>
#include "cpccUnicodeSupport.h"

// /////////////////////////////
//
//  class cpccOS declaration
//
// /////////////////////////////


class cpccOS 
{
private:
    #ifdef __APPLE__
        static void util_RemoveSuffixFromString(std::string &str, const char* suffixToRemove);
    #endif
    
public:

    // todo: move it to core.cpccHardware.h
    static void getMainMonitorResolution(int &width, int &height);
    static cpcc_string getMainMonitorResolutionAsText(void);
    	
    
    // portable / cross platform C function for Windows, OSX returns the computer name
	static const cpcc_string getComputerName(void);
    static const cpcc_string getUserName(void);
    
    static cpcc_string& readProgramVersion(void);
    

#ifdef __APPLE__
    static std::string readProgramVersionByPrincipalClass(const char *aClassName);
    static std::string getBundleID(void);
    
    #if !(TARGET_OS_IPHONE)
        static std::string getBundleIDfromAppName(const char *aAppName);
    #endif
    
#endif

    static cpcc_string getPreferredLanguage(void);

#ifdef _WIN32
    static const HWND getWindowHandleOfProgram(const TCHAR *aClassName);
    	
	
#endif

};

// /////////////////////////////
//
//  class cpccOS implementation
//
// /////////////////////////////

inline cpcc_string cpccOS::getMainMonitorResolutionAsText(void)
{
    cpcc_stringstream result;
    int mw, mh;
    getMainMonitorResolution(mw, mh);
    result << mw;
    result << "x";
    result << mh;
    return result.str();
}

#ifdef __APPLE__
    inline void cpccOS::util_RemoveSuffixFromString(std::string &str, const char* suffixToRemove)
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
#endif

#ifdef _WIN32

    inline const HWND cpccOS::getWindowHandleOfProgram(const TCHAR* aClassName)
    {
        // e.g. FindWindow("Notepad", "Untitled - Notepad");
        return FindWindow(aClassName, NULL);
    }



#endif



