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


#include "cpccUnicodeSupport.h"
#include <vector>

// about retina display:
// HiDPI and DPI (high dots-per-inch) and 
// PPI (pixels per inch) as the technical terms used for Retina Display on non-Apple platforms
struct cpccMonitorInfoT { int left, top, right, bottom; bool isRetina=false;  };


typedef std::vector<cpccMonitorInfoT> cpccMonitorList;

///////////////////////////////////////////
//
//  cpccHardware
//
///////////////////////////////////////////

class cpccHardware
{
public:

    static bool hasRetinaDisplay(void);
    static size_t getListOfMonitors(cpccMonitorList &list);
    static cpcc_string getAllMonitorResolutionAsText(void);
};

///////////////////////////////////////////
//
//  cpccHardware implementation
//
///////////////////////////////////////////

inline cpcc_string cpccHardware::getAllMonitorResolutionAsText(void)
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



