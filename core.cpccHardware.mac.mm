/*  *****************************************
 *  File:		core.hardware.mm
 *	Purpose:	    Portable (cross-platform), light-weight functions
  *	*****************************************
 *  Library:	    Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2019 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			    Commercial license exists for closed source projects.
 *	Web:		    http://www.StarMessageSoftware.com/cpcclibrary
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

#include <sys/types.h> // for getting the system RAM
#include <sys/sysctl.h> // for getting the system RAM

#import <mach/host_info.h>
#import <mach/mach_host.h>
#import <mach/task_info.h>
#import <mach/task.h>


#include "core.cpccIdeMacros.h"
#include "core.cpccHardware.h"

/* Get CPU brand
 
 #include <sys/types.h>
#include <sys/sysctl.h>
#include <stdio.h>

#define BUFFERLEN 128

int main(){
    char buffer[BUFFERLEN];
    size_t bufferlen = BUFFERLEN;
    sysctlbyname("machdep.cpu.brand_string",&buffer,&bufferlen,NULL,0);
    printf("%s\n", buffer);
}
 result: Intel(R) Core(TM) i7-2600 CPU @ 3.40GHz
 
 */

long cpccHardware::getRamMemoryTotal_inMb(void)
{
    /*
     https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/sysctl.3.html
     The sysctlbyname() function accepts an ASCII representation of the name
     and internally looks up the integer name vector.  Apart from that, it
     behaves the same as the standard sysctl() function.
     */
    
    int mib[] = { CTL_HW, HW_MEMSIZE };
    
    int64_t physical_memory = 0;
    size_t length = sizeof(physical_memory);
    if (sysctl(mib, 2, &physical_memory, &length, NULL, 0)==-1)
        {
            // Upon successful completion, _sysctl() returns 0. Otherwise, a value of -1 is returned and errno is set to indicate the error.
            return 0L;
        }
    return (long) (physical_memory / (1024 *1024));
}


std::basic_string<TCHAR> cpccHardware::getCPUmodel(void)
{
    // int mib[] = {CTL_HW, HW_MODEL};
    // sysctl
    TCHAR buffer[1024];
    size_t size=sizeof(buffer);
    if (sysctlbyname("machdep.cpu.brand_string", &buffer, &size, NULL, 0) < 0)
    {
        // perror("sysctl");
        return _T("");
    }
    return buffer;
}


long cpccHardware::getRamMemoryFree_inMb(void)
{
    // for linux/ posix, (but not for MacOS):
    // long ramsize = sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE);
    
    int mib[6];
    mib[0] = CTL_HW;
    mib[1] = HW_PAGESIZE;

    int pagesize;
    size_t length;
    length = sizeof (pagesize);
    if (sysctl (mib, 2, &pagesize, &length, NULL, 0) < 0)
    {
        // fprintf (stderr, "getting page size");
        return 0L;
    }

    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

    vm_statistics_data_t vmstat;
    if (host_statistics (mach_host_self (), HOST_VM_INFO, (host_info_t) &vmstat, &count) != KERN_SUCCESS)
    {
        // fprintf (stderr, "Failed to get VM statistics.");
        return 0L;
    }

    return ((vmstat.inactive_count + vmstat.free_count) * pagesize)/ (1024 *1024L);
}





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

