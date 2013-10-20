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
	cpccSettings(cpcc_char *aCompanyName, cpcc_char *aSoftwareName, settingsScope aScope=scopeCurrentUser);
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



