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
#include "core.cpccHardware.h"


static BOOL CALLBACK util_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    //lprcMonitor holds the rectangle that describes the monitor position and resolution)
    if (!lprcMonitor)
        return false;

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

    return TRUE;
}


bool cpccHardware::hasRetinaDisplay(void) { return false; }


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







