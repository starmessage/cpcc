
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

#include "cpccUnicodeSupport.h"



	
class cpccOS
{
private:
    static void util_RemoveSuffixFromString( cpcc_string &str, cpcc_char* suffixToRemove )
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
    
    
public:
	static void sleep(const int msec)	
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
    static const cpcc_string getComputerName(void)
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
    
};
