/*  *****************************************
 *  File:		cpccSettings.cpp
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
#include <locale>
#include <codecvt>
#include <cmath>
#include "core.cpccStringUtil.h"
#include "io.cpccSettings.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#include "io.cpccSystemFolders.h"
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




class cINI_utils
{
public:
    typedef cpcc_char *tEncodingsTable[5][2];

private:
    
    
    static const tEncodingsTable& encondingsINI(void)
    {
        // static variable
        // encode characters to fit to the INI file (e.g. multiline text)
        static const tEncodingsTable encodedINIcharacterTable =
        {
            //        normal     ->       encoded
            { (cpcc_char *)  _T("="),  (cpcc_char *) _T("\\=") },
            { (cpcc_char *)  _T("\n"), (cpcc_char *) _T("\\n") },
            { (cpcc_char *)  _T("\r"), (cpcc_char *) _T("\\r") },
            { (cpcc_char *)  _T("\t"), (cpcc_char *) _T("\\t") },
            // this must be the last rule  for the decoding and first rule for the encoding
            { (cpcc_char *)  _T("\\"), (cpcc_char *) _T("\\\\") }
        };
        return encodedINIcharacterTable;
    }
    
public:
    
    static void    encodeStrForINI(cpcc_string &str)
    {
        for (int i = (sizeof(encondingsINI()) / sizeof(encondingsINI()[0]))-1; i>=0; --i)
            stringUtils::findAndReplaceAll(str, encondingsINI()[i][0], encondingsINI()[i][1]);
    }
    
    
    static void    decodeStrFromINI(cpcc_string &str)
    {
        //for (int i = 0; i< sizeof(encodedINIcharacterTable) / sizeof(encodedINIcharacterTable[0]); ++i)
        //    findAndReplaceAll(str, encodedINIcharacterTable[i][1], encodedINIcharacterTable[i][0]);
        for (unsigned int i = 0; i< sizeof(encondingsINI()) / sizeof(encondingsINI()[0]); ++i)
            stringUtils::findAndReplaceAll(str, encondingsINI()[i][1], encondingsINI()[i][0]);
    }


    static bool     saveToFile(const cpcc_char *aFilename, const cpccKeyValue::tKeysAndValues& aMap)
    {
        cpcc_ostringstream ss;

        for (cpccKeyValue::tKeysAndValues::const_iterator it = aMap.begin(); it != aMap.end(); ++it)
        {
            const cpcc_char *key = it->first.c_str();
            cpcc_string    value = it->second; // todo: value( it->second);
            cINI_utils::encodeStrForINI(value);
            ss << key << _T("=") << value << std::endl;
        }

#ifdef UNICODE	// write the BOM if UTF-8
        //const cpcc_string tmpTxt(ss.str());
        //const cpcc_string tmpTxtAfterUTF8(helper_to_utf8(tmpTxt.c_str(), tmpTxt.length()));
        //return saveStringToFile(mFilename.c_str(), tmpTxtAfterUTF8.c_str(), true);
        return cpccFileSystemMini::writeTextFile(aFilename, ss.str().c_str(), true);
#else
        return cpccFileSystemMini::writeTextFile(aFilename, ss.str().c_str(), false);
#endif
    }


    static bool loadFromFile(const cpcc_char *aFilename, cpccKeyValue::tKeysAndValues& aMap)
    {
        if (!cpccFileSystemMini::fileExists(aFilename))
            return false;


        cpcc_ifstream iniFile(aFilename);
        if (!iniFile.is_open())
        {
            cpcc_cerr << _T("#8551: Could not open file:") << aFilename << _T("\n");
            return false;
        }

        // todo: is this needed for MacOS ?
        std::locale my_utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
        iniFile.imbue(my_utf8_locale);

        // directly manipulate the map so that the save-to-file is not triggered
        aMap.clear();
        cpcc_string key, value;

        // http://forums.codeguru.com/showthread.php?511066-RESOLVED-Is-it-possible-to-use-getline-with-unicode
        while (getline(iniFile, key, _T('=')))
        {
            if (key.compare(0, 3, _T(UTF8_BOM)) == 0)  // the file has a UTF-8 BOM
                key.erase(0, 3);                  // Now get rid of the BOM.

            getline(iniFile, value);
            cINI_utils::decodeStrFromINI(value);
            // set(key.c_str(), value);
            // directly add into the map so that the save to file is not triggered
            aMap[key] = value;
        }

        return true;
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
///////////////////////////////////////////////////////////////////////////////////////////////////
cpccSettings::cpccSettings(const cpcc_char *aFilename)
{
    if (!aFilename)
        return;
    
    mFilename = aFilename;
    if (!load())
        cpcc_cerr << _T("Error #1351: loading cpccSettings from file:") << mFilename << std::endl;
}


cpcc_string cpccSettings::getAutoFilename(const settingsScope aScope, const cpcc_char* aCompanyName, const cpcc_char* aAppName, const cpcc_char* aBundleID)
{
    cpccPathString fname(aScope==scopeAllUsers ? cpccSystemFolders::getFolder_CommonAppData() : cpccSystemFolders::getFolder_UserData());
    assert(cpccFileSystemMini::folderExists(fname.c_str()) && _T("#5381: folder for saving the settings file does not exist"));
    
    #ifdef __APPLE__
        // fname("Preferences");
        if (aScope==scopeAllUsers)
            if (aBundleID)
                if (cpcc_strlen(aBundleID)>0)
                    fname.appendPathSegment(aBundleID);
    
    #else
        if (aCompanyName)
            if (cpcc_strlen(aCompanyName)>0)
                fname.appendPathSegment(aCompanyName);
    #endif
    // now the fname contains the containing folder for the INI file.
    
    // add the appName as part of the filename
    // operator && evaluates left operand first
    if (aAppName && (cpcc_strlen(aAppName)>0))
        fname.appendPathSegment(aAppName);
    else
        fname.appendPathSegment(_T("no AppName cpccSettings error no.7511"));
    
    fname.append(_T(".ini"));
    return fname;
}

/*
cpccSettings::cpccSettings(const settingsScope aScope):
	instantSaving(true)
{
	// std::cout << "cpccSettings constructor\n";
	
	assert(cpcc_strlen(config_getCompanyName())>0 && _T("#5351: cpccSettings: blank company name"));
	assert(cpcc_strlen(config_getAppName())>0 && _T("#5351: cpccSettings: blank Software name"));

	cpccPathString _settingsFilename(aScope==scopeAllUsers ? cpccSystemFolders::getFolder_CommonAppData() : cpccSystemFolders::getFolder_UserData());
	assert(cpccFileSystemMini::folderExists(_settingsFilename.c_str()) && _T("#5381: folder for saving the settings file does not exist"));
#ifdef __APPLE__
	// _settingsFilename.appendPathSegment("Preferences");
    if (aScope==scopeAllUsers)
        _settingsFilename.appendPathSegment(config_getAppBundleID());
    
#else
    _settingsFilename.appendPathSegment(config_getCompanyName());
#endif
    // make sure the folder for the INI file exists
    if (! _settingsFilename.pathExists())
        if (!cpccFileSystemMini::createFolder(_settingsFilename.c_str()))
            cpcc_cerr << _T("During cpccSettings constructor could not create folder:") << _settingsFilename << _T("\n");
    
    assert((_settingsFilename.pathExists()) && _T("#7712i: folder for INI was not created"));
    
    _settingsFilename.appendPathSegment(config_getAppName());
	_settingsFilename.append(_T(".ini"));
    
	mFilename = _settingsFilename;
	
	if (!load())
		cpcc_cerr << _T("Error #1351: loading cpccSettings from file:") << mFilename << std::endl;
	
	//std::cout << "cpccSettings construction point 2\n";
}
*/

	
cpccSettings::~cpccSettings()
{
	if (!instantSaving)
		if (!save())
			cpcc_cerr << _T("Error #1352: saving cpccSettings to file:") << mFilename << std::endl;
}


