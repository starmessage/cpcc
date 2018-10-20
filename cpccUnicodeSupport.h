/*  *****************************************
 *  File:		cpccUnicodeSupport.h
 *  Version:	1.0
 *	Purpose:	Portable (cross-platform), light-weight, unit self-checking
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
 /*

	recommendations:
		If you need to work with UTF-8 in Windows - Save your source file in UTF-8 with BOM
		Use the setlocale function to correctly configure the runtime. This configuration depends on your need, don't simply call setlocale(LC_ALL, "")
		if you're using WinAPI, you shouldn't be using standard lib file I/O because the standard lib is not Unicode friendly.
		if text is going in an output file, you should use a Unicode encoding, such as UTF-8 or UTF-16.

		For writing new programs prefer to avoid using legacy codepages entirely and instead use char as UTF-8, 
		and then on Windows to use the *W APIs unconditionally, converting between UTF-8 char strings and wchar_t at the API boundary.

		wstring is available anywhere that has C++ because it's in the standard library, but it's useless on UNIX since UNIX is UTF-8.
		Windows does not have native support for UTF-8. If you want proper Unicode support, you need to use the wchar_t version of 
		the Windows API functions, not the char version.
		Character encoding is rather painful when Windows doesn't support UTF-8 and Un*x doesn't support UTF-16.
		Suggestion:  use UTF-8 as your main encoding and convert as necessary on Windows.

		Convert UTF-8 (std::string for Mac/Unix) to UTF-18 (std::wstring, for Windows)
		http://en.cppreference.com/w/cpp/locale/wstring_convert/from_bytes


	On Windows, TCHAR is either wchar_t or plain char depending on your project settings.
	Likewise, _tprintf is either wprintf of printf to match the choice of characters.
	On OSX you will likely have to make this choice yourself, perhaps
	#define _tprintf    wprintf


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
	

	// Linux isn't exactly "agnostic" to Unicode -- it does recognize Unicode but the standard library functions assume UTF-8 encoding,
	// so Unicode strings fit into standard char arrays.
	// Windows, on the other hand, uses UTF-16 encoding, so you need a wchar_t array to represent 16-bit characters.
	
	On  Unix/Linux system there is no point in looking for a wchar_t version of getcwd. 
	That is because all file names and directory names are just bytes with the exception of the '/' and '\0' characters. 
	You can use UTF-8 encoding with them but the filesystem itself does not care.

 */
#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>

#ifdef	_WIN32
	#include <tchar.h>
	#include <windows.h>
#endif


#ifdef __APPLE__	// define the _T() macro that is a MS VC macro
	#ifdef UNICODE  // UNICODE is a Windows only define. So the next code is not needed.
	//	#define _T(s) L ## s
	//	#define  TCHAR wchar_t
	#else
		#define _T(s) s
		#define  TCHAR char
	#endif
#endif

// http://unicode.org/faq/utf_bom.html#BOM		
#define UTF16_BOM	"\xFF\xFE"  // UTF-16, little-endian
#define UTF8_BOM	"\xEF\xBB\xBF"

/* reminder of how TCHAR is defined in Windows

#ifdef _UNICODE
	typedef wchar_t TCHAR;
#else
	typedef char TCHAR;
#endif

*/
typedef 	TCHAR							cpcc_char;
typedef		std::basic_string<TCHAR>		cpcc_string;
typedef		std::basic_stringstream<TCHAR>	cpcc_stringstream;
typedef		std::basic_ostringstream<TCHAR> cpcc_ostringstream;
typedef		std::basic_istringstream<TCHAR> cpcc_istringstream;
typedef		std::basic_ifstream<TCHAR>		cpcc_ifstream;
typedef		std::basic_ofstream<TCHAR>		cpcc_ofstream;


#ifdef UNICODE

    #define		cpcc_cin			std::wcin 
	#define		cpcc_cout			std::wcout 		
	#define		cpcc_cerr			std::wcerr 	
	#define		cpcc_thread			std::wthread
	
	// The wsprintf function has a surprising deficiency: it does not handle floating-point parameters.
	// #define		cpcc_sprintf		wsprintf		
	// #define		cpcc_sprintf		swprintf		
	#define		cpcc_sprintf		_stprintf
	#define		cpcc_fprintf		fwprintf	// _ftprintf
	#define		cpcc_fscanf			fwscanf
	#define		cpcc_fopen			_wfopen
	#define		cpcc_strlen			_tcslen
	//  stricmp, wcsicmp: These POSIX functions are deprecated. Use the ISO C++ conformant _stricmp, _wcsicmp,
	#define		cpcc_stricmp		_wcsicmp
	#define		cpcc_strnicmp		_wcsnicmp
	#define		cpcc_strftime		_tcsftime	

	#define		cpcc_strtok			_tcstok
	#define		cpcc_strcpy			_tcscpy
	#define		cpcc_strstr			wcsstr
	#define		cpcc_strtol			wcstol
	#define		cpcc_strtod			wcstod
    
	#define		cpcc_tolower		towlower
	// #define		cpcc_getline		getline
	#define		cpcc_stat			_wstat
	typedef		struct _stat		cpcc_struct_stat;
	
 #else 

    #define		cpcc_cin			std::cin
	#define		cpcc_cout			std::cout 	
	#define		cpcc_cerr			std::cerr
	#define		cpcc_thread			std::thread
	#define		cpcc_sprintf		sprintf	
	#define		cpcc_fprintf		fprintf
	#define		cpcc_fscanf			fscanf
	#define		cpcc_fopen			fopen
	#define		cpcc_strlen			strlen
	#define		cpcc_stricmp		_stricmp
	#define		cpcc_strnicmp		_strnicmp
	#define		cpcc_strftime		strftime	
	
	#define		cpcc_strtok			strtok
	#define		cpcc_strcpy			strcpy
	#define		cpcc_strstr			strstr
	#define		cpcc_strtol			strtol
	#define		cpcc_strtod			strtod
    
	#define		cpcc_tolower		tolower
	// #define		cpcc_getline		getline
	#define		cpcc_stat			stat
	typedef		struct stat			cpcc_struct_stat;

 #endif
 


#ifdef _WIN32

	// helper class for Windows, to convert from char * to wchar_t 
	class wchar_from_char
	{
	private:
		std::wstring	m_wstr= L"";
		const wchar_t	*m_resultPtr;

	public:

		explicit wchar_from_char(const char *str): m_resultPtr(NULL)
		{
			if (!str)
				return;
			
			setlocale(LC_CTYPE, "");
			size_t len = strlen(str);
			// allocate memory
			m_wstr.insert(0, len + 4, L'-');
			#pragma warning(suppress : 4996)
			mbstowcs(&m_wstr[0], str, len +2);
			m_resultPtr = m_wstr.c_str();
		}

		inline  operator const wchar_t *(void) const { return m_resultPtr; }

		const wchar_t *get(void) const { return m_resultPtr; }
	};

#endif
