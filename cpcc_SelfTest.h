/*  *****************************************
 *  File:		cpcc_SelfTest.h
 *  Version:	1.0
 *	Purpose:	Portable (cross-platform), light-weight, unit self-checking
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#ifndef cpcc_SelfTest_h
#define cpcc_SelfTest_h

#include "cpccUnicodeSupport.h"
#ifdef _WIN32
	#include <Windows.h>
	#include <sstream>
#elif defined(__APPLE__)
	#include <iostream>
#endif



/// Central switch to close all selfTests in your application
#define ENABLE_SELF_TESTS	1

/**
	How to use the self test macro:
	1) Start with SELFTEST_BEGIN and a unique label
	2) add the self test code (can be a static function of your class under test
	2) Close with SELFTEST_END
	Your testing code will run when your application starts
	
	Example:
	SELFTEST_BEGIN(cpccFileSystemMini_SelfTest)
		// MessageBox(NULL, "selftest of cpccFileSystemMini"	, NULL, NULL);
		cpccFileSystemMini::selfTest();
	SELFTEST_END

 */
 
/// consolePut() is used to allow console output inside visual studio.
/// In OSX, xcode, the command std::cout << "something" 
/// send the text in the console window. This does not happen in visual studio (if creating a windows app)
/// This macro is used to fix this inconvenience, that is an annoying inconsistency when you develop cross platform applications
#ifdef _WIN32
/*
    class consoleRedirect
    {
    public:
		~consoleRedirect(void) 	{ OutputDebugString(s.str().c_str()); 
									// OutputDebugString("\n");  }

		std::ostringstream s;

		static std::ostringstream& put(void)	{  consoleRedirect a; return a.s; }
    };
*/


	#define consolePut(x)	{	std::ostringstream s;  s << x;     \
								OutputDebugString(s.str().c_str()); \
								OutputDebugString("\n"); \
							}
#else
	#define consolePut(x)	std::cout << x << "\n";
	
#endif


#if (ENABLE_SELF_TESTS==1)
	#define SelftestVariableName aSelfTest
#else
	#define SelftestVariableName
#endif



#define SELFTEST_BEGIN(SelfTestUniqueName)	\
	namespace SelfTestUniqueName {			\
	class TmioanSelfTest					\
	{										\
	public:									\
		TmioanSelfTest()					\
		{ 




#define SELFTEST_END			\
		};						\
	} SelftestVariableName; }






#endif // cpcc_SelfTest_h