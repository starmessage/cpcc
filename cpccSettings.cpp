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
#include <iostream>
#include <sstream>
#include <fstream>
#include <errno.h>

#include "cpccSettings.h"
#include "app.cpccAppInfo.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#if defined(cpccSettings_DoSelfTest)
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
//		cpccSettings
///////////////////////////////////////////////////////////////////////////////////////////////////

cpccSettings::cpccSettings(const cpcc_char *aCompanyName, const cpcc_char *aSoftwareName, const settingsScope aScope):
	instantSaving(true)
{
	// std::cout << "cpccSettings constructor\n";
	
	assert(cpcc_strlen(aCompanyName)>0 && _T("#5351: cpccSettings: blank company name"));
	assert(cpcc_strlen(aSoftwareName)>0 && _T("#5351: cpccSettings: blank Software name"));

	cpccPathString _settingsFilename(aScope==scopeAllUsers ? cpccFileSystemMini::getFolder_CommonAppData() : cpccFileSystemMini::getFolder_UserData());
	assert(cpccFileSystemMini::folderExists(_settingsFilename.c_str()) && _T("#5381: folder for saving the settings file does not exist"));
#ifdef __APPLE__
	// _settingsFilename.appendPathSegment("Preferences");
    _settingsFilename.appendPathSegment(cpccAppInfo::BundleId);
#else
    _settingsFilename.appendPathSegment(aCompanyName);
#endif
    
    if (! _settingsFilename.pathExists())
        if (!cpccFileSystemMini::createFolder(_settingsFilename.c_str()))
            cpcc_cerr << _T("During cpccSettings constructor could not create folder:") << _settingsFilename << _T("\n");
    
    assert((_settingsFilename.pathExists()) && _T("#7712i: folder for INI was not created"));
    
#ifdef __APPLE__
    _settingsFilename.appendPathSegment(cpccAppInfo::BundleId);
#else
    _settingsFilename.appendPathSegment(aSoftwareName);
#endif
	_settingsFilename.append(_T(".ini"));
    
	mFilename = _settingsFilename;
	// todo: remove cout
	cpcc_cout << _T("cpccSettings constructor: filename:") << mFilename << _T("\n");
	
	if (!load())
		cpcc_cerr << _T("Error #1351: loading cpccSettings from file:") << mFilename << std::endl;
	
	//std::cout << "cpccSettings construction point 2\n";
}

	
cpccSettings::~cpccSettings()
{
	if (!instantSaving)
		if (!save())
			cpcc_cerr << _T("Error #1352: saving cpccSettings to file:") << mFilename << std::endl;
}


bool cpccSettings::load(void)
{
	if (!cpccFileSystemMini::fileExists(mFilename))
		return true;
	
	
	cpcc_ifstream iniFile(mFilename.c_str());
	if (!iniFile.is_open())
	{
		cpcc_cerr << _T("#8551: Could not open file:") << mFilename << _T("\n");
		return false;
	}
		
	mSettings.clear();
	cpcc_string key, value;
    
	// http://forums.codeguru.com/showthread.php?511066-RESOLVED-Is-it-possible-to-use-getline-with-unicode
	while(getline(iniFile, key, _T('=')))
    {
		getline(iniFile, value);
		stringConversions::decodeStrFromINI(value);
		mSettings[key] = value;
    }
	
	return true;
}


