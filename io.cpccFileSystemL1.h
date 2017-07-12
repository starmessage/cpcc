/*  *****************************************
 *  File:		io.cpccFileSystemL1.h
 *	Purpose:	Portable (cross-platform), light-weight, file system library.
 *				It was created because the boost::filesystem is too complicated and 
 *				needs considerable effort to be included in your application.
 *  			The "xxxxL1" class contains the functions that could be implemented 
 *				with minimum dependencies on other classes.
 *				Therefore, if the file system function you need is contained in L1, 
 *				you can use it in your project with minimum effort.
 *	Help:		The character type is passed as template parameter, so you can define 
 *				that from your code according to UNICODE or other considerations
 *				e.g. typedef cpccFileSystemL1<wchar_t>		cpccFS;
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <fstream>
#include <string>
#ifdef __APPLE__
    #include <sys/stat.h>
#endif


// template<typename cpccType_PChar>
class cpccFileSystemL1
{
public:	 // types
	typedef long		cpccType_FileSize;
	// typedef std::basic_string<cpccType_PChar>	cpccType_String;

public:  // functions   
	template<typename aPCharType>
	static cpccType_FileSize getFileSize(const aPCharType *aFilename)
	{
	std::ifstream f(aFilename, std::ios::binary | std::ios::ate);
	if (!f.good())
		return -1L;
	return static_cast<cpccType_FileSize>(f.tellg());
	/*
	// On a Windows system this is implemented with a Windows specific GetFileAttributesEx(),
	// on linux this is implemented as a lstat64(), and
	// on the Macintosh it uses the Mac specific call getattrlist().

	#ifdef _WIN32
	struct _tstat stat_buf;
	int rc = _tstat(aFilename, &stat_buf);
	#else
	struct stat stat_buf;
	int rc = stat(aFilename, &stat_buf);
	#endif
	return (rc == 0) ? stat_buf.st_size : -1;
	*/
	
	}

	template<typename aPCharType>
	static bool fileExists(const aPCharType *aFilename)
	{
	#ifdef _WIN32
		struct _stat fileinfo;

		// On success, zero is returned. 
		// On error, -1 is returned, and errno is set appropriately. 
		if (_tstat(aFilename, &fileinfo) == -1)
			return false;
		return ((fileinfo.st_mode & _S_IFREG) != 0);

	#elif defined(__APPLE__)
		struct stat fileinfo;

		// On success, zero is returned. 
		// On error, -1 is returned, and errno is set appropriately. 
		if (stat(aFilename, &fileinfo) == -1)
			return false;
		return (S_ISREG(fileinfo.st_mode));
	#else
		#error 	Error #5458: unsupported platform for fileExists()
	#endif

		return false;
	}

};



