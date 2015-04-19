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
#include <sstream>
#include "cpccUnicodeSupport.h"


/** A small and portable (cross platform) C++ class 
	to save/load application settings from an INI-like file
	The file is a variable=value text file.
	No [sectionName] headings are used.
	
	Dependencies: cpcc_SelfTest.h
*/

#define cpccSettings_DoSelfTest		true


///////////////////////////////////////////////////////////////////////////////////////////////////
//		stringConversions
///////////////////////////////////////////////////////////////////////////////////////////////////
class stringConversions
{
public:

	static const double fromStr(const cpcc_char* strValue, const double aDefaultValue)
	{
		char* end;
		double n = strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}


	static const float fromStr(const cpcc_char* strValue, const float aDefaultValue)
	{
		char* end;
		float n = (float)strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}


	static const long fromStr(const cpcc_char*  strValue, const long aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		long n = strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

	static const int fromStr(const cpcc_char*  strValue, const int aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		int n = strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

	static const bool fromStr(const cpcc_char*  strValue, const bool aDefaultValue)  { return fromStr(cpcc_string(strValue), aDefaultValue); }
	static const bool fromStr(const cpcc_string&  strValue, const bool aDefaultValue)
	{
		if (strValue.compare("yes") == 0 || strValue.compare("true") == 0 || strValue.compare("1") == 0 || strValue.compare("on") == 0)
			return true;

		if (strValue.compare("no") == 0 || strValue.compare("false") == 0 || strValue.compare("0") == 0 || strValue.compare("off") == 0)
			return false;

		return aDefaultValue;
	}

	static cpcc_string toStr(const bool value) { return cpcc_string(value ? "yes" : "no"); }

	static cpcc_string toStr(const long int value)
	{
		std::ostringstream ss; ss << value; return ss.str();
	}

	static cpcc_string toStr(const int value)
	{
		std::ostringstream  ss; ss << value; return ss.str();
	}

	static cpcc_string toStr(const float value)
	{
		char buf[100];
#pragma warning(disable : 4996)
		sprintf(buf, "%.12f", value);
		return cpcc_string(buf);
	}

	static cpcc_string toStr(const double value)
	{
		char buf[200];
#pragma warning(disable : 4996)
		sprintf(buf, "%.12f", value);
		return cpcc_string(buf);
	}
};




///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
///////////////////////////////////////////////////////////////////////////////////////////////////

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
	const T read(const cpcc_char *aKey, const T aDefault) 						{ return stringConversions::fromStr(read(aKey, "").c_str(), aDefault); }
	const cpcc_string read(const cpcc_char *aKey, const cpcc_char *aDefault)	{ return mSettings.count(aKey) ? mSettings[aKey] : cpcc_string(aDefault); }
	const cpcc_string read(const cpcc_char *aKey, const cpcc_string aDefault)	{ return read(aKey, aDefault.c_str()); }
    


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

///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccPersistentVar
///////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class cpccPersistentVar
{
private:
	 cpccSettings &	mIniPtr;
	 cpcc_string		mKey;
     T                 mDefaultValue;

	const cpcc_string createIndexedKey(const int index) const
	{
		std::ostringstream s;
		s << mKey << index;
		return cpcc_string(s.str());
	}

public:
    cpccPersistentVar(cpccSettings &aIniPtr, const cpcc_char *aKey, const T aDefaultValue):
            mIniPtr(aIniPtr),
            mKey(aKey),
            mDefaultValue(aDefaultValue)
	{ }

	T read(void) { return mIniPtr.read(mKey.c_str(), mDefaultValue); }
	T readAtIndex(const int index) { return mIniPtr.read(createIndexedKey(index).c_str(), mDefaultValue); }

	void write(const T aValue) const { mIniPtr.write(mKey.c_str(), aValue); }
	void writeAtIndex(const int index, const T aValue) { mIniPtr.write(createIndexedKey(index).c_str(), aValue); }
};




cpccSettings &appUserSettings(void);
cpccSettings &appSystemSettings(void);