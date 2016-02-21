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




///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
///////////////////////////////////////////////////////////////////////////////////////////////////

cpccSettings::cpccSettings(const cpcc_char *aCompanyName, const cpcc_char *aSoftwareName, const settingsScope aScope):
	instantSaving(true)
{
	// std::cout << "cpccSettings constructor\n";
	
	// setup the filename
	cpccFileSystemMiniEx	fs; 
	cpccPathHelper			ph;
	
	assert(cpcc_strlen(aCompanyName)>0 && "#5351: cpccSettings: blank company name");
	assert(cpcc_strlen(aSoftwareName)>0 && "#5351: cpccSettings: blank Software name");

	cpcc_string settingsRootFolder = aScope==scopeAllUsers? fs.getFolder_CommonAppData(): fs.getFolder_UserData();
	//std::cout << "settingsRootFolder:" << settingsRootFolder << "\n";
	assert(fs.folderExists(settingsRootFolder) && "#5381: settings folder does not exist");
	
	// problem with writing:
	// http://stackoverflow.com/questions/6993527/c-mac-os-x-cannot-write-to-library-application-support-appname
	
	cpcc_string companySubFolder(aCompanyName);
	
	mFilename = ph.pathCat(settingsRootFolder.c_str(), companySubFolder.c_str());
	if (!fs.createFolder(mFilename))
		std::cerr << "During cpccSettings constructor could not create folder:" << mFilename << "\n";
	
	mFilename = ph.pathCat(mFilename.c_str(), aSoftwareName);	
	mFilename.append(".ini");
	std::cout << "cpccSettings constructor: filename:" << mFilename << "\n";
	
	if (!load())
		std::cerr << "Error #1351: loading cpccSettings from file:" << mFilename << std::endl;
	
	//std::cout << "cpccSettings construction point 2\n";
}

	
cpccSettings::~cpccSettings()
{
	if (!instantSaving)
		if (!save())
			std::cerr << "Error #1352: saving cpccSettings to file:" << mFilename << std::endl;
}


bool cpccSettings::load(void)
{
	cpccFileSystemMiniEx	fs; 
	if (!fs.fileExists(mFilename))
		return true;
	
	
	std::ifstream iniFile(mFilename.c_str());
	if (!iniFile.is_open())
	{
		std::cerr << "#8551: Could not open file:" << mFilename << "\n";
		return false;
	}
		
	mSettings.clear();
	cpcc_string key, value;
    
	while(std::getline(iniFile, key, '='))
    {
		std::getline(iniFile, value);
		stringConversions::decodeStrFromINI(value);
		mSettings[key] = value;
    }
	
	return true;
}


bool cpccSettings::save(void)
{
    //	http://www.stev.org/post/2012/03/19/C++-Read-Write-stdmap-to-a-file.aspx
    
    #pragma warning(suppress : 4996)
    FILE *fp = fopen(mFilename.c_str(), "w");
    if (!fp)
    {
        // If it fails returns NULL", yes. The global variable errno (found in <errno.h>)
        // contains information about what went wrong;
        // you can use perror() to print that information as a readable string.
		#pragma warning(suppress : 4996)
		std::cerr << "Error saving file " << mFilename << " Error message:"<< strerror(errno) << "\n";
        return false;
    }
    
    cpcc_string key, value;
    for(tKeysAndValues::iterator it = mSettings.begin(); it != mSettings.end(); ++it)
    {
		key = it->first;   
		value = it->second; stringConversions::encodeStrForINI(value);
        fprintf(fp, "%s=%s\n", key.c_str(), value.c_str());
    }
    fclose(fp);
    
    return true;
}


void	cpccSettings::write(const cpcc_char *aKey, const cpcc_char * aValue)
{
    mSettings[cpcc_string(aKey)] = aValue;
    
    if (instantSaving)
        if (!save())
            std::cerr << "Error #1352c: saving cpccSettings to file:" << mFilename << std::endl;
}

#ifdef  BYPASS_RELEASE_ERROR
void		cpccSettings::write(const cpcc_char *aKey, const bool aValue)   {   write(aKey, stringConversions::toStr(aValue)); }
void		cpccSettings::write(const cpcc_char *aKey, const int aValue)   {   write(aKey, stringConversions::toStr(aValue)); }
void		cpccSettings::write(const cpcc_char *aKey, const long int aValue)   {   write(aKey, stringConversions::toStr(aValue)); }
#endif