bool cpccSettings::save(void)
{
	/*
	//	write with streams
	cpcc_ofstream _file(mFilename.c_str());
	if (! _file.good())
	{ 
		#pragma warning(suppress : 4996)
		cpcc_cerr << _T("Error saving file ") << mFilename << _T(" Error message:") << strerror(errno) << _T("\n");
		return false;
	}
	
	// write the BOM if UTF-8
	#ifdef UNICODE
		_file << "\xEF\xBB\xBF";
	#endif
	
	for (tKeysAndValues::iterator it = mSettings.begin(); it != mSettings.end(); ++it)
	{
		const cpcc_char *key = it->first.c_str();
		cpcc_string    value = it->second; 
		stringConversions::encodeStrForINI(value);
		_file << key << _T("=") << value << std::endl;
	}
	
	_file.close();
	return true;
	*/

    //	http://www.stev.org/post/2012/03/19/C++-Read-Write-stdmap-to-a-file.aspx
    //	examples with greek text
	//	http://blog.cppcms.com/post/105
	//  http://www.cplusplus.com/forum/general/7142/
    #pragma warning(suppress : 4996)
    // FILE *fp = cpcc_fopen(mFilename.c_str(), _T("w, ccs=UTF-8")); // me ayto grafei sosta to elliniko keimeno alla stamataei pio kato
	FILE *fp = cpcc_fopen(mFilename.c_str(), _T("w"));
    if (!fp)
    {
        #pragma warning(suppress : 4996)
		cpcc_cerr << _T("Error saving file ") << mFilename << _T(" Error message:") << strerror(errno) << _T("\n");
        return false;
    }
	
	#ifdef UNICODE	// write the BOM if UTF-8
		fprintf(fp, "\xEF\xBB\xBF");
	#endif

    for(tKeysAndValues::iterator it = mSettings.begin(); it != mSettings.end(); ++it)
    {
		const cpcc_char *key = it->first.c_str();	// key = it->first;   
		cpcc_string    value = it->second;
		stringConversions::encodeStrForINI(value);
        cpcc_fprintf(fp, _T("%s=%s\n"), key, value.c_str());
    }
    fclose(fp);
    
    return true;
}


void	cpccSettings::write(const cpcc_char *aKey, const cpcc_char * aValue)
{
    mSettings[cpcc_string(aKey)] = aValue;
    
    if (instantSaving)
        if (!save())
            cpcc_cerr << _T("Error #1352c: saving cpccSettings to file:") << mFilename << std::endl;
    assert((cpccFileSystemMini::fileExists(mFilename.c_str())) && _T("SelfTest #7712r: ini file does not exist after save()"));
}

#ifdef  BYPASS_RELEASE_ERROR
void		cpccSettings::write(const cpcc_char *aKey, const bool aValue)   {   write(aKey, stringConversions::toStr(aValue)); }
void		cpccSettings::write(const cpcc_char *aKey, const int aValue)   {   write(aKey, stringConversions::toStr(aValue)); }
void		cpccSettings::write(const cpcc_char *aKey, const long int aValue)   {   write(aKey, stringConversions::toStr(aValue)); }

	#ifndef __APPLE__
	// in OSX, time_t is defined as long, so there is a ready function for it
	// in Windows time_t is _int64
	void	cpccSettings::write(const cpcc_char *aKey, const time_t aValue) { write(aKey, stringConversions::toStr(aValue)); }
	#endif
#endif

template <typename T>
void		cpccSettings::write(const cpcc_char *aKey, const T aValue)  { write(aKey, stringConversions::toStr(aValue)); }



void cpccSettings::clear(void)	
{
	mSettings.clear();
	if (instantSaving)
		if (!save())
			cpcc_cerr << _T("Error #1352d: saving cpccSettings to file:") << mFilename << std::endl;
}


void cpccSettings::resumeInstantSaving(void)
{
	instantSaving = true;
	if (!save())
		cpcc_cerr << _T("Error #1353: saving cpccSettings to file:") << mFilename << std::endl;
}



