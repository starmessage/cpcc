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

#pragma once


#include <string>
#include "cpccUnicodeSupport.h"
#include "core.cpccIdeMacros.h"


// todo: rename to sVersionComponents
struct sOsVerComponents
{
    int verMajor, verMinor, verPatch, verBuild;
};



class cpccOSversion 
{

public:
    
   
    // [NSProcessInfo operatingSystemVersion] returns OS X version in iOS simulator
    /*
    static sOSversionInfo getOSversionInfo(void)
    {
        sOSversionInfo result;
        
        return result;
    }
    */
    static const cpcc_string      getMajorMinorPatchVersionStr(void);
    static const cpcc_char *      kernelVerToOsVer(const cpcc_char *aKernelVer);
    static const cpcc_string      getKernelVersionStr(const int nComponents);
    static const sOsVerComponents getKernelVersionComponents(void);
    
    static const cpcc_string      getOSNameAndVersion(void)
    {
        #ifdef cpccTARGET_MACOS
            cpcc_string result("MAC OS ");
        #elif defined(cpccTARGET_IOS)
            cpcc_string result("IOS ");
        #elif defined(cpccTARGET_WINDOWS)
            cpcc_string result(_T("Windows "));
        #endif
        result.append(getMajorMinorPatchVersionStr());
        return result;
    }
    
    static bool            isIPad(void);
    static bool            isIPhone(void);

    static bool            is64bit(void);
    
    static bool            is64bit_cached(void)
    {
        static bool checked_ = false;
        static bool result_ = false;
        if (checked_)
            return result_;
        result_ = is64bit();
        checked_ = true;
        return result_;
    }
    


};
