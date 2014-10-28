/*  *****************************************
 *  File:		cpccUnicode.h
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
 
 
 /*
	example of using std::string in both Unicode and Non-Unicode
	typedef std::basic_string<TCHAR> tstring;
	tstring(_T("Hello"));
	
	defines:
	UNICODE is used by Windows headers
	_UNICODE is used by C-runtime/MFC headers
	
	The _T macro is identical to the _TEXT macro.

	If you writing portable code:
	1st Never use wchar_t it is nor portable and its encoding is not well 
	defined between platforms (utf-16 windows/utf-32 all others).
	Never use TChar, use plain std::string encoded as UTF-8.
	When dealing with Brain Damaged Win32 API just convert UTF-8 string to 
	UTF-16 before calling it.

	LPSTR = char*
	LPCSTR = const char*
	LPWSTR = wchar_t*
	LPCWSTR = const wchar_t*
	LPTSTR = char* or wchar_t* depending on _UNICODE
	LPCTSTR = const char* or const wchar_t* depending on _UNICODE
	
	-------------------------------------------------------------
	
	example of converting from ascii to wide char
	std::wstring A_to_W (const char* p)
	{
    	std::wstring   ws ;
	    if (p)
    	{
        	setlocale (LC_CTYPE, "") ;

        	std::vector<wchar_t>  buf (strlen(p)+1, 0) ;
        	mbstowcs (&buf[0], p, buf.size()-1) ;
        	ws = &buf[0] ;
    	}
    	return ws ;
	}
	
 */
#pragma once

#include <string>
#include <iostream>

#ifdef	_WIN32
	#include <tchar.h>
	#include <windows.h>
#endif


#ifdef __APPLE__	// define the _T() macro that is a MS VC macro
	#ifdef UNICODE
		#define _T(s) L ## s
	#else
		#define _T(s) s
	#endif
#endif


// http://unicode.org/faq/utf_bom.html#BOM		
#define UTF16_BOM	"\xff\xfe"
#define UTF8_BOM	"\xEF\xBB\xBF"

#ifdef UNICODE

	#define		cpcc_cout			std::wcout 		
	#define		cpcc_cerr			std::wcerr 		
	#define		cpcc_sprintf		wsprintf		
	#define		cpcc_fopen			_wfopen
	#define		cpcc_strlen			_tcslen
	#define		cpcc_strftime		_tcsftime	
	#define		cpcc_fprintf		_ftprintf
	#define		cpcc_strtok			_tcstok
	#define		cpcc_strcpy			_tcscpy

	typedef		wchar_t				cpcc_char;
	typedef		std::wifstream		cpcc_ifstream;
	typedef		std::wostringstream cpcc_ostringstream;
	typedef		std::wstringstream	cpcc_stringstream;
	typedef		std::wstring		cpcc_string;
 #else 
	#define		cpcc_cout			std::cout 	
	#define		cpcc_cerr			std::cerr
	#define		cpcc_sprintf		sprintf	
	#define		cpcc_fopen			fopen
	#define		cpcc_strlen			strlen
	#define		cpcc_strftime		strftime	
	#define		cpcc_fprintf		fprintf
	#define		cpcc_strtok			strtok
	#define		cpcc_strcpy			strcpy

	typedef		char				cpcc_char;
	typedef		std::ifstream		cpcc_ifstream;
	typedef		std::ostringstream	cpcc_ostringstream;
	typedef		std::stringstream	cpcc_stringstream;
	typedef		std::string			cpcc_string;
 #endif
 