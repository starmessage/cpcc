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
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include "cpccUnicodeSupport.h"
#include "cpccPlatformAPI.h"

//////////////////////////////////////////////////////////
///		class cpccFileSystemMini
//////////////////////////////////////////////////////////



/** A small and portable (cross platform) C++ class with basic file system functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h
	
	It was created because the boost::filesystem is too complicated and needs considerable effort 
	to be included in your application.
*/





class cpccFileSystemMini
{
public:	// class metadata and selftest
	const cpcc_char *	getClassVersion(void) { return _T("1.05"); };
	static void			selfTest(void);

	// if true then a the selftest function is called automatically on app start
	enum { config_RunSelfTest=true };

protected:
	cpccPlatformAPI platformAPI;

public:
	
	// folder functions --------------------------------
	virtual bool folderExists(const cpcc_char * aFilename);
	virtual bool folderExists(const cpcc_string& aFilename) { return folderExists(aFilename.c_str()); };	

	virtual const cpcc_string getFolder_Desktop(void);
	virtual const cpcc_string getFolder_Temp(void);
	virtual const cpcc_string getFolder_Fonts(void);
	
	// file functions --------------------------------
	virtual bool createEmptyFile(const cpcc_char * aFilename);
	virtual bool createEmptyFile(const cpcc_string & aFilename) {	return createEmptyFile(aFilename.c_str());	};

	virtual bool fileExists(const cpcc_char * aFilename);
	virtual bool fileExists(const cpcc_string &aFilename)	{ return fileExists(aFilename.c_str()); };

	virtual bool fileAppend(const cpcc_char* aFilename, const cpcc_char *txt);
	virtual bool fileAppend(const cpcc_string aFilename, const cpcc_string text) { return fileAppend(aFilename.c_str(), text.c_str());  };
	
	virtual bool deleteFile(const cpcc_char * filename);
	virtual bool deleteFile(const cpcc_string aFilename)  {  return deleteFile(aFilename.c_str()); };
	
	virtual bool copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder);	
	virtual bool copyFile(const cpcc_string &sourceFile, const cpcc_string &destFileOrFolder) {	return copyFile(sourceFile.c_str(), destFileOrFolder.c_str()); }; 

	virtual cpccFileSize_t getFileSize(const cpcc_string &aFilename)  {	return platformAPI.getFileSize(aFilename.c_str()); 	};
		
	
	// Other functions --------------------------------
	virtual const cpcc_string getAppFullPath(void);
	virtual const cpcc_string getAppFullPathFilename(void);
	virtual const cpcc_string getAppFilename(void);
	
	virtual const cpcc_string getFileSystemReport(void);

};



