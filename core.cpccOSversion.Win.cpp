/*  *****************************************
 *  File:		core.cpccOSversion.Win.cpp
 *	Purpose:	Portable (cross-platform), light-weight, OS functions
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2018 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#include "core.cpccOSversion.h"

#include <sstream>
#include <Windows.h>
#pragma comment(lib, "Version.lib") // for GetFileVersionInfo


static const sOsVerComponents util_GetFileVersion(const TCHAR* aFilePath)
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

    sOsVerComponents result = { 0 };
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
                    result.verMajor = ((verInfo->dwFileVersionMS >> 16) & 0xffff);
                    result.verMinor = ((verInfo->dwFileVersionMS) & 0xffff);
                    result.verPatch = ((verInfo->dwFileVersionLS >> 16) & 0xffff);
                    result.verBuild = ((verInfo->dwFileVersionLS) & 0xffff);
                }
            }
        }
    }
    delete[] verData;
    return result;
}


const sOsVerComponents cpccOSversion::getKernelVersionComponents(void)
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

    return util_GetFileVersion(_T("kernel32.dll"));
}

/// returns the full versoion and build number, e.g. for windows 10: "10.0.15063.296"
const cpcc_string cpccOSversion::getKernelVersionStr(const int nComponents)
{
    sOsVerComponents ver = getKernelVersionComponents();
    cpcc_stringstream ssresult;
    if (nComponents >= 1)
        ssresult << ver.verMajor;
    if (nComponents >= 2)
        ssresult << _T(".") << ver.verMinor;
    if (nComponents >= 3)
        ssresult << _T(".") << ver.verPatch;
    if (nComponents >= 4)
        ssresult << _T(".") << ver.verBuild;

    return ssresult.str();
}
    
    /*
    static const cpcc_string getWindowsKernelShortVersionNumber(void) // returns something like: "6.1"
    {
        cpcc_string  result = getWindowsKernelFullVersionNumber();    // returns something like: "6.1.7601.23864"
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
    */
    

bool cpccOSversion::isIPad(void) { return false; }
bool cpccOSversion::isIPhone(void) { return false;  }

  
bool cpccOSversion::is64bit(void)
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

const cpcc_string cpccOSversion::getMajorMinorPatchVersionStr(void)
{
    return getKernelVersionStr(3);
}
