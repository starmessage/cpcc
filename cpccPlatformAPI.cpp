/*  *****************************************
 *  File:		cpccPlatformAPI.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				
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

#include <stdio.h>
#include "cpccPlatformAPI.h"
#include <fstream> // std::ifstream


#ifdef UNICODE
	#define		cpcc_rename			_trename 		
 #else 
	#define		cpcc_rename			std::rename 	
 #endif


bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew)
{
	/*
	If the file referenced by dest_file exists prior to calling rename(), 
	the behavior is implementation-defined. On POSIX systems, the destination 
	file is removed. On Windows systems, the rename() fails.
	This creates issues when trying to write portable code. 
	*/
	return (cpcc_rename(filenameOld, filenameNew)==0);
};

/**
	to create an empty file (replacing any existing one) call this as
	writeToFile(aFilename, "", 0, false)
*/
cpccFileSize_t	cpccPlatformAPI::writeToFile(const cpcc_char *aFilename, const char *buffer, const cpccFileSize_t bufSize, const bool appendToFile)
{
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	if (!buffer)
		return -3;

	FILE * pFile;
	
	pFile = cpcc_fopen (aFilename, (appendToFile)? _T("ab") : _T("wb") );
	if (pFile==NULL) 
		return -1;
	
	cpccFileSize_t res=fwrite (buffer, 1, bufSize, pFile);
	if( ferror( pFile ) )      
		res=-2;
		
    fclose (pFile);
    return res;
};
	
	
cpccFileSize_t	cpccPlatformAPI::readFromFile(const cpcc_char *aFilename, char *buffer, const cpccFileSize_t bufSize)
{
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	if (!buffer)
		return -3;

	FILE * pFile;
	pFile = cpcc_fopen (aFilename, _T("rb"));
	if (pFile==NULL) 
		return -1;
	
	cpccFileSize_t res=fread(buffer, 1, bufSize, pFile);
	if( ferror( pFile ) )      
		res=-2;
		
    fclose (pFile);
    return res;
};


cpccFileSize_t cpccPlatformAPI::getFileSize(const cpcc_char *aFilename)
{	
	// http://www.codeproject.com/Articles/9016/Quick-and-Dirty-Series-C-FileSize-function
	// GetFileSizeEx()
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa364957%28v=vs.85%29.aspx

	std::ifstream f(aFilename, std::ios::binary | std::ios::ate);
	return static_cast<cpccFileSize_t>(f.tellg());
/*
#ifdef _WIN32
	struct _tstat stat_buf;
	int rc = _tstat(aFilename, &stat_buf);
#else
	struct stat stat_buf;
	int rc = stat(aFilename, &stat_buf);
#endif
	return (rc == 0) ? stat_buf.st_size : -1;
*/	
};


bool cpccPlatformAPI::deleteFile(const cpcc_char* aFilename)
{
	/*
	 http://www.cplusplus.com/reference/clibrary/cstdio/remove/
	 If the file is successfully deleted, a zero value is returned.
	 On failure, a nonzero value is reurned and the errno variable is 
	 set to the corresponding error code. A string interpreting 
	 this value can be printed to the standard error stream by a call to perror.
	 */ 
	 
#ifdef UNICODE
	 return (_wremove(aFilename)==0);
#else
	return (remove(aFilename)==0);
#endif
};

bool	cpccPlatformAPI::copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile)
{
	// You may use std::fopen, std::fread, std::fwrite, and std::fclose, 
	// all of which are part of the standard C++ library (#include <cstdio>, very portable)

	// http://msdn.microsoft.com/en-us/library/yeby3zcb%28v=vs.90%29.aspx

	char buf[4096];
	size_t size;
	
	#ifdef _WIN32
		#pragma warning( disable : 4996 )
	#endif
	FILE* source = cpcc_fopen(sourceFile, _T("rb"));
	if (!source) 
		return false;
	
	FILE* dest = cpcc_fopen(destFile, _T("wb"));
	if (!dest) 
	{
		fclose(source);
		return false;
	};

	bool errorOccured=false;
	while ((!errorOccured) && (size = fread(buf, 1, BUFSIZ, source))) 
		errorOccured = ( fwrite(buf, 1, size, dest) != size );
	
	fclose(source);
	fclose(dest);
	
	return (!errorOccured);

};

	

