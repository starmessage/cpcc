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

#include <vector>
#include <thread> // for number of CPU cores
#include "cpccUnicodeSupport.h"


// about retina display:
// HiDPI and DPI (high dots-per-inch) and 
// PPI (pixels per inch) as the technical terms used for Retina Display on non-Apple platforms
struct cpccMonitorInfoT { int left=0, top=0, right=0, bottom=0; bool isRetina=false;  };


typedef std::vector<cpccMonitorInfoT> cpccMonitorList;



// /////////////////////////////////////////
//
//  cpccHardware
//
// /////////////////////////////////////////

class cpccHardware
{
public:

    static bool hasRetinaDisplay(void); // todo: rename to hasAtLeastOneRetinaDisplay. But would this function be useful?
    static size_t getListOfMonitors(cpccMonitorList &list);
    static std::basic_string<TCHAR>  getAllMonitorResolutionAsText(void);
    
    static long getRamMemoryTotal_inMb(void);
    static long getRamMemoryFree_inMb(void);
    static int  getCPUcores(void);
    static std::basic_string<TCHAR> getCPUmodel(void);
    
};

// /////////////////////////////////////////
//
//  cpccHardware implementation
//
// /////////////////////////////////////////

inline int cpccHardware::getCPUcores(void)
{
    /*
     // for macOS:
    int mib[] = { CTL_HW, HW_NCPU };
    int nCPUs = 0;
    size_t length = sizeof(nCPUs);
    sysctl(mib, 2, &nCPUs, &length, NULL, 0);
    return nCPUs;
    */
    
    return std::thread::hardware_concurrency();
}


inline std::basic_string<TCHAR>  cpccHardware::getAllMonitorResolutionAsText(void)
{
    cpcc_string result, separatorText;
    cpccMonitorList list;
    getListOfMonitors(list);
    for (const auto &monitor : list)
    {
        result += separatorText + cpcc_to_string(monitor.right - monitor.left) + _T("x") + cpcc_to_string(monitor.bottom - monitor.top);
        if (monitor.isRetina)
            result += _T(" HiDPI");

        separatorText = _T(", ");
    }

    return result;
}



#ifdef _WIN32
    inline bool cpccHardware::hasRetinaDisplay(void) { return false; }
#endif