template <typename T>
void		cpccSettings::write(const cpcc_char *aKey, const T aValue)  { write(aKey, stringConversions::toStr(aValue)); }





void cpccSettings::clear(void)	
{
	mSettings.clear();
	if (instantSaving)
		if (!save())
			std::cerr << "Error #1352d: saving cpccSettings to file:" << mFilename << std::endl;
}


void cpccSettings::resumeInstantSaving(void)
{
	instantSaving = true;
	if (!save())
		std::cerr << "Error #1353: saving cpccSettings to file:" << mFilename << std::endl;
}



#if defined(cpccSettings_DoSelfTest)
void cpccSettings::selfTest(void) 
{
	std::cout << "cpccSettings::SelfTest starting\n";
	cpccFileSystemMiniEx fs;
	double pi = 3.14159265359;
    float  bigFloat = 1.3456798e16f;
	const char * tmpTestString = "abc-καλημέρα=good\x0A\x0Dmorning to all.";
	{
		// writing 
		cpccSettings settingsUser("testCompanyName","testSoftwareName", scopeCurrentUser);
		cpccSettings settingsApp("testCompanyName","testSoftwareName", scopeAllUsers);

		settingsUser.write("testStringKeyA", "testStringValueA");
		assert(fs.fileExists(settingsUser.getFilename()) && "SelfTest #7711a: file does not exist");

		settingsUser.write("testStringKeyB", "tmpValue");
		settingsUser.write("testStringKeyB", "B");

		settingsUser.write("testTrueKey", true);
		settingsUser.write("testFalseKey", false);
		settingsUser.write("pi", pi);
		settingsUser.write("twentythree", 23);
        settingsUser.write("bigFloat", bigFloat);
		settingsApp.write("AppSettingsOfSoftware", "testSoftwareName");
		settingsApp.write("extremeString", tmpTestString);
		cpccPersistentVar<int> tmpPersistentInt(settingsApp, "tmpPersInt", 98);
		tmpPersistentInt = 456;
		tmpPersistentInt.writeAtIndex(3, 678);
	}

	{
		// separate reading
		cpccSettings settingsUser("testCompanyName","testSoftwareName", scopeCurrentUser);
		cpccSettings settingsSystem("testCompanyName","testSoftwareName", scopeAllUsers);

		cpcc_string tmp = settingsUser.read("testStringKeyA","default");
		assert(tmp.compare("testStringValueA")==0 && "SelfTest #7711b: readString error");

		tmp = settingsUser.read("NonExistingKey","default");
		assert(tmp.compare("default")==0 && "SelfTest #7711c: readString error on default value");

		tmp = settingsUser.read("testStringKeyB","default");
		assert(tmp.compare("B")==0 && "SelfTest #7711d: writeString error: does not update values");

		assert(settingsUser.read("testTrueKey",false)		&& "SelfTest #7711e: readBool error 1");
		assert(!settingsUser.read("testFalseKey",true)		&& "SelfTest #7711e: readBool error 2");
		assert(settingsUser.read("testMissingKey1",true)	&& "SelfTest #7711e: readBool error 3");
		assert(!settingsUser.read("testMissingKey2",false)	&& "SelfTest #7711e: readBool error 4");

		assert(settingsUser.read("pi",1.0)==pi	&& "SelfTest #7711f: readReal");
        assert(settingsUser.read("bigFloat",2.0)==bigFloat	&& "SelfTest #7711k: readReal bigFloat");
        
		assert(settingsUser.read("twentythree",2)==23	&& "SelfTest #7711g: readLongint");

		cpccPersistentVar<int> tmpPersistentInt(settingsSystem, "tmpPersInt", 92);
		assert((tmpPersistentInt == 456) && "SelfTest #7711r: tmpPersistentInt error 1");
		// assert((tmpPersistentInt.readAtIndex(3) == 678) && "SelfTest #7711j: tmpPersistentInt error 2");
		assert((tmpPersistentInt[3] == 678) && "SelfTest #7711j: tmpPersistentInt error 2");

		tmp = settingsSystem.read("extremeString", "----");
		assert(tmp.compare(tmpTestString) == 0 && "SelfTest #7711k: readString error");
		}
		
	std::cout << "cpccSettings::SelfTest ended\n";

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


