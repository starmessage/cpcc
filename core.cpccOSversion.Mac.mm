/*  *****************************************
 *  File:		core.cpccOSversion.h
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
#include "core.cpccIdeMacros.h"

#ifdef cpccTARGET_IOS
    #include <uikit/uikit.h> // for UIUserInterfaceIdiomPad
#else
    #include <cocoa/cocoa.h> 
#endif

#include <mach/mach.h> // for is64bit()
#include <sys/sysctl.h> // for sysctlbyname



const cpcc_string cpccOSversion::getMajorMinorPatchVersionStr(void)
{
    #ifdef cpccTARGET_IOS
        return [[[UIDevice currentDevice] systemVersion] UTF8String];
    #endif
    
    #ifdef cpccTARGET_MACOS

        // In the documentation: "this string is not appropriate for parsing."
        NSString * operatingSystemVersionString = //    @"Версія 10.10.5 (складення 14F2511)";
                [[NSProcessInfo processInfo] operatingSystemVersionString];
    
        // returns: Version 10.12.6 (Build 16G29)
        // under ukrainian — 'uk_UA' locale it returns: Версія 10.10.5 (складення 14F2511)
        if (!operatingSystemVersionString)
            return "?";
    
        std::string result = [operatingSystemVersionString cStringUsingEncoding : NSUTF8StringEncoding];
    
        // remove the localized text of "Version "
        size_t pos = result.find("10.");
        if (pos!=std::string::npos)
            result.erase(0, pos);
        // now it will be like: 10.12.6 (Build 16G29)
    
        // remove the patch version and the build number
        /*
        pos = result.find(".");
        if (pos!=std::string::npos)
            result.erase(pos);
        // now it will be like: 10.12
         */
    
        // remove the build number
        pos = result.find(" (");
        if (pos!=std::string::npos)
            result.erase(pos);
        // now it will be like: 10.12.6
     
        return result; // returns: 10.12.6
    #endif
    return "";
}


// https://stackoverflow.com/questions/3694940/get-osx-version-with-objective-c
// https://stackoverflow.com/questions/11072804/how-do-i-determine-the-os-version-at-runtime-in-os-x-or-ios-without-using-gesta




bool cpccOSversion::isIPad(void) 
{   
    #ifndef cpccTARGET_IOS
        return false;
    #else
        return (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad); 
    #endif
}

    
bool cpccOSversion::isIPhone(void) 
{ 
    #ifndef cpccTARGET_IOS
        return false;
    #else
        return (UI_USER_INTERFACE_IDIOM() != UIUserInterfaceIdiomPad); 
    #endif
}



    /*
     A list of ways to obtain the OS version.
     
     1)
     The uname command-line tool and function provides the unix (darwin) version of the OS.
     Although this is not the marketing version of the OS, it is aligned with it uniquely, so you can deduce the OS-X marketing version from it.
     uname is really useful for getting the mobile device model (e.g. "iPad2,1")
     
     2)
     sysctl kern.osrelease command line (or sysctlbyname("kern.osrelease", str, &size, NULL, 0) function) will provide the same information as uname, marginally easier to parse.
     
     3)
     Gestalt(gestaltSystemVersionMajor) (with its "Minor" and BugFix" variants is the oldest (pre-Carbon!) API to get the marketing OS version, still supported by long deprecated. Available in C from the CoreServices framework, but not recommended.
     
     4) [NO, ceased to update]
     NSAppKitVersionNumber is a float constant of the AppKit framework, that will provide the OS-X Appkit version (aligned with the OS version), available to all applications which link against AppKit. It also provides a comprehensive enumeration of all possible versions (e.g. NSAppKitVersionNumber10_7_2)
     
     5) [NO]
     kCFCoreFoundationVersionNumber is a CoreFoundation framework float constant, identical to the Appkit counterpart, available to all apps linked against CoreFoundation, in both C, Obj-C and Swift. It, too provides a comprehensive enumeration over all OS X released versions (e.g. kCFCoreFoundationVersionNumber10_9)
     This is not updated anymore. Even in 10.13 the latest define is for kCFCoreFoundationVersionNumber10_11_Max
    
     6)
     [[NSProcessInfo processInfo] operatingSystemVersionString]; is a Cocoa API available in Obj-C to both OS-X and iOS applications.
     Under IOS simulator it returns the MacOS host info, not the ISO version
     
     7)
     There is a resource .plist in /System/Library/CoreServices/SystemVersion.plist which among other things, contains the OS version in the "ProductVersion" key. NSProcessInfo reads its information from this file, but you can do this directly using your PList-reading API of choice.
     Might not work on sandboxed apps
     
     */


