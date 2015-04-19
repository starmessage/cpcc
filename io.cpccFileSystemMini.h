﻿/*  *****************************************
 *  File:		cpccFileSystemMini.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
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
	const cpcc_char *	getClassVersion(void) { return _T("1.07"); };
#if defined(cpccFileSystemMini_DoSelfTest)
	static void			selfTest(void);
#endif


protected:
	

public:
	
	// folder functions --------------------------------

	bool createFolder(const cpcc_char *  aFoldername);
	bool createFolder(const cpcc_string& aFoldername) { return createFolder(aFoldername.c_str()); }
	
	bool folderExists(const cpcc_char *  aFoldername) const;
	bool folderExists(const cpcc_string& aFoldername) const { return folderExists(aFoldername.c_str()); };

	const cpcc_string getFolder_Desktop(void);
	const cpcc_string getFolder_SystemsTemp(void);
	const cpcc_string getFolder_UsersTemp(void);
	const cpcc_string getFolder_Fonts(void);
	const cpcc_string getFolder_CommonAppData(void);
	const cpcc_string getFolder_UserData(void) const;
	const cpcc_string getFolder_UserHome(void) const;
#ifdef _WIN32
	const cpcc_string getFolder_Windows(void) const;
#endif
	// file functions --------------------------------

#if defined(__APPLE__)
	mode_t              getFileOrFolderPermissions_OSX(const cpcc_char *aFilename);
	bool                createFolder_Linux(const cpcc_char *aFilename, const mode_t permissions);
    const std::string   expandTilde_OSX(const char *aPath);
#endif
	
	bool fileExists(const cpcc_char * aFilename) const;
	
	cpccFileSize_t getFileSize(const cpcc_char *aFilename); 
	
	/// returns the number of bytes written, or a negative number in case of error
	cpccFileSize_t	writeToFile(const cpcc_char *aFilename, const char *buffer, const cpccFileSize_t bufSize, const bool appendToFile=true);
	
	/// returns the number of bytes read, or a negative number in case of error
	cpccFileSize_t	readFromFile(const cpcc_char *aFilename, char *buffer, const cpccFileSize_t bufSize);
	
	/// the destFile must be a file specification, not a folder specification
	bool copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile);
	bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew);
	bool createEmptyFile(const cpcc_char * aFilename);
	bool fileAppend(const cpcc_char* aFilename, const cpcc_char *txt);
	bool deleteFile(const cpcc_char * filename);
	bool copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder);	
	
		
	// date functions --------------------------------
	time_t		 getFileModificationDate(const cpcc_char * aFilename) const;
	
	// Other functions --------------------------------
	const cpcc_string getAppFullPath(void);
	const cpcc_string getAppFullPathFilename(void);
	const cpcc_string getAppFilename(void);
	const cpcc_string getFileSystemReport(void);

};


/// convenience class with additional calling parameters
class cpccFileSystemMiniEx : public cpccFileSystemMini
{
public:
	virtual bool createEmptyFile(const cpcc_string & aFilename) { return cpccFileSystemMini::createEmptyFile(aFilename.c_str()); };
	virtual bool deleteFile(const cpcc_string& aFilename)  { return cpccFileSystemMini::deleteFile(aFilename.c_str()); };
	cpccFileSize_t getFileSize(const cpcc_string &aFilename) { return cpccFileSystemMini::getFileSize(aFilename.c_str()); };
	virtual bool fileExists(const cpcc_string &aFilename)	{ return cpccFileSystemMini::fileExists(aFilename.c_str()); };

	virtual bool copyFile(const cpcc_string& sourceFile, const cpcc_string& destFileOrFolder) { return cpccFileSystemMini::copyFile(sourceFile.c_str(), destFileOrFolder.c_str()); };
	virtual bool fileAppend(const cpcc_char* aFilename, const cpcc_string& text) { return cpccFileSystemMini::fileAppend(aFilename, text.c_str()); };
	virtual bool fileAppend(const cpcc_string& aFilename, const cpcc_string& text) { return cpccFileSystemMini::fileAppend(aFilename.c_str(), text.c_str()); };

};