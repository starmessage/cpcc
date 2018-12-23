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
#include "core.cpccIdeMacros.h"
#include "cpccUnicodeSupport.h"
#include "core.cpccKeyValue.h"

/** A small and portable (cross platform) C++ class 
	to save/load application settings from an INI-like file
	The file is a variable=value text file.
	No [sectionName] headings are used.

*/

#ifdef cpccDEBUG
    // #define cpccSettings_DoSelfTest		true
#endif


////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
////////////////////////////////////////////////////////////////////////////////

class cpccSettings: public cpccKeyValue
{
private:
    
	// todo: replace this with the ready class cpccKeyValue
	// typedef std::map<cpcc_string, cpcc_string> tKeysAndValues;
	// tKeysAndValues	mSettings;
    // cpccKeyValue    mSettings;
    cpcc_string 	mFilename;

    // These functions return the name of the application, the company name and the bundleID.
    // These parameters will be used to compose the path of where to store the settings file.
    // You must define these function in your main code.
    // e.g. in one of you cpp files:
    //      const cpcc_char *cpccSettings::config_getAppName(void) { return _T("CrcCheckCopy"); }
    //       const cpcc_char *cpccSettings::config_getCompanyName(void) { return _T("StarMessage software"); }

    /*
    static const cpcc_char *config_getAppName(void);
    static const cpcc_char *config_getCompanyName(void);
    #ifdef __APPLE__
        static const cpcc_char *config_getAppBundleID(void);
    #endif
    */

public:	// class metadata and selftest
	enum settingsScope { scopeCurrentUser=0, scopeAllUsers };
	

#if defined(cpccSettings_DoSelfTest)
	static void selfTest(void);
#endif

private: 	// data
    
	bool instantSaving = true;

public: 	// ctors
    
	// explicit cpccSettings(const settingsScope aScope=scopeCurrentUser);
    explicit cpccSettings(const cpcc_char *aFilename);
	virtual ~cpccSettings();
	

public:		// functions

    static cpcc_string getAutoFilename(const settingsScope aScope, const cpcc_char* aCompanyName, const cpcc_char* aAppName, const cpcc_char* aBundleID);

    virtual void dataChanged(void) override
    { 
        if (instantSaving)
            if (!save())
                cpcc_cerr << _T("Error #1352p: saving cpccSettings to file:") << mFilename << std::endl;
    }

	cpcc_string getFilename(void) { return mFilename; }
	
	bool		load(void);
	bool		save(void);

	void		pauseInstantSaving(void) { instantSaving = false; }
	void		resumeInstantSaving(void);
		  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccPersistentVar
///////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class cpccPersistentVar
{
private:
	 cpccSettings &	mIniPtr;
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
            mIniPtr(aIniPtr),
            mKey(aKey),
            mDefaultValue(aDefaultValue)
	{ }

    // get value
    inline const T getValue(void) const     { return mIniPtr.get(mKey.c_str(), mDefaultValue); }
	inline  operator const T(void) const     { return mIniPtr.get(mKey.c_str(), mDefaultValue); }
	// T read(void) const { return mIniPtr.read(mKey.c_str(), mDefaultValue); }
	// T readAtIndex(const int index) const { return mIniPtr.read(createIndexedKey(index).c_str(), mDefaultValue); }
	inline const T operator[](const int index) const { return mIniPtr.get(createIndexedKey(index).c_str(), mDefaultValue); }

    // set value
    inline void setValue(const T &aValue)	{ mIniPtr.set(mKey.c_str(), aValue); }

	// void operator=(T &aValue)	const { mIniPtr.write(mKey.c_str(), aValue); } // try a non-const version
	void operator=(const T &aValue)	const { mIniPtr.set(mKey.c_str(), aValue); }
	//void write(const T aValue) const { mIniPtr.write(mKey.c_str(), aValue); }
	void writeAtIndex(const int index, const T aValue) { mIniPtr.set(createIndexedKey(index).c_str(), aValue); }
};



/*
cpccSettings &appUserSettings(void);
cpccSettings &appSystemSettings(void);
*/