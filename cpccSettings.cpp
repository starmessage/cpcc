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
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */



#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "cpccSettings.h"
#include "cpccFileSystemMini.h"
#include "cpccPathHelper.h"
#if defined(cpccSettings_DoSelfTest)
	#include "cpcc_SelfTest.h"
#endif



cpccSettings::cpccSettings(const cpcc_char *aCompanyName, const cpcc_char *aSoftwareName, const settingsScope aScope):
	instantSaving(true)
{
	// setup the filename
	cpccFileSystemMini	fs; 
	cpccPathHelper		ph;
	cpcc_string			settingsRootFolder;

	assert(cpcc_strlen(aCompanyName)>0 && "#5351: cpccSettings: blank company name");
	assert(cpcc_strlen(aSoftwareName)>0 && "#5351: cpccSettings: blank Software name");

	settingsRootFolder = aScope==scopeAllUsers? fs.getFolder_CommonAppData(): fs.getFolder_UserData();
	
	//std::cout << "settingsRootFolder:" << settingsRootFolder << "\n";
	
	assert(fs.folderExists(settingsRootFolder) && "#5381: settings folder does not exist");
	
#ifdef __APPLE__
	// apple prefixes the folders with a com.
	mFilename = ph.pathCat(settingsRootFolder.c_str(), "com.");
	mFilename.append(aCompanyName);
#else
	mFilename = ph.pathCat(settingsRootFolder.c_str(), aCompanyName);
#endif
	
	if (!fs.createFolder(mFilename))
		std::cerr << "During cpccSettings constructor could not create folder:" << mFilename << "\n";

	mFilename = ph.pathCat(mFilename.c_str(), aSoftwareName);
	mFilename.append(".ini");
	
	std::cout << "cpccSettings construction point 1\n";
	
	if (!load())
		std::cerr << "Error #1351: loading cpccSettings from file:" << mFilename << std::endl;
	
	std::cout << "cpccSettings construction point 2\n";
}

	
cpccSettings::~cpccSettings()
{
	if (!instantSaving)
		if (!save())
			std::cerr << "Error #1352: saving cpccSettings to file:" << mFilename << std::endl;
}


bool cpccSettings::load(void)
{
	// http://stackoverflow.com/questions/10951447/load-stdmap-from-text-file
	// http://stackoverflow.com/questions/16135285/iterate-over-ini-file-on-c-probably-using-boostproperty-treeptree
	// http://www.cplusplus.com/forum/beginner/29576/
	cpcc_string key, value;
	std::ifstream iniFile(mFilename.c_str());

	mSettings.clear();
	
	while(std::getline(iniFile, key, '='))
    {
		std::getline(iniFile, value);
		mSettings[key] = value;
    }


	return true;
}


cpcc_string cpccSettings::readString(const cpcc_char *aKey, const cpcc_char *aDefaultValue)
{
	return mSettings.count(aKey) ? mSettings[aKey] : cpcc_string(aDefaultValue);
}


