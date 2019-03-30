/*  *****************************************
 *  File:		core.hardware.mm
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

#include "core.cpccIdeMacros.h"
#ifdef cpccTARGET_IOS
    #include <Foundation/Foundation.h>
    #include <UIKit/UIKit.h>
#else
    #include <AppKit/AppKit.h>
#endif

#include "core.cpccIdeMacros.h"
#include "core.cpccHardware.h"



bool cpccHardware::hasRetinaDisplay(void)
{

#ifdef cpccTARGET_IOS
    return ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] && [[UIScreen mainScreen] scale] == 3.0);
#endif
    
#ifdef cpccTARGET_MACOS
        float displayScale = 1;
        if ([[NSScreen mainScreen] respondsToSelector:@selector(backingScaleFactor)]) 
        {
            for (NSScreen *aScreen in [NSScreen screens])
            {
                float s = [aScreen backingScaleFactor];
                if (s > displayScale)
                    displayScale = s;
            }
        }

        return (displayScale>1);
#endif
    
}


/*	cross platform (windows, MacOS, IOS) function that finds and enumerates the monitors and their coordinates
    return value: number of monitors,
    list parameter: details about the monitors
*/
size_t cpccHardware::getListOfMonitors(cpccMonitorList &list)
{
    list.clear();

#if defined(cpccTARGET_IOS)
    cpccMonitorInfoT info;
    info.left = [UIScreen mainScreen].bounds.origin.x;
    info.top = [UIScreen mainScreen].bounds.origin.y;
    info.right = info.left + [UIScreen mainScreen].bounds.size.width;
    info.bottom = info.top + [UIScreen mainScreen].bounds.size.height;
    info.isRetina = ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] && [[UIScreen mainScreen] scale] == 3.0);
    list.push_back(info);

#elif defined(cpccTARGET_MACOS)
    /*	sample code:
        CGDisplayCount nDisplays;
        CGGetActiveDisplayList(0,0, &nDisplays);
        log.printf("Displays connected: %d",(int)nDisplays);
    */
    
    for (NSScreen *aScreen in [NSScreen screens])
    {
            NSRect screenRect = [aScreen frame];
            cpccMonitorInfoT info;
            
            info.top = (int)screenRect.origin.y;
            info.bottom = (int)(screenRect.origin.y + screenRect.size.height);
            info.left = (int)screenRect.origin.x;
            info.right = (int)(screenRect.origin.x + screenRect.size.width);
            info.isRetina = false;
            if ([aScreen respondsToSelector:@selector(backingScaleFactor)]) 
                if ([aScreen backingScaleFactor] > 1)
                    info.isRetina = true;
        
            list.push_back(info);
    }

#endif

    return list.size();
}

