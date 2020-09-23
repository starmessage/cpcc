/*  *****************************************
 *  File:		io.cpccSettings.h
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
#include <cmath>
#include "core.cpccIdeMacros.h"
#include "cpccUnicodeSupport.h"
#include "core.cpccKeyValue.h"
#include "cpccTesting.h"
#include "io.cpccPathHelper.h"
#include "io.cpccFileSystemMini.h"

/** A small and portable (cross platform) C++ class 
	to save/load application settings from an INI-like file
	The file is a variable=value text file.
	No [sectionName] headings are used.

*/


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

            If you need to create a directory in /Library/Application Support for your application to use then you need to do privilege elevation.
            Applications in the App Store cannot use privilege elevation

        sandboxed:

 */


// //////////////////////////////////////////////////////////////////////////////
//
//		class cpccSettings declaration
//
// //////////////////////////////////////////////////////////////////////////////

class cpccSettings: public cpccKeyValue
{
private:
    cpcc_string 	mFilename;

public:	// class metadata and selftest
	enum class settingsScope { scopeCurrentUser=0, scopeAllUsers };


public: 	// data
    
	bool instantSaving = true;
	bool m_needsSaving = false;

public: 	// ctors
    
	// explicit cpccSettings(const settingsScope aScope=scopeCurrentUser);
    explicit cpccSettings(const cpcc_char *aFilename);
	virtual ~cpccSettings();
	
public:		// functions

    static cpcc_string getAutoFilename(const settingsScope aScope, const cpcc_char* aCompanyName, const cpcc_char* aAppName, const cpcc_char* aBundleID);

    virtual void dataHasChanged(void) override;

	cpcc_string getFilename(void) const { return mFilename; }
	
	bool		load(void);
	bool		save(void);

	void		pauseInstantSaving(void) { instantSaving = false; }
	void		resumeInstantSaving(void);

};

// //////////////////////////////////////////////////////////////////////////////
//
//		class cpccSettings implementation
//
// //////////////////////////////////////////////////////////////////////////////


inline void cpccSettings::dataHasChanged(void)
{
    m_needsSaving = true;
    if (!instantSaving)
        return;

    if (!save())
        cpcc_cerr << _T("Error #1352p: saving cpccSettings to file:") << mFilename << std::endl;
    else
        m_needsSaving = false;
}




// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccPersistentVar
//
// /////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class cpccPersistentVar
{
private:
	 cpccSettings &	mIniRef;
	 cpcc_string	mKey;
     const T		mDefaultValue;

	const cpcc_string createIndexedKey(const int index) const
	{
		cpcc_ostringstream s;
		s << mKey << index;
		return cpcc_string(s.str());
	}
       
public:
    explicit cpccPersistentVar(cpccSettings &aIniPtr, const cpcc_char *aKey, const T aDefaultValue):
            mIniRef(aIniPtr),
            mKey(aKey),
            mDefaultValue(aDefaultValue)
	{ }

    // get value
    inline const T getValue(void) const     { return mIniRef.get(mKey.c_str(), mDefaultValue); }
	inline  operator const T(void) const     { return mIniRef.get(mKey.c_str(), mDefaultValue); }
	// T read(void) const { return mIniRef.read(mKey.c_str(), mDefaultValue); }
	// T readAtIndex(const int index) const { return mIniRef.read(createIndexedKey(index).c_str(), mDefaultValue); }
	inline const T operator[](const int index) const { return mIniRef.get(createIndexedKey(index).c_str(), mDefaultValue); }

    // set value
    inline void setValue(const T &aValue)	{ mIniRef.set(mKey.c_str(), aValue); }

	// void operator=(T &aValue)	const { mIniRef.write(mKey.c_str(), aValue); } // try a non-const version
	void operator=(const T &aValue)	const { mIniRef.set(mKey.c_str(), aValue); }
	//void write(const T aValue) const { mIniRef.write(mKey.c_str(), aValue); }
	void writeAtIndex(const int index, const T aValue) { mIniRef.set(createIndexedKey(index).c_str(), aValue); }
};



////////////////////////////////////////////////////////////////////////////////
//
//		class cpccSettings testing
//
////////////////////////////////////////////////////////////////////////////////


