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
	
	Dependencies: cpcc_SelfTest.h
 
    ToDo: On MAC, settings should be saved in a directory with same name as bundle identifier.
*/

#ifdef cpccDEBUG
    #define cpccSettings_DoSelfTest		true
#endif


////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
////////////////////////////////////////////////////////////////////////////////

class cpccSettings
{
private:
	typedef std::map<cpcc_string, cpcc_string> tKeysAndValues;
	tKeysAndValues	mSettings;
	cpcc_string 	mFilename;

public:	// class metadata and selftest
	enum settingsScope { scopeCurrentUser=0, scopeAllUsers };
	

#if defined(cpccSettings_DoSelfTest)
	static void selfTest(void);
#endif

private: 	// data
	bool instantSaving;

public: 	// ctors	
	cpccSettings(const cpcc_char *aCompanyName, const cpcc_char *aSoftwareName, const settingsScope aScope=scopeCurrentUser);
	~cpccSettings();
	
public:		// functions
	cpcc_string getFilename(void) { return mFilename; }
	
	template <typename T>
	const T read(const cpcc_char *aKey, const T aDefault)
    {
        return stringConversions::fromStr(read(aKey, _T("")).c_str(), aDefault);
    }
    
	const cpcc_string read(const cpcc_char *aKey, const cpcc_char *aDefault)
    {
        return mSettings.count(aKey) ? mSettings[aKey] : aDefault;
    }
    
	const cpcc_string read(const cpcc_char *aKey, const cpcc_string &aDefault)	{ return read(aKey, aDefault.c_str()); }
    
	void		write(const cpcc_char *aKey, const cpcc_char *aValue);
    void        write(const cpcc_char *aKey, const cpcc_string &aValue) { write(aKey, aValue.c_str()); };
    template <typename T>
	void		write(const cpcc_char *aKey, const T aValue);
    
#define BYPASS_RELEASE_ERROR
#ifdef  BYPASS_RELEASE_ERROR

#ifndef __APPLE__
	// in OSX, time_t is defined as long, so there is a ready function for it
	// in Windows time_t is _int64
	void		write(const cpcc_char *aKey, const time_t aValue);
#endif
    void		write(const cpcc_char *aKey, const bool aValue);
    void		write(const cpcc_char *aKey, const int aValue);
    void		write(const cpcc_char *aKey, const long int aValue);
#endif
    
	void		clear(void);
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
    inline const T getValue(void) const     { return mIniPtr.read(mKey.c_str(), mDefaultValue); }
	inline  operator const T(void) const     { return mIniPtr.read(mKey.c_str(), mDefaultValue); }
	// T read(void) const { return mIniPtr.read(mKey.c_str(), mDefaultValue); }
	// T readAtIndex(const int index) const { return mIniPtr.read(createIndexedKey(index).c_str(), mDefaultValue); }
	inline const T operator[](const int index) const { return mIniPtr.read(createIndexedKey(index).c_str(), mDefaultValue); }

    // set value
	// void operator=(T &aValue)	const { mIniPtr.write(mKey.c_str(), aValue); } // try a non-const version
	void operator=(const T &aValue)	const { mIniPtr.write(mKey.c_str(), aValue); }
	//void write(const T aValue) const { mIniPtr.write(mKey.c_str(), aValue); }
	void writeAtIndex(const int index, const T aValue) { mIniPtr.write(createIndexedKey(index).c_str(), aValue); }
};




cpccSettings &appUserSettings(void);
cpccSettings &appSystemSettings(void);
