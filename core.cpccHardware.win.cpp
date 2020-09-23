/*  *****************************************
 *  File:		core.hardware.h
 *	Purpose:	Portable (cross-platform), light-weight functions
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2019 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <Windows.h>
#include <intrin.h> // for __cpuid()

#include "core.cpccHardware.h"


static BOOL CALLBACK util_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    //lprcMonitor holds the rectangle that describes the monitor position and resolution)
    if (!lprcMonitor)
        return FALSE;

    cpccMonitorList *listPtr = (cpccMonitorList *)dwData;
    cpccMonitorInfoT info;

    info.isRetina = false;
    info.bottom = lprcMonitor->bottom;
    info.top = lprcMonitor->top;
    info.left = lprcMonitor->left;
    info.right = lprcMonitor->right;
    listPtr->push_back(info);

    /*
    LPMONITORINFO lpmi;	MONITORINFOEX info;
    info.cbSize = sizeof(info);
    if (!::GetMonitorInfo(hMonitor, (LPMONITORINFO)&info))
    return FALSE;  // stop enumeration if error

    ((cpccMonitorList *)dwData).
    monitors.Add(info);
    return TRUE;
    */

    // To continue the enumeration, return TRUE.
    // To stop the enumeration, return FALSE.
    return TRUE;
}



size_t cpccHardware::getListOfMonitors(cpccMonitorList &list)
{
    list.clear();
    //	Explaation of coordinates in multiple monitors under windows
    //    http://www.flounder.com/virtual_screen_coordinates.htm
    
    // https://msdn.microsoft.com/en-us/library/dd162610%28v=vs.85%29.aspx
    if (!EnumDisplayMonitors(NULL, NULL, util_MonitorEnumProc, (LPARAM)&list))
        return 0;
    return list.size();
}




long cpccHardware::getRamMemoryTotal_inMb(void)
{
    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa366589%28v=vs.85%29.aspx
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    // If the function fails, the return value is zero. To get extended error information, call GetLastError.
    if (GlobalMemoryStatusEx(&statex))
        return (long) (statex.ullTotalPhys / (1024 *1024)); // ullTotalPhys: The amount of actual physical memory, in bytes.

    return 0;
}


long cpccHardware::getRamMemoryFree_inMb(void)
{
    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa366589%28v=vs.85%29.aspx
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    // If the function fails, the return value is zero. To get extended error information, call GetLastError.
    if (GlobalMemoryStatusEx(&statex))
        return (long) (statex.ullAvailPhys / (1024 *1024)); // ullTotalPhys: The amount of actual physical memory, in bytes.

    return 0;
}


std::basic_string<TCHAR> cpccHardware::getCPUmodel(void)
{
    // https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?redirectedfrom=MSDN&view=vs-2019
    
    // says "GenuineIntel" or "AuthenticAMD" when read in the correct order:
    // CPUInfo[1] = 0x756e6547 = 'Genu'
    // CPUInfo[3] = 0x49656e69 = 'ineI'
    // CPUInfo[2] = 0x6c65746e = 'ntel'

        int CPUInfo[4] = {-1}; // An array of four integers will contain the information returned in EAX, EBX, ECX, and EDX about supported features of the CPU.
        unsigned   nExIds, i =  0;
        TCHAR CPUBrandString[0x40] = { 0 };
        // Get the information associated with each extended ID.
        // When the function_id argument is 0, cpuInfo[0] returns the highest available non-extended function_id value supported by the processor. 
        // The processor manufacturer is encoded in cpuInfo[1], cpuInfo[2], and cpuInfo[3].
        // Some processors support Extended Function CPUID information.
        // When it's supported, function_id values from 0x80000000 might be used to return information. 
        // To determine the maximum meaningful value allowed, set function_id to 0x80000000. 
        // The maximum value of function_id supported for extended functions will be written to cpuInfo[0].
        
        __cpuid(CPUInfo, 0x80000000);
        nExIds = CPUInfo[0]; 
        for (i=0x80000000; i<=nExIds; ++i)
        {
            __cpuid(CPUInfo, i);
            // Interpret CPU brand string
            if  (i == 0x80000002)
                memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            else if  (i == 0x80000003)
                memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            else if  (i == 0x80000004)
                memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }

    return CPUBrandString;
}
