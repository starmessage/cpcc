
/*  *****************************************
 *  File:		cpccGetComputerID.h
 *	Purpose:	Portable (cross-platform), light-weight, 
 *				ComputerID for license registration systems
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include "io.cpccFileSystemMini.h"
#include "cpccUnicodeSupport.h"
#include <sstream>
#ifdef __APPLE__
    #include <ext/hash_map>
    #include <tr1/functional>
    typedef std::tr1::hash<cpcc_string> portableHashString;
#elif defined(_WIN32)
    typedef std::hash<cpcc_string>      portableHashString;
#endif

/**	cpccGetComputerID: a cross platform GetComputerID()
	Algorithm: 
	1)	read the GUID from a secret file that the application knows.
		If the file does not exist (first run of application) create a GUID and store it to the secret file 
	2)	To prevent a copy of the secret file between clients, 
		one part of the GUID must depend on the modification datetime of a standard file or folder of the OS.
		Only a reinstallation of the OS would change the date of a folder, e.g. windows (WIN) or applications (MAC)
	3)  If the GUID read from the file does not match none the OS modification datetime, delete the file with the GUID
*/


struct sOsFile 
{
	char * containerPath;
	char * file;
};

/* some examples
	Characteristic OS file:C:\Windows\regedit.exe
	modification date of OS folder:Tue, 14.07.2009 04:14:30
*/

sOsFile	listOfWindowsFiles[] =
{	{ (char *)"<WINDOWS>", (char *)"notepad.exe" },
	{ (char *)"<WINDOWS>", (char *)"regedit.exe" },
	{ (char *)"<WINDOWS>", (char *)"explorer.exe" }
};


sOsFile	listOfOSXFiles[] =
{	{ (char *)"/Applications/", (char *)"Calculator.app/Contents/MacOSX/Calculator" }, // 23/2/2015
    
	{ (char *)"/Applications/", (char *)"SimpleText.app/Contents/MacOSX/SimpleText" }   // 2/11/2007
};

//  http://www.cplusplus.com/reference/functional/hash/

template<int UNIQUE_SEED>
class cpccGetComputerID
{
private:
	cpccFileSystemMiniEx fs;

	struct		components 
				{ 
						cpcc_string computerID; 
						time_t		OStimestamp; 
						time_t		SelfTimeStamp; 
				};

	cpcc_string combineComponents(const components c);
	components	splitComponents(const cpcc_string& combined);
	cpcc_string getSecretFilename(void);
	cpcc_string createGUID(void);
	bool		loadFromSecretFile(components &c);
	bool		saveToSecretFile(const components &c);

public:
    cpcc_string getSelfFilename(void)    { return fs.getAppFullPathFilename(); }
    time_t      getSelfModifyDate(void)	 { return fs.getFileModificationDate(getSelfFilename().c_str()); }
	time_t      getOSModifyDate(void)	 { return fs.getFileModificationDate(getCharacteristicOSFile().c_str()); }


	const cpcc_string getHash(const cpcc_string &aTxt)
	{	// 	http://www.cplusplus.com/reference/functional/hash/
		portableHashString str_hash;
		size_t result = str_hash(aTxt);

		std::stringstream ss;
		ss << result;

		return cpcc_string(ss.str());
	}


	const cpcc_string getComputerName(void)
	{
#ifdef _WIN32
		cpcc_char name[255]; DWORD size;
		size = sizeof(name) - 1;
		GetComputerName(name, &size);
		return cpcc_string(name);
#endif
#ifdef __APPLE__
		cpcc_char name[_POSIX_HOST_NAME_MAX + 1];
        if (gethostname(name, sizeof name) == -1 )
            return cpcc_string("getComputerName failed.");
        else
            return cpcc_string(name);
        
		/*
		[[NSDictionary
		dictionaryWithContentsOfFile:@"/var/db/SystemConfiguration/preferences.xml"
		] valueForKeyPath:@"System.System.ComputerName"];
		*/

		/*
		[[NSProcessInfo processInfo] hostName]
		*/
#endif

	}


	const cpcc_string getCharacteristicOSFile(void) 
	{
		cpcc_string theFilename;
		#if defined(__APPLE__)
		for (int n = 0; n < sizeof(listOfOSXFiles) / sizeof(listOfOSXFiles[0]); ++n)
		{
			theFilename = listOfOSXFiles[n].containerPath;
			theFilename.append(listOfOSXFiles[n].file);
			if (fs.fileExists(theFilename))
				return theFilename;
		}

		#elif defined(_WIN32)

		for (int n = 0; n < sizeof(listOfWindowsFiles) / sizeof(listOfWindowsFiles[0]); ++n)
		{
			theFilename = listOfWindowsFiles[n].containerPath;
			if (theFilename.compare("<WINDOWS>") == 0)
				theFilename = fs.getFolder_Windows();
			theFilename.append(listOfWindowsFiles[n].file);
			if (fs.fileExists(theFilename))
				return theFilename;
		}
		#else
			#error Unexpected platform in cpccGetComputerID
		#endif

		return cpcc_string("");
	}



	

	const cpcc_string getAstandardOSfolder(void) const
	{
	#if defined(__APPLE__)
		return cpcc_string("/Applications");
	#elif defined(_WIN32)
		cpcc_char buffer[300];
		GetWindowsDirectory( buffer, 300);
		return cpcc_string(buffer);
	#else
		#error Unexpected platform in cpccGetComputerID
	#endif
	}


	cpcc_string getID(const int msec)	
	{
		static cpcc_string cachedID;
		if (cachedID.length()>0)
			return cachedID;

		// if file 
		return cachedID;
	}
    
    /* move to the cpp file 
    static void selfTest(void)
    {
        logObjectLife logThis("cpccGetComputerID::selfTest()");
        
        cpccGetComputerID<33> mid;
        infoLog().addf("current executable (self):%s",mid.getSelfFilename().c_str());
        
        time_t adate = mid.getSelfModifyDate();
		#pragma warning( disable : 4996 )
        std::tm * ptm = std::localtime(&adate);
        char buffer[42];
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        infoLog().addf("modification date of current executable (self):%s",buffer);
		
		//////////////////////////

		infoLog().addf("Characteristic OS file:%s", mid.getCharacteristicOSFile().c_str());
		adate = mid.getOSModifyDate();
		#pragma warning( disable : 4996 )
        ptm = std::localtime(&adate);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        infoLog().addf("modification date of OS folder:%s",buffer);
    }
    */
	
};
