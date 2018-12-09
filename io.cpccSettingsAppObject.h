/*  *****************************************
 *  File:		io.cpccSettingsAppObject.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to save/load application settings from an INI-like file
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <map>
#include <assert.h>
#include <sstream>
#include "core.cpccIdeMacros.h"
#include "cpccUnicodeSupport.h"
#include "io.cpccSettings.h"



#ifdef cpccDEBUG
    #define cpccSettingsAppObject_DoSelfTest		true
#endif


////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
////////////////////////////////////////////////////////////////////////////////

class cpccSettingsAppObject : public cpccSettings
{
private:
    
    /*
	static const cpcc_char *config_getAppName(void);
    static const cpcc_char *config_getCompanyName(void);
    #ifdef __APPLE__
        static const cpcc_char *config_getAppBundleID(void);
    #endif
    */


#if defined(cpccSettingsAppObject_DoSelfTest)
	static void selfTest(void);
#endif

public: 	// ctors
    
	explicit cpccSettingsAppObject(const cpccSettings::settingsScope aScope=scopeCurrentUser);
	

public:		// functions
       
    	  
};


cpccSettings &appUserSettings(void);
cpccSettings &appSystemSettings(void);