TEST_RUN(cpccSettings_testNew)
{
    const bool skipThisTest = false;

    if (skipThisTest)
    {
        TEST_ADDNOTE("Test skipped");
        return;
    }

    const double piFloat = 3.14159265359;
    const float  bigFloat = 13456798.43e9f;
    const cpcc_char* tmpTestString = _T("abc-καλημέρα=good\n\rmorning to all.");

    cpcc_string fnameCurrentUser(cpccSettings::getAutoFilename(cpccSettings::settingsScope::scopeCurrentUser, _T("Test Company Name"), _T("Test App Name"), _T("com.StarMessageSoftware.SelfTestBundleID")));
    cpcc_string fnameAllUsers(cpccSettings::getAutoFilename(cpccSettings::settingsScope::scopeAllUsers, _T("Test Company Name"), _T("Test App Name"), _T("com.StarMessageSoftware.SelfTestBundleID")));
    
    TEST_ADDNOTE(_T("ini file for current user: ") << fnameCurrentUser);
    TEST_ADDNOTE(_T("ini file for all users: ") << fnameAllUsers);

    // clean up
    if (cpccFileSystemMini::fileExists(fnameCurrentUser.c_str()))
        cpccFileSystemMini::deleteFile(fnameCurrentUser.c_str());
    if (cpccFileSystemMini::fileExists(fnameAllUsers.c_str()))
        cpccFileSystemMini::deleteFile(fnameAllUsers.c_str());

    /*
    cpcc_string folderCurrentUser = cpccPathHelper::getParentFolderOf(fnameCurrentUser);
    if (cpccFileSystemMini::folderExists(folderCurrentUser.c_str()))
        cpccFileSystemMini::deleteFolder(folderCurrentUser.c_str());
    
    cpcc_string folderAllUsers = cpccPathHelper::getParentFolderOf(fnameCurrentUser);
    if (cpccFileSystemMini::folderExists(folderAllUsers.c_str()))
        cpccFileSystemMini::deleteFolder(folderAllUsers.c_str());
    */

    {
        // writing


        cpccSettings settingsUser(fnameCurrentUser.c_str());
#ifndef OSX_SANDBOXED   // define this is your app is Sandboxed for the OSX apple store
        cpccSettings settingsApp(fnameAllUsers.c_str());
#else
        cpccSettings& settingsApp = settingsUser;
#endif

        settingsUser.set(_T("testStringKeyA"), _T("testStringValueA"));


        settingsUser.set(_T("testStringKeyB"), _T("tmpValue"));
        settingsUser.set(_T("testStringKeyB"), _T("B"));

        settingsUser.set(_T("testTrueKey"), true);
        settingsUser.set(_T("testFalseKey"), false);
        settingsUser.set(_T("pi"), piFloat);
        settingsUser.set(_T("twentythree"), 23);

        settingsUser.set(_T("bigFloat"), bigFloat);

        settingsApp.set(_T("AppSettingsOfSoftware"), _T("testSoftwareName"));
        settingsApp.set(_T("extremeString"), tmpTestString);
        cpccPersistentVar<int> tmpPersistentInt(settingsApp, _T("tmpPersInt"), 98);
        tmpPersistentInt = 456;
        tmpPersistentInt.writeAtIndex(3, 678);
    }

    //assert(cpccFileSystemMini::fileExists(fnameCurrentUser.c_str()) && _T("SelfTest #7712a: file does not exist"));

    TEST_EXPECT(cpccFileSystemMini::fileExists(fnameCurrentUser.c_str()) , _T("SelfTest #7712a: file does not exist"));

#ifndef OSX_SANDBOXED
    // the following fails under Catalina
    TEST_EXPECT(cpccFileSystemMini::fileExists(fnameAllUsers.c_str()) , _T("SelfTest #7712b: file does not exist"));
#endif

    if (true) // turn ON/OFF the reading tests
    {
        // separate reading
        cpccSettings settingsUser(fnameCurrentUser.c_str());

#ifndef OSX_SANDBOXED
        cpccSettings settingsSystem(fnameAllUsers.c_str());
#else
        cpccSettings& settingsSystem = settingsUser;
#endif

        cpcc_string tmp = settingsUser.get(_T("testStringKeyA"), _T("default"));
        TEST_EXPECT(tmp.compare(_T("testStringValueA")) == 0 , _T("SelfTest #7711b: readString error"));

        tmp = settingsUser.get(_T("NonExistingKey"), _T("default"));
        TEST_EXPECT(tmp.compare(_T("default")) == 0 , _T("SelfTest #7711c: readString error on default value"));

        tmp = settingsUser.get(_T("testStringKeyB"), _T("default"));
        TEST_EXPECT(tmp.compare(_T("B")) == 0 , _T("SelfTest #7711d: writeString error: does not update values"));

        TEST_EXPECT(settingsUser.get(_T("testTrueKey"), false) , _T("SelfTest #7711e: readBool error 1"));
        TEST_EXPECT(!settingsUser.get(_T("testFalseKey"), true) , _T("SelfTest #7711e: readBool error 2"));
        TEST_EXPECT(settingsUser.get(_T("testMissingKey1"), true) , _T("SelfTest #7711e: readBool error 3"));
        TEST_EXPECT(!settingsUser.get(_T("testMissingKey2"), false) , _T("SelfTest #7711e: readBool error 4"));

        TEST_EXPECT(settingsUser.get(_T("pi"), 1.0) == piFloat , _T("SelfTest #7711f: readReal"));

        // todo: this has problems to solve
        float aFloat = settingsUser.get(_T("bigFloat"), 2.0f);
        aFloat -= bigFloat;
        if (bigFloat != 0)
            aFloat /= bigFloat;

        //cpcc_cout << _T("aFloat read=") << aFloat << std::endl;
        TEST_EXPECT((std::fabs(aFloat) < 0.000001f) , _T("SelfTest #7711k: readReal bigFloat"));

        TEST_EXPECT(settingsUser.get(_T("twentythree"), 2) == 23 , _T("SelfTest #7711g: readLongint"));

        cpccPersistentVar<int> tmpPersistentInt(settingsSystem, _T("tmpPersInt"), 92);
        TEST_EXPECT((tmpPersistentInt == 456) , _T("SelfTest #7711r: tmpPersistentInt error 1"));
        // assert((tmpPersistentInt.readAtIndex(3) == 678) && "SelfTest #7711j: tmpPersistentInt error 2");
        TEST_EXPECT((tmpPersistentInt[3] == 678) , _T("SelfTest #7711j: tmpPersistentInt error 2"));

        tmp = settingsSystem.get(_T("extremeString"), _T("----"));
        TEST_EXPECT((tmp.compare(tmpTestString) == 0) , _T("SelfTest #7711w: readString error"));
    }

    // clean up
    cpccFileSystemMini::deleteFile(fnameCurrentUser.c_str());
    cpccFileSystemMini::deleteFile(fnameAllUsers.c_str());

    // cpccFileSystemMini::deleteFolder(folderCurrentUser.c_str());
    // cpccFileSystemMini::deleteFolder(folderAllUsers.c_str());
}
