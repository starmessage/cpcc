/*  *****************************************
 *  File:		cpccFileSystemMini.h
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <mutex>

#include "cpccUnicodeSupport.h"
#include "io.cpccFileSystemL1.h"
#include "core.cpccIdeMacros.h" 


#define 	cpccFileSystemMini_DoSelfTest	true
// If the above is set to FALSE the selftest code is bypassed (not compiled at all)
// See also the config_RunSelfTest. If TRUE, the selftest code is compiled but not called.



/** A small and portable (cross platform) C++ class with basic file system functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h, cpccUnicodeSupport.h
	
	It was created because the boost::filesystem is too complicated and needs considerable effort 
	to be included in your application.
*/
class cpccFileSystemMini: public cpccFileSystemL1
{
    
public:	// class metadata and selftest

    
#if defined(cpccFileSystemMini_DoSelfTest)
	static void			selfTest(void);
#endif


public:
	
	// folder functions --------------------------------

	static bool createFolder(const cpcc_char *  aFoldername);
	static bool	createFolder(const cpcc_string& aFoldername) { return createFolder(aFoldername.c_str()); }
	
	// todo: move them to fs.cpccSystemFolders
	// static cpcc_string getFolder_SystemsTemp(void);
	


    // file functions --------------------------------

	/// returns the number of bytes written, or a negative number in case of error
	static size_t	writeToFile(const cpcc_char *aFilename, const char *buffer, const size_t bufSize, const bool appendToFile=true);
	
	/// returns the number of bytes read, or a negative number in case of error
	static size_t	readFromFile(const cpcc_char *aFilename, char *buffer, const size_t bufSize);
	

	/// the destFile must be a file specification, not a folder specification
	static bool copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile);
	static bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew);
	static bool createEmptyFile(const cpcc_char * aFilename);
	// static bool	createEmptyFile(const cpcc_string & aFilename) { return createEmptyFile(aFilename.c_str()); };
	static bool appendTextFile(const cpcc_char* aFilename, const cpcc_char *txt);
    static bool writeTextFile(const cpcc_char* aFilename, const cpcc_char *aTxt, const bool inUTF8);
    inline static bool fileContainsText(const cpcc_char *fn, const cpcc_char *txt);

	#ifdef _WIN32
	    #ifdef UNICODE
	        static bool appendTextFile(const cpcc_char* aFilename, const char *txt) { wchar_from_char wtxt(txt); return appendTextFile(aFilename, wtxt.get()); }
	    #endif
	#endif

    

	static bool deleteFile(const cpcc_char * filename);
	
	static bool copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder);
	// static bool	copyFile(const cpcc_string& sourceFile, const cpcc_string& destFileOrFolder) { return copyFile(sourceFile.c_str(), destFileOrFolder.c_str()); };

		
	// date functions --------------------------------
	static time_t		 getFileModificationDate(const cpcc_char * aFilename);
	
	// Path functions
	
	
	// Other functions --------------------------------
	static cpcc_string getAppFullPathFilename(void);
    #if !defined(cpccTARGET_IOS)
        static cpcc_string getAppFullPath(void);
    #endif
	static cpcc_string getAppFilename(void);
	static cpcc_string getFileSystemReport(void);

    static cpcc_string getTempFilename(void);

};


// todo: merge this class with cpccPathHelper (some functions that do not require internal state will be static)
class cpccPathString: public cpcc_string
{
public:
	// enum standardFolderIds { sfNone = 0, sfCommonAppData, sfUsersTemp, sfUserData };

public: // constructors
    explicit cpccPathString(const cpcc_char * aPath=NULL) noexcept: cpcc_string( (aPath)? aPath: (cpcc_char *)"") 	{	}
	explicit cpccPathString(const cpcc_string & aPath) : cpcc_string(aPath) 	{	}
	// explicit cpccPathString(const standardFolderIds aFolderID);

public: // functions
    
	bool	pathExists(void)	const { return (cpccFileSystemL1::fileExists(c_str()) || cpccFileSystemL1::folderExists(c_str())); }
	void	appendPathSegment(const cpcc_char* aPathSegment);

	// this is already defined in the parent class
    inline const cpccPathString & operator =(const cpcc_string& aPath)
    {   assign(aPath);
        return *this;
    }
	
    
};

//////////////////////////////////////////////////////
//
//  cpccFileSystemMini implementation
//
///////////////////////////

bool    cpccFileSystemMini::fileContainsText(const cpcc_char *fn, const cpcc_char *txt)
{
    if (!fn)
        return false;
    
    if (!cpccFileSystemMini::fileExists(fn))
        return false;
    
    cpcc_ifstream thefile( fn);
    if (!thefile.good())
        return false;
    
    cpcc_string line;
    while (std::getline(thefile, line ))  // same as: while (getline( myfile, line ).good())
    {
        if (line.find(txt) != std::string::npos)
        {
            thefile.close();
            return true;
        }
    }
    thefile.close();
    return false;
    }
