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
#include "cpccUnicodeSupport.h"




#define 	cpccFileSystemMini_DoSelfTest	true
// If the above is set to FALSE the selftest code is bypassed (not compiled at all)
// See also the config_RunSelfTest. If TRUE, the selftest code is compiled but not called.

typedef long cpccFileSize_t;



/** A small and portable (cross platform) C++ class with basic file system functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h, cpccUnicodeSupport.h
	
	It was created because the boost::filesystem is too complicated and needs considerable effort 
	to be included in your application.
*/
class cpccFileSystemMini
{
public:	// class metadata and selftest

    
#if defined(cpccFileSystemMini_DoSelfTest)
	static void			selfTest(void);
#endif


protected:
	

public:
	
	// folder functions --------------------------------

	static bool createFolder(const cpcc_char *  aFoldername);
	static bool	createFolder(const cpcc_string& aFoldername) { return createFolder(aFoldername.c_str()); }
	
	static bool folderExists(const cpcc_char *  aFoldername);
	static bool	folderExists(const cpcc_string& aFoldername) { return folderExists(aFoldername.c_str()); };
	

	static cpcc_string getFolder_Desktop(void);
	static cpcc_string getFolder_SystemsTemp(void);
	static cpcc_string getFolder_UsersTemp(void);
	static cpcc_string getFolder_UsersCache(void);
	static cpcc_string getFolder_Fonts(void);
	
	static cpcc_string getFolder_UserHome(void);
#ifdef _WIN32
	static cpcc_string getFolder_Windows(void);
#endif

    // file functions --------------------------------

	
	static cpccFileSize_t 	getFileSize(const cpcc_char *aFilename); 
	static cpccFileSize_t	getFileSize(const cpcc_string &aFilename) { return getFileSize(aFilename.c_str()); };

	/// returns the number of bytes written, or a negative number in case of error
	static size_t	writeToFile(const cpcc_char *aFilename, const char *buffer, const cpccFileSize_t bufSize, const bool appendToFile=true);
	
	/// returns the number of bytes read, or a negative number in case of error
	static cpccFileSize_t	readFromFile(const cpcc_char *aFilename, char *buffer, const cpccFileSize_t bufSize);
	
	static bool	fileExists(const cpcc_char * aFilename);
	static bool	fileExists(const cpcc_string &aFilename) { return fileExists(aFilename.c_str()); };

	/// the destFile must be a file specification, not a folder specification
	static bool copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile);
	static bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew);
	static bool createEmptyFile(const cpcc_char * aFilename);
	static bool	createEmptyFile(const cpcc_string & aFilename) { return createEmptyFile(aFilename.c_str()); };
	static bool appendTextFile(const cpcc_char* aFilename, const cpcc_char *txt);
	static bool	appendTextFile(const cpcc_char* aFilename, const cpcc_string& text) { return appendTextFile(aFilename, text.c_str()); };
	static bool	appendTextFile(const cpcc_string& aFilename, const cpcc_string& text) { return appendTextFile(aFilename.c_str(), text.c_str()); };

	static bool deleteFile(const cpcc_char * filename);
	static bool	deleteFile(const cpcc_string& aFilename) { return deleteFile(aFilename.c_str()); };

	static bool copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder);
	static bool	copyFile(const cpcc_string& sourceFile, const cpcc_string& destFileOrFolder) { return copyFile(sourceFile.c_str(), destFileOrFolder.c_str()); };

		
	// date functions --------------------------------
	static time_t		 getFileModificationDate(const cpcc_char * aFilename);
	
	// Path functions
	
	
	// Other functions --------------------------------
	static cpcc_string getAppFullPath(void);
#ifdef __APPLE__
	static cpcc_string getAppBundlePath(void);
#endif
	static cpcc_string getAppFullPathFilename(void);
	static cpcc_string getAppFilename(void);
	static cpcc_string getFileSystemReport(void);

};



class cpccPathString: public cpcc_string
{
public:
	// enum standardFolderIds { sfNone = 0, sfCommonAppData, sfUsersTemp, sfUserData };

public: // constructors
    explicit cpccPathString(const cpcc_char * aPath=NULL) : cpcc_string( (aPath)? aPath: (cpcc_char *)"") 	{	}
	explicit cpccPathString(const cpcc_string & aPath) : cpcc_string(aPath) 	{	}
	// explicit cpccPathString(const standardFolderIds aFolderID);

public: // functions
	bool	pathExists(void)	const { return (cpccFileSystemMini::fileExists(c_str()) || cpccFileSystemMini::folderExists(c_str())); }
	void	appendPathSegment(const cpcc_char* aPathSegment);

    inline const cpccPathString & operator =(const cpcc_string& aPath)
    {   assign(aPath);
        return *this;
    }
    
};