bool cpccSettings::load(void)
{
    if (!cpccFileSystemMini::fileExists(mFilename.c_str()))
    {
        clear();
        return true; // consider the INI loaded (empty file)
    }
    
    return cINI_utils::loadFromFile(mFilename.c_str(), m_map);
    /* moved
	if (!cpccFileSystemMini::fileExists(mFilename.c_str()))
		return true;
	
	
	cpcc_ifstream iniFile(mFilename.c_str());
	if (!iniFile.is_open())
	{
		cpcc_cerr << _T("#8551: Could not open file:") << mFilename << _T("\n");
		return false;
	}
	
    // todo: is this needed for MacOS ?
    std::locale my_utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
    iniFile.imbue(my_utf8_locale);

    // directly manipulate the map so that the save-to-file is not triggered
    m_map.clear();
	cpcc_string key, value;
    
	// http://forums.codeguru.com/showthread.php?511066-RESOLVED-Is-it-possible-to-use-getline-with-unicode
	while(getline(iniFile, key, _T('=')))
    {
        if (key.compare(0, 3, _T(UTF8_BOM)) == 0)  // the file has a UTF-8 BOM
            key.erase(0, 3);                  // Now get rid of the BOM.

		getline(iniFile, value);
		cINI_utils::decodeStrFromINI(value);
        // set(key.c_str(), value);
        // directly add into the map so that the save to file is not triggered
        m_map[key] = value;
    }
	
	return true;
    */
}


