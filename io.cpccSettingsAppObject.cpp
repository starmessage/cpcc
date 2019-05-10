/*  *****************************************
 *  File:		cpccSettings.cpcc
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


#include <assert.h>
#include <cmath>

#include "app.cpccAppInfo.h"
#include "io.cpccSettingsAppObject.h"
#include "io.cpccFileSystemMini.h"
#if defined(cpccSettingsAppObject_DoSelfTest)
	#include "cpcc_SelfTest.h"
#endif


/*
 ini file location tests:
 
 
 
 MAC OS X
    info: The User Domain values accessed by NSUserDefaults are serialized to a file ~/Library/Preferences/application.plist.
 
    user settings:
        non-sandboxed:
            /Users/cto/Library/Preferences/com.StarMessageSoftware.StarMessage.ini (ok)
            /Users/cto/Library/Preferences/com.StarMessageSoftware.StarMessage/com.StarMessageSoftware.StarMessage.ini (ok)
 
        sandboxed:
 
    system settings (app-wide, like common APPDATA):
        non-sandboxed:
            /users/shared/com.StarMessageSoftware.StarMessage.ini (ok)
            /Library/Preferences/com.StarMessageSoftware.StarMessage.ini (failed)
            /Library/Preferences/com.StarMessageSoftware.StarMessage/ (failed to create folder)
 
            If you need to create a directory in /Library/Application Support for your application to use then you need to do privilege elevation. Applications in the App Store cannot use privilege elevation
 
        sandboxed:
 
 */



///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccSettingsAppObject
///////////////////////////////////////////////////////////////////////////////////////////////////





cpccSettingsAppObject::cpccSettingsAppObject(const cpccSettings::settingsScope aScope): 
        cpccSettings(cpccSettings::getAutoFilename(aScope, cpccAppInfo::CompanyName, cpccAppInfo::ProgramName, cpccAppInfo::MacBundleId).c_str())
{
	
}

	



#if defined(cpccSettingsAppObject_DoSelfTest)
void cpccSettingsAppObject::selfTest(void)
{
	cpcc_cout << _T("cpccSettings::SelfTest starting\n");
	
	double pi = 3.14159265359;
    float  bigFloat = 13456798.43e9f;
	const cpcc_char * tmpTestString = _T("abc-καλημέρα=good\n\rmorning to all.");
	{
		// writing 
		cpccSettingsAppObject settingsUser( scopeCurrentUser);
#ifndef OSX_SANDBOXED   // define this is your app is Sandboxed for the OSX apple store
		cpccSettingsAppObject settingsApp( scopeAllUsers);
#else
        cpccSettings &settingsApp = settingsUser;
#endif
        
		settingsUser.set(_T("testStringKeyA"), _T("testStringValueA"));
		assert(cpccFileSystemMini::fileExists(settingsUser.getFilename().c_str()) && _T("SelfTest #7711a: file does not exist"));

		settingsUser.set(_T("testStringKeyB"), _T("tmpValue"));
		settingsUser.set(_T("testStringKeyB"), _T("B"));

		settingsUser.set(_T("testTrueKey"), true);
		settingsUser.set(_T("testFalseKey"), false);
		settingsUser.set(_T("pi"), pi);
        settingsUser.set(_T("twentythree"), 23);
        
        settingsUser.set(_T("bigFloat"), bigFloat);

		settingsApp.set(_T("AppSettingsOfSoftware"), _T("testSoftwareName"));
		settingsApp.set(_T("extremeString"), tmpTestString);
		cpccPersistentVar<int> tmpPersistentInt(settingsApp, _T("tmpPersInt"), 98);
		tmpPersistentInt = 456;
		tmpPersistentInt.writeAtIndex(3, 678);
	}

    if (true) // turn ON/OFF the reading tests
	{
		// separate reading
		cpccSettingsAppObject settingsUser(scopeCurrentUser);
        
#ifndef OSX_SANDBOXED
		cpccSettings settingsSystem( scopeAllUsers);
#else
        cpccSettings &settingsSystem = settingsUser;
#endif

		cpcc_string tmp = settingsUser.get(_T("testStringKeyA"), _T("default"));
		assert(tmp.compare(_T("testStringValueA"))==0 && _T("SelfTest #7711b: readString error"));

		tmp = settingsUser.get(_T("NonExistingKey"), _T("default"));
		assert(tmp.compare(_T("default"))==0 && _T("SelfTest #7711c: readString error on default value"));

		tmp = settingsUser.get(_T("testStringKeyB"), _T("default"));
		assert(tmp.compare(_T("B"))==0 && _T("SelfTest #7711d: writeString error: does not update values"));

		assert(settingsUser.get(_T("testTrueKey"),false)		&& _T("SelfTest #7711e: readBool error 1"));
		assert(!settingsUser.get(_T("testFalseKey"),true)		&& _T("SelfTest #7711e: readBool error 2"));
		assert(settingsUser.get(_T("testMissingKey1"),true)	&& _T("SelfTest #7711e: readBool error 3"));
		assert(!settingsUser.get(_T("testMissingKey2"),false)	&& _T("SelfTest #7711e: readBool error 4"));

		assert(settingsUser.get(_T("pi"),1.0)==pi	&& _T("SelfTest #7711f: readReal"));

        // todo: this has problems to solve
        float aFloat=settingsUser.get(_T("bigFloat"), 2.0f);
        aFloat -= bigFloat;
        if (bigFloat!=0)
            aFloat /= bigFloat;

        //cpcc_cout << _T("aFloat read=") << aFloat << std::endl;
        assert((std::fabs(aFloat) < 0.000001f)	&& _T("SelfTest #7711k: readReal bigFloat"));
        
		assert(settingsUser.get(_T("twentythree"),2)==23	&& _T("SelfTest #7711g: readLongint"));

		cpccPersistentVar<int> tmpPersistentInt(settingsSystem, _T("tmpPersInt"), 92);
		assert((tmpPersistentInt == 456) && _T("SelfTest #7711r: tmpPersistentInt error 1"));
		// assert((tmpPersistentInt.readAtIndex(3) == 678) && "SelfTest #7711j: tmpPersistentInt error 2");
		assert((tmpPersistentInt[3] == 678) && _T("SelfTest #7711j: tmpPersistentInt error 2"));

		tmp = settingsSystem.get(_T("extremeString"), _T("----"));
		assert((tmp.compare(tmpTestString) == 0) && _T("SelfTest #7711w: readString error"));
		}
		
	cpcc_cout << _T("cpccSettings::SelfTest ended\n");

}
#endif

// lazy but early enough constructor for the application's settings objects
cpccSettings &appUserSettings(void)
{
    // static variable
	static cpccSettingsAppObject m_appUserSettings(cpccSettings::scopeCurrentUser);
	return m_appUserSettings;
}


cpccSettings &appSystemSettings(void)
{
#ifndef OSX_SANDBOXED
    // static variable
    static cpccSettingsAppObject m_appSystemSettings( cpccSettings::scopeAllUsers);
    return m_appSystemSettings;
#else
    return appUserSettings();
#endif
}


#if defined(cpccSettingsAppObject_DoSelfTest)
	SELFTEST_BEGIN(cpccSettingsAppObject_SelfTest)
        cpccKeyValue::selfTest();
        cpccSettings::selfTest();
		
	SELFTEST_END
#endif




