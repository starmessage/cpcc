/*  *****************************************
 *  File:		cpccPlatformAPI.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				cpccPlatformAPI is a class abstracting the raw plaform functions
 *				Further grouping and enapsulation of these API functions are done
 *				by other classes (e.g. miniFileSystem) that in their turn use the platformAPI
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


typedef long cpccFileSize_t;

class cpccPlatformAPI
{
public:	// class metadata and selftest
	const cpcc_char *	getClassVersion(void) { return _T("1.00"); };
	static void			selfTest(void) { };

protected:
	
public:
	// time functions --------------------------------
	
	// folder functions --------------------------------
	
	// file functions --------------------------------
	cpccFileSize_t getFileSize(const cpcc_char *aFilename); 
	
	/// returns the number of bytes written, or a negative number in case of error
	cpccFileSize_t	writeToFile(const cpcc_char *aFilename, const char *buffer, const cpccFileSize_t bufSize, const bool appendToFile=true);
	
	/// returns the number of bytes read, or a negative number in case of error
	cpccFileSize_t	readFromFile(const cpcc_char *aFilename, char *buffer, const cpccFileSize_t bufSize);
	
	/// the destFile must be a file specification, not a folder specification
	bool copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile);
	bool deleteFile(const cpcc_char* aFilename);
	bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew);

	// Other functions --------------------------------

};



