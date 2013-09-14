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
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#ifndef cpcc_SelfTest_h
#define cpcc_SelfTest_h

#include "cpccUnicodeSupport.h"
#ifdef _WIN32
	#include <Windows.h>
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
 

#if (ENABLE_SELF_TESTS==1)
	#define SelftestVariableName aSelfTest
#else
	#define SelftestVariableName
#endif


class cpccQmsg
{
public:
	static void Qmsg(const cpcc_char *title, const cpcc_char *text)
	{
	#ifdef _WIN32
		OutputDebugString(title);
		OutputDebugString(_T(": "));
		OutputDebugString(text);
		OutputDebugString(_T("\n"));
		MessageBox( NULL, text, title, NULL);
	#elif defined(__APPLE__)
		std::cout << title << ": " << text << std::endl;
	#endif
	};
};


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