/*  *****************************************
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
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
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
	const cpcc_char *	getClassVersion(void) { return _T("1.06"); };
#if defined(cpccFileSystemMini_DoSelfTest)
	static void			selfTest(void);
#endif


protected:
	

public:
	
	// folder functions --------------------------------
		// folder functions --------------------------------
	virtual bool createFolder(const cpcc_char *  aFoldername);
	virtual bool createFolder(const cpcc_string& aFoldername) { return createFolder(aFoldername.c_str()); }
	
	virtual bool folderExists(const cpcc_char *  aFoldername);
	virtual bool folderExists(const cpcc_string& aFoldername) { return folderExists(aFoldername.c_str()); };

	virtual const cpcc_string getFolder_Desktop(void);
	virtual const cpcc_string getFolder_Temp(void);
	virtual const cpcc_string getFolder_Fonts(void);
	virtual const cpcc_string getFolder_CommonAppData(void);
	virtual const cpcc_string getFolder_UserData(void);
	virtual const cpcc_string getFolder_UserHome(void);
	
	// file functions --------------------------------
#if defined(__APPLE__)
	mode_t getFileOrFolderPermissions(const cpcc_char *aFilename);
	bool createFolderLinux(const cpcc_char *aFilename, const mode_t permissions);
#endif
	
	virtual bool fileExists(const cpcc_char * aFilename);
	virtual bool fileExists(const cpcc_string &aFilename)	{ return fileExists(aFilename.c_str()); };

	cpccFileSize_t getFileSize(const cpcc_char *aFilename); 
	cpccFileSize_t getFileSize(const cpcc_string &aFilename) { return getFileSize(aFilename.c_str()); };

	/// returns the number of bytes written, or a negative number in case of error
	cpccFileSize_t	writeToFile(const cpcc_char *aFilename, const char *buffer, const cpccFileSize_t bufSize, const bool appendToFile=true);
	
	/// returns the number of bytes read, or a negative number in case of error
	cpccFileSize_t	readFromFile(const cpcc_char *aFilename, char *buffer, const cpccFileSize_t bufSize);
	
	/// the destFile must be a file specification, not a folder specification
	bool copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile);

	bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew);


	virtual bool createEmptyFile(const cpcc_char * aFilename);
	virtual bool createEmptyFile(const cpcc_string & aFilename) {	return createEmptyFile(aFilename.c_str());	};


	virtual bool fileAppend(const cpcc_char* aFilename, const cpcc_char *txt);
	virtual bool fileAppend(const cpcc_string aFilename, const cpcc_string text) { return fileAppend(aFilename.c_str(), text.c_str());  };
	
	virtual bool deleteFile(const cpcc_char * filename);
	virtual bool deleteFile(const cpcc_string aFilename)  {  return deleteFile(aFilename.c_str()); };
	
	virtual bool copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder);	
	virtual bool copyFile(const cpcc_string &sourceFile, const cpcc_string &destFileOrFolder) {	return copyFile(sourceFile.c_str(), destFileOrFolder.c_str()); }; 

		
	
	// Other functions --------------------------------
	virtual const cpcc_string getAppFullPath(void);
	virtual const cpcc_string getAppFullPathFilename(void);
	virtual const cpcc_string getAppFilename(void);
	
	virtual const cpcc_string getFileSystemReport(void);

};



