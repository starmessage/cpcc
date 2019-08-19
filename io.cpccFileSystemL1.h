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
 *  Copyright: 	StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once


#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
    #include <unistd.h> // for rmdir()
#endif

#ifdef _WIN32
    #include <direct.h> // for rmdir()
#endif




struct sFileInformation
{
	typedef long long	cpccType_FileSize;

	cpccType_FileSize	size = 0;

	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions?view=vs-2019
	// you must include TYPES.H before STAT.H
	// _stat structure:
	// st_ctime 	Time of creation of file. Valid on NTFS but not on FAT formatted disk drives.
	// st_mtime 	Time of last modification of file.
	time_t				dateModified = 0;
	bool				isFile = false;
	bool				isFolder = false;
	bool				itemExists = false;
	bool				errorOccured = false;

	bool	getStat(char *fn)
	{
		return !errorOccured;
	}

	bool	getStat(wchar_t *fn)
	{
		return !errorOccured;
	}

};




// template<typename cpccType_PChar>
class cpccFileSystemL1
{
public:	 // types
	typedef long long		cpccType_FileSize;
	// typedef std::basic_string<cpccType_PChar>	cpccType_String;

public:  // functions   
	template<typename aPCharType>
	static cpccType_FileSize getFileSize(const aPCharType *aFilename)
	{
	//  long is 4 byte in Visual Studio, so you have to use long long to get correct file size for big files on Windows

	/*
	std::ifstream f(aFilename, std::ios::binary | std::ios::ate);
	if (!f.good())
		return -1L;
	
	// you need to seek before getting the result: myfile.seekg(0, ios::end);

	return static_cast<cpccType_FileSize>(f.tellg());
	*/

	
	// On a Windows system this is implemented with a Windows specific GetFileAttributesEx(),
	// on linux this is implemented as a lstat64(), and
	// on the Macintosh it uses the Mac specific call getattrlist().

		// large files:
		// __int42 is equivalent to long long
		// https://docs.microsoft.com/en-us/cpp/cpp/data-type-ranges?view=vs-2019

	#ifdef _WIN32
		struct _stat64 stat_buf;
		#ifdef UNICODE
			int rc = _wstat64(aFilename, &stat_buf);
		#else
			int rc = _stat64(aFilename, &stat_buf);
		#endif
	#else
		struct stat stat_buf;
		int rc = stat(aFilename, &stat_buf);
	#endif
	return (rc == 0) ? stat_buf.st_size : -1;
	
	}

    
	template<typename aPCharType>
	static bool fileExists(const aPCharType *aFilename)
	{
        if (!aFilename)
            return false;
	#ifdef _WIN32
		// _stat() does not work on WinXP.
		/*
		struct _stat fileinfo;

		// On success, zero is returned. 
		// On error, -1 is returned, and errno is set appropriately. 
		if (_tstat(aFilename, &fileinfo) == -1)
			return false;
		return ((fileinfo.st_mode & _S_IFREG) != 0);
		*/
		
		// check that the file system object exists and that it is not a directory
		DWORD dwAttrib = GetFileAttributes(aFilename);
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

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

    
    template<typename aPCharType>
    inline static bool deleteFolder(const aPCharType * aFoldername);
    
    
    template<typename aPCharType>
    inline static bool folderExists(const aPCharType * aFoldername);

};


///////////////////////////////
//
//  cpccFileSystemL1 implementation
//
///////////////////////////////

template<typename aPCharType>
bool cpccFileSystemL1::deleteFolder(const aPCharType * aFoldername)
{
    if (!aFoldername)
        return false;
    
    // Windows: To recursively delete the files in a directory, use the SHFileOperation function.
	#ifdef UNICODE // for Unicode Windows 
		return _wrmdir(aFoldername) == 0;
	#else
		return rmdir(aFoldername) == 0;
	#endif
}


template<typename aPCharType>
bool cpccFileSystemL1::folderExists(const aPCharType * aFoldername)
{
    if (!aFoldername)
        return false;
#ifdef _WIN32
    DWORD attrib = GetFileAttributes(aFoldername);
    return (! ( attrib == 0xFFFFFFFF || !(attrib & FILE_ATTRIBUTE_DIRECTORY) ) );
    // Other way:
    // return (PathIsDirectory( aFilename ) == FILE_ATTRIBUTE_DIRECTORY);
    
#elif defined(__APPLE__)
    struct stat fileinfo;
    if (stat(aFoldername, &fileinfo) == -1)
    {    // On success, zero is returned.
        // On error, -1 is returned, and errno is set appropriately.
        return false;
    }
    else
        return (S_ISDIR(fileinfo.st_mode));
    
#else
    #error     Error #5414: unsupported platform for folderExists()
#endif
    return false;
}