#if defined(cpccSettings_DoSelfTest)
void cpccSettings::selfTest(void) 
{
	cpcc_cout << _T("cpccSettings::SelfTest starting\n");
	
	double pi = 3.14159265359;
    float  bigFloat = 1.3456798e16f;
	const cpcc_char * tmpTestString = _T("abc-καλημέρα=good\x0A\x0Dmorning to all.");
	{
		// writing 
		cpccSettings settingsUser(_T("testCompanyName"), _T("testSoftwareName"), scopeCurrentUser);
#ifndef OSX_SANDBOXED   // define this is your app is Sandboxed for the OSX apple store
		cpccSettings settingsApp(_T("testCompanyName"), _T("testSoftwareName"), scopeAllUsers);
#else
        cpccSettings &settingsApp = settingsUser;
#endif
        
		settingsUser.write(_T("testStringKeyA"), _T("testStringValueA"));
		assert(cpccFileSystemMini::fileExists(settingsUser.getFilename()) && _T("SelfTest #7711a: file does not exist"));

		settingsUser.write(_T("testStringKeyB"), _T("tmpValue"));
		settingsUser.write(_T("testStringKeyB"), _T("B"));

		settingsUser.write(_T("testTrueKey"), true);
		settingsUser.write(_T("testFalseKey"), false);
		settingsUser.write(_T("pi"), pi);
		settingsUser.write(_T("twentythree"), 23);
        settingsUser.write(_T("bigFloat"), bigFloat);
    
		settingsApp.write(_T("AppSettingsOfSoftware"), _T("testSoftwareName"));
		settingsApp.write(_T("extremeString"), tmpTestString);
		cpccPersistentVar<int> tmpPersistentInt(settingsApp, _T("tmpPersInt"), 98);
		tmpPersistentInt = 456;
		tmpPersistentInt.writeAtIndex(3, 678);
	}

	{
		// separate reading
		cpccSettings settingsUser(_T("testCompanyName"), _T("testSoftwareName"), scopeCurrentUser);
        
#ifndef OSX_SANDBOXED
		cpccSettings settingsSystem(_T("testCompanyName"), _T("testSoftwareName"), scopeAllUsers);
#else
        cpccSettings &settingsSystem = settingsUser;
#endif

		cpcc_string tmp = settingsUser.read(_T("testStringKeyA"), _T("default"));
		assert(tmp.compare(_T("testStringValueA"))==0 && _T("SelfTest #7711b: readString error"));

		tmp = settingsUser.read(_T("NonExistingKey"), _T("default"));
		assert(tmp.compare(_T("default"))==0 && _T("SelfTest #7711c: readString error on default value"));

		tmp = settingsUser.read(_T("testStringKeyB"), _T("default"));
		assert(tmp.compare(_T("B"))==0 && _T("SelfTest #7711d: writeString error: does not update values"));

		assert(settingsUser.read(_T("testTrueKey"),false)		&& _T("SelfTest #7711e: readBool error 1"));
		assert(!settingsUser.read(_T("testFalseKey"),true)		&& _T("SelfTest #7711e: readBool error 2"));
		assert(settingsUser.read(_T("testMissingKey1"),true)	&& _T("SelfTest #7711e: readBool error 3"));
		assert(!settingsUser.read(_T("testMissingKey2"),false)	&& _T("SelfTest #7711e: readBool error 4"));

		assert(settingsUser.read(_T("pi"),1.0)==pi	&& _T("SelfTest #7711f: readReal"));
        assert(settingsUser.read(_T("bigFloat"),2.0)==bigFloat	&& _T("SelfTest #7711k: readReal bigFloat"));
        
		assert(settingsUser.read(_T("twentythree"),2)==23	&& _T("SelfTest #7711g: readLongint"));

		cpccPersistentVar<int> tmpPersistentInt(settingsSystem, _T("tmpPersInt"), 92);
		assert((tmpPersistentInt == 456) && _T("SelfTest #7711r: tmpPersistentInt error 1"));
		// assert((tmpPersistentInt.readAtIndex(3) == 678) && "SelfTest #7711j: tmpPersistentInt error 2");
		assert((tmpPersistentInt[3] == 678) && _T("SelfTest #7711j: tmpPersistentInt error 2"));

		tmp = settingsSystem.read(_T("extremeString"), _T("----"));
		assert((tmp.compare(tmpTestString) == 0) && _T("SelfTest #7711w: readString error"));
		}
		
	cpcc_cout << _T("cpccSettings::SelfTest ended\n");

}
#endif

// lazy but early enough constructor for the application's settings objects
cpccSettings &appUserSettings(void)
{
	static cpccSettings m_appUserSettings(cpccAppInfo::CompanyName, cpccAppInfo::ProgramName, cpccSettings::scopeCurrentUser);
	return m_appUserSettings;
}

cpccSettings &appSystemSettings(void)
{
	static cpccSettings m_appSystemSettings(cpccAppInfo::CompanyName, cpccAppInfo::ProgramName, cpccSettings::scopeAllUsers);
	return m_appSystemSettings;
}


#if defined(cpccSettings_DoSelfTest)
	SELFTEST_BEGIN(cpccSettings_SelfTest)
        cpccSettings::selfTest();
		cpccKeyValue::selfTest();
	SELFTEST_END
#endif


///////////////////////////////////////////////////////////////////////////


