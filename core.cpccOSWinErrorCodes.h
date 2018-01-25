
/*  *****************************************
 *  File:		core.cpccOSWinErrorCodes.h
 *	Purpose:	Windows error text from GetLastError
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


#include <iostream>
#include <sstream>
#include <string>
#include <tchar.h>


class cpccOSWinErrorCodes
{
public:

	/// returns the textual version of the windows error code.
	/// Parameter: the number given by GetLastError()
	static std::basic_string<TCHAR> getText(const DWORD anErrorCode)
	{
		// some ready codes, because the win functions do not always find the error text
		switch(anErrorCode) 
		{
			case 0: return  _T("No error text for error code 0");
			//  WinINet errors
			// https://msdn.microsoft.com/en-us/library/aa385465(v=vs.85).aspx
			case 12002: return  _T("The request has timed out.");
			case 12004: return  _T("ERROR_INTERNET_INTERNAL_ERROR. An internal error has occurred.");
			case 12017: return  _T("ERROR_INTERNET_OPERATION_CANCELLED. The operation was canceled, usually because the handle on which the request was operating was closed before the operation completed.");
			case 12019: return  _T("ERROR_WINHTTP_INCORRECT_HANDLE_STATE. The requested operation cannot be carried out because the handle supplied is not in the correct state.");
			// encoding help: https://msdn.microsoft.com/en-us/library/aa383955(v=vs.85).aspx
			case 12175: return  _T("ERROR_INTERNET_DECODING_FAILED. WinINet failed to perform content decoding on the response.");
			
			// winHttpErrors
			// https://msdn.microsoft.com/en-us/library/windows/desktop/aa383770(v=vs.85).aspx


			// System Error Codes (4000-5999)
			// https://msdn.microsoft.com/en-us/library/windows/desktop/ms681387(v=vs.85).aspx
			case 4317: return  _T("ERROR_INVALID_OPERATION. The operation identifier is not valid.");
		}
		

		// find the error code text automatically
		// example at
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa387678(v=vs.85).aspx
		const int bufferSize = 500;
		TCHAR   lpBuffer[bufferSize] = _T("No text for this error code.");
		
		DWORD nChars = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,                 
										   NULL,                                      // No string to be formatted needed
										   anErrorCode,                               
										   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
										   lpBuffer,              // Put the message here
										   bufferSize - 1,        // Number of bytes to store the message
										   NULL);
			
		if (nChars == 0)
		{
			// The error code did not exist in the system errors.
			// Try Ntdsbmsg.dll for the error code.
			HINSTANCE hInst;

			// Load the library.
			hInst = LoadLibrary(_T("Ntdsbmsg.dll"));
			if (hInst == NULL)
				return  _T("No description of the error because getWindowsErrorText() could not load Ntdsbmsg.dll\n");
				
			
			// Try getting message text from ntdsbmsg.
			nChars = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
						hInst, anErrorCode, 0, lpBuffer, bufferSize - 1, NULL);

			// Free the library.
			FreeLibrary(hInst);
		}
			
		if (nChars == 0)
			return _T("FormatMessage() failed inside getWindowsErrorText()\n");
		
		return lpBuffer;
	}

	
	static std::basic_string<TCHAR> getDescription(const TCHAR *failedFunctionName, const DWORD anErrorCode)
	{
		std::basic_ostringstream<TCHAR>  s;
		s << failedFunctionName << _T("() failed.\nWindows error code ") << anErrorCode << _T(": ");
		s << getText(anErrorCode) << std::endl;
		return s.str();
	}


};