const cpcc_char *cpccOSversion::kernelVerToOsVer(const cpcc_char *aKernelVer)
{
    if (!aKernelVer)
        return "";
    
    /*
     Darwin version to OS X release
     17.x.x. macOS 10.13.x High Sierra
     16.x.x  macOS 10.12.x Sierra
     15.x.x  OS X  10.11.x El Capitan
     14.x.x  OS X  10.10.x Yosemite
     13.x.x  OS X  10.9.x  Mavericks
     12.x.x  OS X  10.8.x  Mountain Lion
     11.x.x  OS X  10.7.x  Lion
     10.x.x  OS X  10.6.x  Snow Leopard
     9.x.x  OS X  10.5.x  Leopard
     8.x.x  OS X  10.4.x  Tiger
     7.x.x  OS X  10.3.x  Panther
     6.x.x  OS X  10.2.x  Jaguar
     5.x    OS X  10.1.x  Puma
     */

    short int kver=0;
    sscanf(aKernelVer, "%hd.", &kver);
    switch (kver)
    {
        case 5: return "10.1";
        case 6: return "10.2";
        case 7: return "10.3";
        case 8: return "10.4";
        case 9: return "10.5";
        case 10: return "10.6";
        case 11: return "10.7";
        case 12: return "10.8";
        case 13: return "10.9";
        case 14: return "10.10";
        case 15: return "10.11";
        case 16: return "10.12";
        case 17: return "10.13";
        case 18: return "10.14";
        default: return "10.14";;
    }
}

const cpcc_string cpccOSversion::getKernelVersionStr(const int nComponents)
{
    char str[256] = {0};
    size_t size = sizeof(str);
    int ret = sysctlbyname("kern.osrelease", str, &size, NULL, 0); // e.g. returns 17.7.0
    if (ret == 0)
    {
        std::string result(str);
        
        if (nComponents==1)
        {
            size_t pos = result.find(".");
            if (pos!=std::string::npos)
                result.erase(pos);
        }
        
        if (nComponents==2)
        {
            size_t pos = result.rfind(".");
            if (pos!=std::string::npos)
                result.erase(pos);
        }
        
        return result;
    }
    return "";
}

const sOsVerComponents cpccOSversion::getKernelVersionComponents(void)
{
        short int version_[3] = {0};
        char str[256] = {0};
        size_t size = sizeof(str);
        int ret = sysctlbyname("kern.osrelease", str, &size, NULL, 0);
        if (ret == 0)
            sscanf(str, "%hd.%hd.%hd", &version_[0], &version_[1], &version_[2]);
        sOsVerComponents result;
        result.verMajor = version_[0];
        result.verMinor = version_[1];
        result.verPatch = version_[2];
        return result;
}
    


bool cpccOSversion::is64bit(void)
{
        
 		#if __LP64__
            // The app has been compiled for 64-bit intel and thus, can runs only as 64-bit intel
            return true;
        #else
                // check
                // https://stackoverflow.com/questions/458304/how-can-i-programmatically-determine-if-my-app-is-running-in-the-iphone-simulato
                // [[UIDevice currentDevice] model]
                // in Xcode 7, iOS 9 Simulator [[UIDevice currentDevice] model] is returning iPhone instead of iPhone Simulator
                // [[UIDevice currentDevice] name]
    
                #if TARGET_IPHONE_SIMULATOR
                    // The app was compiled as 32-bit for the iOS Simulator.
                    // The following probably does not examine the OS (32 or 64) but the App (32 or 64) compilation
                    return sizeof(int *) == 8;

                #else
                    // The app runs on a real iOS device: ask the kernel for the host info.
                    struct host_basic_info host_basic_info;
                    unsigned int count;
                    kern_return_t returnValue = host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)(&host_basic_info), &count);
                    if(returnValue != KERN_SUCCESS)
                        return false;
                    return (host_basic_info.cpu_type == CPU_TYPE_ARM64);
        
        		#endif // TARGET_IPHONE_SIMULATOR
        #endif // else
        
}

