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

#include "cpccUnicodeSupport.h"


/** A small and portable (cross platform) C++ class 
	to save/load application settings from an INI-like file
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
	
	static const char * getClassVersion(void) { return "0.50"; };

#if defined(cpccSettings_DoSelfTest)
	static void selfTest(void);
#endif

public: 	// data
	bool instantSaving;

public: 	// ctors	
	cpccSettings(const cpcc_char *aCompanyName, const cpcc_char *aSoftwareName, const settingsScope aScope=scopeCurrentUser);
	~cpccSettings();
	

public:		// functions
	cpcc_string getFilename(void) { return mFilename; }
	
	cpcc_string readString(		const cpcc_char *aKey, const cpcc_char *aDefaultValue);
	bool		readBool(		const cpcc_char *aKey, const bool aDefaultValue);
	long		readLongint(	const cpcc_char *aKey, const long aDefaultValue);
	double		readReal(		const cpcc_char *aKey, const double aDefaultValue);
	void		writeString(	const cpcc_char *aKey, const cpcc_char *aValue);
	void		writeBool(		const cpcc_char *aKey, const bool aValue);
	void		writeLongint(	const cpcc_char *aKey, const long int aValue);
	void		writeReal(		const cpcc_char *aKey, const double aValue);

	void		clear(void);
	bool		load(void);
	bool		save(void);

	void pauseSaving(void) { instantSaving = false; }
	void resumeSaving(void);
	
							  
};



class cpccPersistentVar
{
protected:
	cpccSettings *	mIniPtr;
	cpcc_string		mKey;
public:
	cpccPersistentVar(cpccSettings *aIniPtr, const cpcc_char *aKey): mIniPtr(aIniPtr), mKey(aKey)
	{
		assert(aIniPtr && "#8251: INI pointer is null when constructing cpccPersistentVariable");
	}
};


class cpccPersistentVar_real: public cpccPersistentVar
{
public:
	cpccPersistentVar_real(cpccSettings *aIniPtr, const cpcc_char *aKey): cpccPersistentVar(aIniPtr, aKey) { }
	const double read(const double aDefault) { return mIniPtr->readReal(mKey.c_str(), aDefault); }
	void write(const double aValue)			 { return mIniPtr->writeReal(mKey.c_str(), aValue);  }
};


class cpccPersistentVar_bool: public cpccPersistentVar
{
public:
	cpccPersistentVar_bool(cpccSettings *aIniPtr, const cpcc_char *aKey): cpccPersistentVar(aIniPtr, aKey) { }
	const bool read(const bool aDefault)	{ return mIniPtr->readBool(mKey.c_str(), aDefault); }
	void write(const bool aValue)			{ return mIniPtr->writeBool(mKey.c_str(), aValue);  }
};


class cpccPersistentVar_longint: public cpccPersistentVar
{
public:
	cpccPersistentVar_longint(cpccSettings *aIniPtr, const cpcc_char *aKey): cpccPersistentVar(aIniPtr, aKey) { }
	const long int read(const long int aDefault)	{ return mIniPtr->readLongint(mKey.c_str(), aDefault); }
	void write(const long int aValue)				{ return mIniPtr->writeLongint(mKey.c_str(), aValue);  }
};


class cpccPersistentVar_string: public cpccPersistentVar
{
public:
	cpccPersistentVar_string(cpccSettings *aIniPtr, const cpcc_char *aKey): cpccPersistentVar(aIniPtr, aKey) { }
	const cpcc_string read(const cpcc_char *aDefault)	{ return mIniPtr->readString(mKey.c_str(), aDefault); }
	void write(const cpcc_char * aValue)				{ return mIniPtr->writeString(mKey.c_str(), aValue);  }
};


class cpccPersistentVar_SelfTest
{
public:
	static void selfTest(void) 
	{ 
		std::cout << "cpccPersistentVar_SelfTest starting\n";
		{ // writing
			
			cpccSettings settingsUser("testCompanyName", "testSoftwareName", cpccSettings::scopeCurrentUser);
			cpccSettings settingsApp("testCompanyName","testSoftwareName", cpccSettings::scopeAllUsers);
			
			std::cout << "cpccPersistentVar_SelfTest point1\n";
			
			cpccPersistentVar_bool tmpIsRegistered(&settingsApp, (cpcc_char *)"isRegistered");
			tmpIsRegistered.write(true);

			cpccPersistentVar_string tmpUserName(&settingsUser, (cpcc_char *)"username");
			tmpUserName.write("John");
			
			std::cout << "cpccPersistentVar_SelfTest point2\n";
		}


		{	// reading
			cpccSettings settingsUser("testCompanyName", "testSoftwareName", cpccSettings::scopeCurrentUser);
			cpccSettings settingsApp((cpcc_char *)"testCompanyName",(cpcc_char *)"testSoftwareName", cpccSettings::scopeAllUsers);

			std::cout << "cpccPersistentVar_SelfTest point3\n";
			
			cpccPersistentVar_bool tmpIsRegistered(&settingsApp, (cpcc_char *)"isRegistered");
			assert(tmpIsRegistered.read(false) && "SelfTest #8524: cpccPersistentVar_bool");

			std::cout << "cpccPersistentVar_SelfTest point4\n";
			
			cpccPersistentVar_string tmpUserName(&settingsUser, "username");
			assert(tmpUserName.read("unknown").compare("John")==0 && "SelfTest #8524: cpccPersistentVar_string");
		}
		
		std::cout << "cpccPersistentVar_SelfTest ended\n";
	}
	
};