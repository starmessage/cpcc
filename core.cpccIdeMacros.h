
/*  *****************************************
 *  File:		cpccDefines.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2018 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

 // https://msdn.microsoft.com/en-us/library/b0084kay.aspx
 // Visual Studio: _DEBUG Defined as 1 when the /LDd, /MDd, or /MTd compiler option is set. Otherwise, undefined.
 // NDEBUG disables standard-C assertions
 // Use them when apropriate, 
 // ie _DEBUG if you want your debugging code to be consistent with the MS CRT debugging techniques and 
 // NDEBUG if you want to be consistent with assert().

// xcode: In iOS, in debug mode, the DEBUG preprocessor directive is defined
//        In MacOS, DEBUG=1

#ifdef _DEBUG
	#define cpccDEBUG
#endif

#ifdef DEBUG
    #define cpccDEBUG
#endif

#ifdef NDEBUG
    #undef cpccDEBUG
#endif

#ifdef __APPLE__
    #ifndef NDEBUG  // in xcode put NDEBUG in the release configuration
        // #define cpccDEBUG
    #endif
#endif


// //////////////////////////////////
// 32 vs 64 bits
// //////////////////////////////////

// Check windows
#if _WIN32 || _WIN64
   #if _WIN64
     #define COMPILED64BIT
  #else
    #define COMPILED32BIT
  #endif
#endif

// This works for MSVC++ and g++ :
// #if defined(_M_X64) || defined(__amd64__)

// Check GCC
#if __GNUC__
  #if __x86_64__ || __ppc64__
    #define COMPILED64BIT
  #else
    #define COMPILED32BIT
  #endif
#endif
// http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
// suggests __LP64__ and _LP64

/* another way for all compilers

// C
#include <stdint.h>

// C++
#include <cstdint>

#if INTPTR_MAX == INT64_MAX
// 64-bit
#elif INTPTR_MAX == INT32_MAX
// 32-bit
#else
#error Unknown pointer size or missing size macros!
#endif

*/

/*
 From TargetConditionals.h:
 
 TARGET_OS_WIN32           - Generated code will run under 32-bit Windows
 TARGET_OS_UNIX            - Generated code will run under some Unix (not OSX)
 TARGET_OS_MAC             - Generated code will run under Mac OS X variant
    TARGET_OS_IPHONE          - Generated code for firmware, devices, or simulator
        TARGET_OS_IOS             - Generated code will run under iOS
        TARGET_OS_TV              - Generated code will run under Apple TV OS
        TARGET_OS_WATCH           - Generated code will run under Apple Watch OS
    TARGET_OS_SIMULATOR      - Generated code will run under a simulator
    TARGET_OS_EMBEDDED       - Generated code for firmware
 
 TARGET_IPHONE_SIMULATOR   - DEPRECATED: Same as TARGET_OS_SIMULATOR
 TARGET_OS_NANO            - DEPRECATED: Same as TARGET_OS_WATCH
 
      +------------------------------------------------+
      |                TARGET_OS_MAC                   |
      | +---+  +-------------------------------------+ |
      | |   |  |          TARGET_OS_IPHONE           | |
      | |OSX|  | +-----+ +----+ +-------+ +--------+ | |
      | |   |  | | IOS | | TV | | WATCH | | BRIDGE | | |
      | |   |  | +-----+ +----+ +-------+ +--------+ | |
      | +---+  +-------------------------------------+ |
      +------------------------------------------------+

 */

// target platform

#ifdef _WIN32  // covers 32-bit and 64-bit
    #define cpccTARGET_WINDOWS
    #ifdef _WIN64
        #define cpccTARGET_WINDOWS64
    #else
        #define cpccTARGET_WINDOWS32
    #endif
#endif


#if __APPLE__   // This macro is defined in any Apple computer.
    #include "TargetConditionals.h"

    #if TARGET_OS_IPHONE
        #define cpccTARGET_IOS
    #elif TARGET_OS_MAC
        #define cpccTARGET_MACOS
    #else
        #error "cpccIdeMacros: Unknown Apple platform"
    #endif
#endif

#if defined(ANDROID) || defined(__ANDROID__)  // both needed
    #define cpccTARGET_ANDROID
#endif



class cppcIDE
{
public:

	// http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
	static const char *getCompilerVersion(void)
	{
	#if __cplusplus == 201703L 
		return "2017 C++ standard";
	#endif

	#if __cplusplus == 201402L
		return "2014 C++ standard";
	#endif

	#if __cplusplus == 201103L 
		return "2011 C++ standard";
	#endif

	#if __cplusplus == 199711L 
		return "1998 C++ standard";
	#endif

	#ifdef  __STDC_VERSION__
		return __STDC_VERSION__;
	#endif
		return "Undetected compiler version";
	}
};
