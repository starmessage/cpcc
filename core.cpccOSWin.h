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



	static const long getSystemMemory_inMb(void)
	{
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366589%28v=vs.85%29.aspx
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		// If the function fails, the return value is zero. To get extended error information, call GetLastError.
		if (GlobalMemoryStatusEx(&statex))
			return (long) (statex.ullTotalPhys / (1024 *1024)); // ullTotalPhys: The amount of actual physical memory, in bytes.

		return 0;
	}

	/// returns the full versoion and build number, e.g. for windows 10: "10.0.15063.296"
	static const cpcc_string getWindowsFullVersionNumber(void)
	{
		/*
			TCHAR bufferSystemFolder[600];
			int lenNeeded = GetSystemDirectory(bufferSystemFolder, sizeof(bufferSystemFolder));
			if (lenNeeded >= sizeof(bufferSystemFolder))
			bufferSystemFolder[0] = 0;

			// there might be multiple versions of kernel32.dll in the system, so do not rely on windows to return the proper one by searching candidate folders.
			TCHAR fullpathnameKernel32[sizeof(bufferSystemFolder) + 20];
			sprintf_s(fullpathnameKernel32, "%s\\%s", bufferSystemFolder, "kernel32.dll");

			// static std::mutex _mutex;
			// std::lock_guard<std::mutex> autoMutex(_mutex);
			// std::cout << "getWindowsFullVersionNumber point 4" << std::endl;

			std::string result(GetFileVersion(fullpathnameKernel32));
		*/

			/* wrong results in XP PRO 64bit
			std::string result(GetFileVersion( "kernel32.dll"));
			std::cout << "Inside getWindowsFullVersionNumber(), OS: " << result << std::endl;
			cached_result = result; // fails in XP Pro win64
			*/

		// std::cout << "Inside getWindowsFullVersionNumber(), cached_result: " << cached_result << std::endl;

		return GetFileVersion(_T("kernel32.dll"));
	}
	

	static const cpcc_string getWindowsShortVersionNumber(void) // returns something like: "6.1"
	{
		cpcc_string  result = getWindowsFullVersionNumber();	// returns something like: "6.1.7601.23864"
		// std::cout << "inside getWindowsShortVersionNumber(), point 1, result= " << result << std::endl;

		// remove the last two fragments
		std::size_t found = result.rfind(_T(".")); // remove everything after the last .
		if (found != std::string::npos)
			result.erase(found);

		found = result.rfind(_T(".")); // remove everything after the last .
		if (found != std::string::npos)
			result.erase(found);

		// std::cout << "inside getWindowsShortVersionNumber(), point 2, result= " << result << std::endl;
		return result;
	}


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

		cpcc_string result(cpccOSWin::getWindowsShortVersionNumber());
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


		if (is64bit())
			result += _T(", 64bit");
		else
			result += _T(", 32bit");

		result += _T(" (") + getWindowsFullVersionNumber() + _T(")");
		// std::cout << "getOSNameAndVersion() point 2, os=" << result << std::endl;

		return result;

	}

	static const cpcc_string GetFileVersion( const TCHAR* aFilePath)
		/*
		https://msdn.microsoft.com/en-us/library/ms724429(VS.85).aspx
		To obtain the full version number for the operating system, call the GetFileVersionInfo function
		on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to obtain the
		\\StringFileInfo\\<lang><codepage>\\ProductVersion
		subblock of the file version information.
		*/
	{
		// https://stackoverflow.com/questions/13626295/how-to-use-the-getfileversioninfo-function
		// https://stackoverflow.com/questions/940707/how-do-i-programmatically-get-the-version-of-a-dll-or-exe-file
		// DWORD  verHandle = 0;
		UINT   size = 0;
		LPBYTE lpBuffer = NULL;
		DWORD  verSize = GetFileVersionInfoSize(aFilePath, NULL);

		cpcc_string result;
		if (verSize == 0)
			return result;

		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(aFilePath, NULL, verSize, verData))
		{
			if (VerQueryValue(verData, _T("\\"), (VOID FAR* FAR*)&lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{

						// Doesn't matter if you are on 32 bit or 64 bit,
						// DWORD is always 32 bits, so first two revision numbers
						// come from dwFileVersionMS, last two come from dwFileVersionLS
						cpcc_stringstream ssresult;
						ssresult << ((verInfo->dwFileVersionMS >> 16) & 0xffff);
						ssresult << _T(".");
						ssresult << ((verInfo->dwFileVersionMS) & 0xffff);
						ssresult << _T(".");
						ssresult << ((verInfo->dwFileVersionLS >> 16) & 0xffff);
						ssresult << _T(".");
						ssresult << ((verInfo->dwFileVersionLS) & 0xffff);
						result = ssresult.str();
					}
				}
			}
		}
		delete[] verData;
		return result;
	}


	static const bool is64bit(void)
	{
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724423%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
		//  PROCESSOR_ARCHITECTURE_AMD64 -> x64 (AMD or Intel)

		SYSTEM_INFO siSysInfo;
		// To retrieve accurate information for an application running on WOW64, call the GetNativeSystemInfo function.
		//GetSystemInfo(&siSysInfo);

		// Copy the hardware information to the SYSTEM_INFO structure.
		GetNativeSystemInfo(&siSysInfo);
		// PROCESSOR_ARCHITECTURE_AMD64 means x64 (AMD or Intel)
		return (siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
	}
    

};
