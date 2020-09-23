/*  *****************************************
 *  File:		core.cpccOSWin.h
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

#include "core.cpccOSversion.h"
#include <windows.h>
#include <tchar.h> 	// TCHAR is either char or wchar_t, so a typedef basic_string<TCHAR>   tstring;

#include <string>
#include <sstream>
#include <iostream>

#pragma comment(lib, "Version.lib") // for GetFileVersionInfo
#pragma comment(lib, "user32.lib") // for GetSystemInfo


class cpccOSWin
{

public:

	static cpcc_string GetLangStringFromLangId(DWORD dwLangID_i, bool returnShortCode)
	{
		// http://ntcoder.com/bab/2007/12/06/how-to-convert-language-ids-like-1033-to-english-united-states-form/
		const int MAX_LANG_LEN = 81;

		// Prepare LCID
		const LCID lcidLang = MAKELCID(dwLangID_i, SORT_DEFAULT);

		// Will hold language
		TCHAR szLangBuffer[MAX_LANG_LEN] = { 0 };

		LCTYPE resultFormat = (returnShortCode ? LOCALE_SISO639LANGNAME : LOCALE_SENGLANGUAGE);
		// Get language
		// https://docs.microsoft.com/en-us/windows/desktop/intl/locale-siso-constants
		DWORD dwCount = GetLocaleInfo(lcidLang, resultFormat, szLangBuffer, MAX_LANG_LEN);
		if (!dwCount)
		{
			// AfxTrace(_T("Failed to get locale language information"));
			return _T("");
		}

		// Will hold country
		TCHAR szCountryBuffer[MAX_LANG_LEN] = { 0 };
		resultFormat = (returnShortCode ? LOCALE_SISO3166CTRYNAME : LOCALE_SENGCOUNTRY);

		// Get country
		dwCount = GetLocaleInfo(lcidLang, resultFormat, szCountryBuffer, MAX_LANG_LEN);

		if (!dwCount)
		{
			// AfxTrace(_T("Failed to get locale country information"));
			return szLangBuffer;
		}// End if

		cpcc_string combinedResult(szLangBuffer);
		combinedResult += _T("-");
		combinedResult += szCountryBuffer;
		return combinedResult;
	}	// End GetLangStringFromLangId




	
    /*
	static const cpcc_string getWindowsVersionMajorMinor(void) // returns something like: "6.1"
	{
		cpcc_string  result = getWindowsFullVersionNumber();	// returns something like: "6.1.7601.23864"
		// std::cout << "inside getWindowsVersionMajorMinor(), point 1, result= " << result << std::endl;

		// remove the last two fragments
		std::size_t found = result.rfind(_T(".")); // remove everything after the last .
		if (found != std::string::npos)
			result.erase(found);

		found = result.rfind(_T(".")); // remove everything after the last .
		if (found != std::string::npos)
			result.erase(found);

		// std::cout << "inside getWindowsVersionMajorMinor(), point 2, result= " << result << std::endl;
		return result;
	}
    */

#ifdef NOT_NEEDED
	static const cpcc_string getWindowsNameVersionAndBuild(void)
	{

		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724439%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
		// GetVersion may be altered or unavailable for releases after Windows 8.1. Instead, use the Version Helper functions
		// GetVersionEx may be altered or unavailable for releases after Windows 8.1. Instead, use the Version Helper functions

		// all the helpers:
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724429(v=vs.85).aspx

		// table of version numbers
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx

		// Note that a 32-bit application can detect whether it is running under WOW64 by calling the IsWow64Process function.
		// It can obtain additional processor information by calling the GetNativeSystemInfo function.

        sOsVerComponents kernVer = cpccOSversion::getKernelVersionComponents();
        cpcc_stringstream ssKernVer;
        ssKernVer << kernVer.verMajor << _T(".") << kernVer.verMinor;

		cpcc_string result(ssKernVer.str());
		// std::cout << "getOSNameAndVersion() point 1, os=" << result << std::endl;
		if (result.compare(_T("10.0")) == 0)
			result = _T("Windows 10"); // or Windows Server 2016
		else
			if (result.compare(_T("6.3")) == 0)
				result = _T("Windows 8.1"); // Windows Server 2012 R2
			else
				if (result.compare(_T("6.2")) == 0)
					result = _T("Windows 8"); // or Windows Server 2012
				else
					if (result.compare(_T("6.1")) == 0)
						result = _T("Windows 7"); // or Windows Server 2008 R2
					else
						if (result.compare(_T("6.0")) == 0)
							result = _T("Windows Vista");
						else
							if (result.compare(_T("5.2")) == 0)
								result = _T("Windows XP 64bit"); // or Windows server 2003
							else
								if (result.compare(_T("5.1")) == 0) // 5.1.2600 -> with SP3
									result = _T("Windows XP");
								else
									if (result.compare(_T("5.0")) == 0)
										result = _T("Windows 2000");
									else result = _T("Windows unknown");


		// must be in reverse order
		/*
		char *ver;
		getWindowsFullVersionNumber

		if (IsWindows10OrGreater())
		ver = "Win 10 or newer";
		else
		if (IsWindows8Point1OrGreater())
		ver = "Win 8.1 or newer";
		else
		if (IsWindows8OrGreater())
		ver = "Win 8";
		else
		if (IsWindows7SP1OrGreater())
		ver = "Win 7 SP1";
		else
		if (IsWindows7OrGreater())
		ver = "Win 7";
		else
		if (IsWindowsVistaSP2OrGreater())
		ver = "Win Vista SP2";
		else
		if (IsWindowsVistaSP1OrGreater())
		ver = "Win Vista SP1";
		else
		if (IsWindowsVistaOrGreater())
		ver = "Win Vista";
		else
		if (IsWindowsXPSP3OrGreater())
		ver = "Win XP SP3";
		else
		if (IsWindowsXPSP2OrGreater())
		ver = "Win XP SP2";
		else
		if (IsWindowsXPSP1OrGreater())
		ver = "Win XP SP1";
		else
		if (IsWindowsXPOrGreater())
		ver = "Win XP";
		else
		ver = "Win 2000 or older";
		*/


		if (cpccOSversion::is64bit())
			result += _T(", 64bit");
		else
			result += _T(", 32bit");

		result += _T(" (") + cpccOSversion::getMajorMinorPatchVersionStr() + _T(")");
		// std::cout << "getOSNameAndVersion() point 2, os=" << result << std::endl;

		return result;

	}

#endif


    

};