long cpccSettings::readLongint(const cpcc_char *aKey, const long aDefaultValue)
{
	//		http://code.google.com/p/inih/source/browse/trunk/cpp/INIReader.cpp

    cpcc_string valstr = readString(aKey, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : aDefaultValue;
}


double cpccSettings::readReal(const cpcc_char *aKey, const double aDefaultValue)
{
    cpcc_string valstr = readString(aKey, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : aDefaultValue;
}


bool cpccSettings::readBool(const cpcc_char *aKey, const bool aDefaultValue)
{
	cpcc_string tmp=readString(aKey, aDefaultValue?"yes":"no");
	if (tmp.compare("yes")==0 || tmp.compare("true")==0 || tmp.compare("1")==0 || tmp.compare("on")==0)
		return true;

	if (tmp.compare("no")==0 || tmp.compare("false")==0 || tmp.compare("0")==0 || tmp.compare("off")==0)
		return false;

	return aDefaultValue;
}


void	cpccSettings::writeString(const cpcc_char *aKey, const cpcc_char *aValue)
{
	mSettings[cpcc_string(aKey)] = cpcc_string(aValue);

	if (instantSaving)
		if (!save())
			std::cerr << "Error #1352c: saving cpccSettings to file:" << mFilename << std::endl;
}


void	cpccSettings::writeBool(const cpcc_char *aKey, const bool aValue)
{
	writeString(aKey, aValue?"yes":"no");
}


void	cpccSettings::writeLongint(const cpcc_char *aKey, const long int aValue)
{
	std::stringstream ss;
	ss << aValue;
	writeString(aKey, ss.str().c_str());
}


void	cpccSettings::writeReal(const cpcc_char *aKey, const double aValue)
{
	char buf[100];
	#pragma warning(disable : 4996)
	sprintf(buf,"%.12f",aValue);
	writeString(aKey, buf);
}


void cpccSettings::clear(void)	
{
	mSettings.clear();
	if (instantSaving)
		if (!save())
			std::cerr << "Error #1352d: saving cpccSettings to file:" << mFilename << std::endl;
}


void cpccSettings::resumeSaving(void)
{
	instantSaving = true;
	if (!save())
		std::cerr << "Error #1353: saving cpccSettings to file:" << mFilename << std::endl;
}





bool cpccSettings::save(void)
{
	//	http://www.stev.org/post/2012/03/19/C++-Read-Write-stdmap-to-a-file.aspx

	#pragma warning(disable : 4996)
	FILE *fp = fopen(mFilename.c_str(), "w");
	if (!fp)
		return false;
		
	for(tKeysAndValues::iterator it = mSettings.begin(); it != mSettings.end(); it++) 
		fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
    fclose(fp);

	return true;
}



#if defined(cpccSettings_DoSelfTest)
void cpccSettings::selfTest(void) 
{
	std::cout << "cpccSettings::SelfTest starting\n";
	cpccFileSystemMini fs;
	double pi = 3.14159265359;

	{
		// writing 
		cpccSettings settingsUser("testCompanyName","testSoftwareName", scopeCurrentUser);
		cpccSettings settingsApp("testCompanyName","testSoftwareName", scopeAllUsers);

		settingsUser.writeString("testStringKeyA", "testStringValueA");
		assert(fs.fileExists(settingsUser.getFilename()) && "SelfTest #7711a: file does not exist");

		settingsUser.writeString("testStringKeyB", "tmpValue");
		settingsUser.writeString("testStringKeyB", "B");

		settingsUser.writeBool("testTrueKey", true);
		settingsUser.writeBool("testFalseKey", false);
		settingsUser.writeReal("pi", pi);
		settingsUser.writeLongint("twentythree", 23);

		settingsApp.writeString("AppSettingsOfSoftware", "testSoftwareName");
	}

	{
		// separate reading
		cpccSettings settingsUser("testCompanyName","testSoftwareName", scopeCurrentUser);
		cpccSettings settingsSystem("testCompanyName","testSoftwareName", scopeAllUsers);

		cpcc_string tmp = settingsUser.readString("testStringKeyA","default");
		assert(tmp.compare("testStringValueA")==0 && "SelfTest #7711b: readString error");

		tmp = settingsUser.readString("NonExistingKey","default");
		assert(tmp.compare("default")==0 && "SelfTest #7711c: readString error on default value");

		tmp = settingsUser.readString("testStringKeyB","default");
		assert(tmp.compare("B")==0 && "SelfTest #7711d: writeString error: does not update values");

		assert(settingsUser.readBool("testTrueKey",false)		&& "SelfTest #7711e: readBool error 1");
		assert(!settingsUser.readBool("testFalseKey",true)		&& "SelfTest #7711e: readBool error 2");
		assert(settingsUser.readBool("testMissingKey1",true)	&& "SelfTest #7711e: readBool error 3");
		assert(!settingsUser.readBool("testMissingKey2",false)	&& "SelfTest #7711e: readBool error 4");

		assert(settingsUser.readReal("pi",1.0)==pi	&& "SelfTest #7711f: readReal");
		assert(settingsUser.readLongint("twentythree",2)==23	&& "SelfTest #7711g: readLongint");
	}
		
	std::cout << "cpccSettings::SelfTest ended\n";
}
#endif


#if defined(cpccSettings_DoSelfTest)
	SELFTEST_BEGIN(cpccSettings_SelfTest)
		cpccPersistentVar_SelfTest::selfTest();
	SELFTEST_END
#endif


///////////////////////////////////////////////////////////////////////////


