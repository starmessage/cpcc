/*  *****************************************
 *  File:		cpccSettings.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to save/load application settings from an INI-like file
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <map>
#include <assert.h>

#include "cpccUnicodeSupport.h"


/** A small and portable (cross platform) C++ class 
	to save/load application settings from an INI-like file
	The file is a variable=value text file.
	No [sectionName] headings are used.
	
	Dependencies: cpcc_SelfTest.h
*/

#define cpccSettings_DoSelfTest		true


class cpccSettings
{
private:
	typedef std::map<cpcc_string, cpcc_string> tKeysAndValues;
	tKeysAndValues	mSettings;
	cpcc_string 	mFilename;

public:	// class metadata and selftest
	enum settingsScope { scopeCurrentUser=0, scopeAllUsers };
	
	static const char * getClassVersion(void) { return "0.53"; };

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
	T read(const cpcc_char *aKey, const T aDefaultValue);
    cpcc_string read(	const cpcc_char *aKey, const cpcc_char *aDefaultValue);
    cpcc_string read(   const cpcc_char *aKey, const cpcc_string aDefaultValue) { return read(aKey, aDefaultValue.c_str()); }
    
	template <typename T>
	void		write(const cpcc_char *aKey, const T aValue);
    void        write(const cpcc_char *aKey, const cpcc_string aValue) { write(aKey, aValue.c_str()); };
    void		write(	const cpcc_char *aKey, const cpcc_char *aValue);
    
	void		clear(void);
	bool		load(void);
	bool		save(void);

	void		pauseInstantSaving(void) { instantSaving = false; }
	void		resumeInstantSaving(void);
		  
};


template <typename T>
class cpccPersistentVar
{
private:
	cpccSettings &	mIniPtr;
	cpcc_string		mKey;

	const cpcc_string createIndexedKey(const int index) const
	{
		std::ostringstream s;
		s << mKey << index;
		return cpcc_string(s.str());
	}

public:
	cpccPersistentVar(cpccSettings &aIniPtr, const cpcc_char *aKey): mIniPtr(aIniPtr), mKey(aKey)
	{ }

	const T read(const T aDefaultValue) const  { return mIniPtr.read(mKey.c_str(), aDefaultValue); }
	const T readAtIndex(const int index, const T aDefaultValue) const  { return mIniPtr.read(createIndexedKey(index).c_str(), aDefaultValue); }

	void write(const T aValue) { mIniPtr.write(mKey.c_str(), aValue); }
	void writeAtIndex(const int index, const T aValue) { mIniPtr.write(createIndexedKey(index).c_str(), aValue); }
};




cpccSettings &appUserSettings(void);
cpccSettings &appSystemSettings(void);