#ifdef _WIN32
std::string helper_to_utf8(const wchar_t* buffer, int len)
{
    std::string result;
    int nChars = ::WideCharToMultiByte( CP_UTF8, 0, buffer, len, NULL, 0, NULL, NULL);
    if (nChars > 0)
    {
        result.resize(nChars);
        // ::WideCharToMultiByte( CP_UTF8, 0, buffer, len, const_cast<char*>(result.c_str()), nChars, NULL, NULL);
        ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, &result[0], nChars, NULL, NULL);
    }
    return result;
}
#endif


bool cpccSettings::save(void)
{
    return cINI_utils::saveToFile(mFilename.c_str(), m_map);

    /* moved
    cpcc_ostringstream ss;

    for (cpccKeyValue::tKeysAndValues::iterator it = m_map.begin(); it != m_map.end(); ++it)
    {
        const cpcc_char *key = it->first.c_str();
        cpcc_string    value = it->second; // todo: value( it->second);
        cINI_utils::encodeStrForINI(value);
        ss << key << _T("=") << value << std::endl;
    }
    
    #ifdef UNICODE	// write the BOM if UTF-8
        //const cpcc_string tmpTxt(ss.str());
        //const cpcc_string tmpTxtAfterUTF8(helper_to_utf8(tmpTxt.c_str(), tmpTxt.length()));
        //return saveStringToFile(mFilename.c_str(), tmpTxtAfterUTF8.c_str(), true);
        return saveStringToFile(mFilename.c_str(), ss.str().c_str(), true);
    #else
        return saveStringToFile(mFilename.c_str(), ss.str().c_str(), false);
    #endif
    */
    

    /*
#define WRITE_WITH_STREAM

#ifdef WRITE_WITH_STREAM
	
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
        // _file.write(_T("\xEF\xBB\xBF"), 3);
	#endif
	
	for (tKeysAndValues::iterator it = mSettings.begin(); it != mSettings.end(); ++it)
	{
		const cpcc_char *key = it->first.c_str();
		cpcc_string    value = it->second; // todo: value( it->second);
		stringConversions::encodeStrForINI(value);
		_file << key << _T("=") << value << std::endl;
	}
	
	_file.close();
	return true;
#else

    //	http://www.stev.org/post/2012/03/19/C++-Read-Write-stdmap-to-a-file.aspx
    //	examples with greek text
	//	http://blog.cppcms.com/post/105
	//  http://www.cplusplus.com/forum/general/7142/
    
    // FILE *fp = cpcc_fopen(mFilename.c_str(), _T("w, ccs=UTF-8")); // me ayto grafei sosta to elliniko keimeno alla stamataei pio kato
    #pragma warning(suppress : 4996)
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
#endif
    
    */
    
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
    float  bigFloat = 13456798.43e9f;
	const cpcc_char * tmpTestString = _T("abc-καλημέρα=good\n\rmorning to all.");
	{
		// writing 
		cpccSettings settingsUser( scopeCurrentUser);
#ifndef OSX_SANDBOXED   // define this is your app is Sandboxed for the OSX apple store
		cpccSettings settingsApp( scopeAllUsers);
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
		cpccSettings settingsUser(scopeCurrentUser);
        
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
/*
cpccSettings &appUserSettings(void)
{
	static cpccSettings m_appUserSettings(cpccSettings::scopeCurrentUser);
	return m_appUserSettings;
}


cpccSettings &appSystemSettings(void)
{
#ifndef OSX_SANDBOXED
    static cpccSettings m_appSystemSettings( cpccSettings::scopeAllUsers);
    return m_appSystemSettings;
#else
    return appUserSettings();
#endif
}


#if defined(cpccSettings_DoSelfTest)
	SELFTEST_BEGIN(cpccSettings_SelfTest)
        cpccKeyValue::selfTest();
        cpccSettings::selfTest();
		
	SELFTEST_END
#endif
*/


///////////////////////////////////////////////////////////////////////////


