/*  *****************************************
 *  File:       types.cpccWideCharSupport.h
 *  *****************************************
 *  Library:    Cross Platform C++ Classes (cpcc)
 *  Copyright:  2020 StarMessage software.
 *  License:    Free for opensource projects.
 *              Commercial license exists for closed source projects.
 *  Web:        http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:   https://github.com/starmessage/cpcc
 *  email:      sales -at- starmessage.info
 *  *****************************************
 */

#pragma once

/*
    purpose:
    include or define the _T and TCHAR macros for both standard and wide char programs to use for their strings
    e.g.
    TCHAR                    instead of char
    std::basic_string<TCHAR> instead of std::string;
*/

// more reading at
// https://accu.org/journals/overload/21/116/steinbach_1842/

#ifdef    _WIN32
    #include <tchar.h>
#endif
#include <string>

#ifdef __APPLE__    // define the _T() macro that is a MS VC macro
    #ifndef _T
        #define _T(s) s
    #endif
    #ifndef TCHAR
        #define  TCHAR char
    #endif
#endif

#ifdef UNICODE 
    #define TOSTRING_WN(v) std::to_wstring(v) 
    // #define STR_WN L
#else 
    #define TOSTRING_WN(v) std::to_string(v) 
    // #define STR_WN  
#endif 
