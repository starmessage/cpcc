
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

#ifdef _DEBUG
	#define cpccDEBUG
#endif

#ifdef DEBUG
    #define cpccDEBUG
#else
    #ifdef __APPLE__
	    //#ifndef NDEBUG
		//    #define cpccDEBUG
	    //#endif
    #endif
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
		return "Undetected compliler version";
	}
};
