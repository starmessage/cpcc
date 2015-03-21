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
#include <errno.h>

#include "cpccSettings.h"
#include "cpccAppInfo.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#if defined(cpccSettings_DoSelfTest)
	#include "cpcc_SelfTest.h"
#endif


// encode characters to fit to the INI file (e.g. multiline text)
const cpcc_char *encodedCharacters[][2] =
{
    //             normal     ->       encoded
    {(cpcc_char *)  "=",  (cpcc_char *) "\\="},
    {(cpcc_char *)  "\\", (cpcc_char *) "\\\\"},
    {(cpcc_char *)  "\n", (cpcc_char *) "\\n"},
    {(cpcc_char *)  "\r", (cpcc_char *) "\\r"}
};


class iniFileStringEncodings
{
private:
    // a pity the whole std library does not contain a ready function for this
    static void findAndReplaceAll( cpcc_string& source, const cpcc_char* find, const cpcc_char* replace )
    {
        if (!find || !replace)
            return;
        
        size_t findLen = strlen(find);
        size_t replaceLen = strlen(replace);
        size_t pos = 0;
        
        while ((pos = source.find(find, pos)) != std::string::npos)
        {
            source.replace( pos, findLen, replace );
            pos += replaceLen;
        }
    }
    
public:
    
    static void    encodeStr(cpcc_string &str)
    {
        for (int i=0 ; i< sizeof(encodedCharacters) / sizeof(encodedCharacters[0]); ++i)
            findAndReplaceAll(str, encodedCharacters[i][0], encodedCharacters[i][1]);
    }
    
    static void    decodeStr(cpcc_string &str)
    {
        for (int i=0 ; i< sizeof(encodedCharacters) / sizeof(encodedCharacters[0]); ++i)
            findAndReplaceAll(str, encodedCharacters[i][1], encodedCharacters[i][0]);
    }
    
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//		stringConversions
///////////////////////////////////////////////////////////////////////////////////////////////////
class stringConversions
{
public:

	static double fromStr(const cpcc_char* strValue, const double aDefaultValue) 
	{
		char* end;
		double n = strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}


	static float fromStr(const cpcc_char* strValue, const float aDefaultValue)
	{
		char* end;
		float n = (float)strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}

    
	static long fromStr(const cpcc_char*  strValue, const long aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		long n = strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

	static int fromStr(const cpcc_char*  strValue, const int aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		int n = strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

	static bool fromStr(const cpcc_char*  strValue, const bool aDefaultValue)  { return fromStr(cpcc_string(strValue), aDefaultValue); }
	static bool fromStr(const cpcc_string&  strValue, const bool aDefaultValue)
	{ 
		if (strValue.compare("yes") == 0 || strValue.compare("true") == 0 || strValue.compare("1") == 0 || strValue.compare("on") == 0)
			return true;

		if (strValue.compare("no") == 0 || strValue.compare("false") == 0 || strValue.compare("0") == 0 || strValue.compare("off") == 0)
			return false;

		return aDefaultValue;
	}
		
    static cpcc_string toStr(const bool value) { return cpcc_string(value?"yes":"no"); }
    
    static cpcc_string toStr(const long int value)
    {   std::stringstream ss; ss << value; return ss.str(); }
    
    static cpcc_string toStr(const int value)
    {   std::stringstream ss; ss << value; return ss.str(); }
    
    static cpcc_string toStr(const float value)
    {
        char buf[100];
        #pragma warning(disable : 4996)
        sprintf(buf,"%.12f",value);
        return cpcc_string(buf);
    }
    
    static cpcc_string toStr(const double value)
    {
        char buf[200];
        #pragma warning(disable : 4996)
        sprintf(buf,"%.12f",value);
        return cpcc_string(buf);
    }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
///////////////////////////////////////////////////////////////////////////////////////////////////

cpccSettings::cpccSettings(const cpcc_char *aCompanyName, const cpcc_char *aSoftwareName, const settingsScope aScope):
	instantSaving(true)
{
	// std::cout << "cpccSettings constructor\n";
	
	// setup the filename
	cpccFileSystemMini	fs; 
	cpccPathHelper		ph;
	
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
	cpccFileSystemMini	fs; 
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
        iniFileStringEncodings::decodeStr(key);
        iniFileStringEncodings::decodeStr(value);
		mSettings[key] = value;
    }
	
	return true;
}


bool cpccSettings::save(void)
{
    //	http://www.stev.org/post/2012/03/19/C++-Read-Write-stdmap-to-a-file.aspx
    
#pragma warning(disable : 4996)
    FILE *fp = fopen(mFilename.c_str(), "w");
    if (!fp)
    {
        // If it fails returns NULL", yes. The global variable errno (found in <errno.h>)
        // contains information about what went wrong;
        // you can use perror() to print that information as a readable string.
        std::cerr << "Error saving file " << mFilename << " Error message:"<< strerror(errno) << "\n";
        return false;
    }
    
    cpcc_string key, value;
    for(tKeysAndValues::iterator it = mSettings.begin(); it != mSettings.end(); ++it)
    {
        key = it->first;    iniFileStringEncodings::encodeStr(key);
        value = it->second; iniFileStringEncodings::encodeStr(value);
        fprintf(fp, "%s=%s\n", key.c_str(), value.c_str());
    }
    fclose(fp);
    
    return true;
}


cpcc_string cpccSettings::read(const cpcc_char *aKey, const cpcc_char *aDefaultValue)
{
	return mSettings.count(aKey) ? mSettings[aKey] : cpcc_string(aDefaultValue);
}


template <typename T>
T cpccSettings::read(const cpcc_char *aKey, const T aDefaultValue)
{
	return stringConversions::fromStr(read(aKey, "").c_str(), aDefaultValue);
}




template <typename T>
void		cpccSettings::write(const cpcc_char *aKey, const T aValue)
{
	write(aKey, stringConversions::toStr(aValue));
}


void	cpccSettings::write(const cpcc_char *aKey, const cpcc_char * aValue)
{
    mSettings[cpcc_string(aKey)] = aValue;
    
    if (instantSaving)
        if (!save())
            std::cerr << "Error #1352c: saving cpccSettings to file:" << mFilename << std::endl;
}



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
	cpccFileSystemMini fs;
	double pi = 3.14159265359;
    float  bigFloat = 1.3456798e16f;
    
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

		cpccPersistentVar<int> tmpPersistentInt(settingsApp, "tmpPersInt");
		tmpPersistentInt.write(456);
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

		cpccPersistentVar<int> tmpPersistentInt(settingsSystem, "tmpPersInt");
		assert((tmpPersistentInt.read(-1) == 456) && "SelfTest #7711r: tmpPersistentInt error 1");
		assert((tmpPersistentInt.readAtIndex(3, -10) == 678) && "SelfTest #7711j: tmpPersistentInt error 2");
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
	SELFTEST_END
#endif


///////////////////////////////////////////////////////////////////////////


