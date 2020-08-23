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
#include <ctime>

#ifdef __APPLE__
    #include <unistd.h> // for rmdir()
#endif

#ifdef _WIN32
    #include <direct.h> // for rmdir()
	#include <Windows.h> // for GetFileAttributesEx()
#endif




struct sFileInformation
{
	typedef long long	cpccFileSize_t;
	
	cpccFileSize_t		fileSize = 0;

	time_t				dateModified = 0;
	bool				isFile = false;
	bool				isFolder = false;
	bool				isReadOnly = false;
	bool				itemExists = false;
	// bool				errorOccured = false;
	
};

#ifdef _WIN32

// /////////////////////////////////
//
//  cpccFileSystemWin 
//
// /////////////////////////////////


class cpccFileSystemWin
{
public:
	std::basic_string<TCHAR>	errorDescription;

	static const time_t filetime2time_t(const FILETIME& ft);


	template<typename aPCharType>
	static unsigned long getFileInfo(const aPCharType* aFilename, sFileInformation &fileInfo)
	{
		// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/ns-fileapi-win32_file_attribute_data
		WIN32_FILE_ATTRIBUTE_DATA fileInfoWin;
		BOOL result = GetFileAttributesEx(aFilename, GetFileExInfoStandard, &fileInfoWin); // Returns non zero if succeed 
		if (!result) // If the function fails, the return value is zero (0). To get extended error information, call GetLastError.
			return GetLastError();
		
		LARGE_INTEGER size;
		size.HighPart = fileInfoWin.nFileSizeHigh;
		size.LowPart = fileInfoWin.nFileSizeLow;
		fileInfo.fileSize = size.QuadPart;

		// https://docs.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants
		fileInfo.isFolder	=		fileInfoWin.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY;
		fileInfo.isFile		= !fileInfo.isFolder;
		fileInfo.isReadOnly = fileInfoWin.dwFileAttributes | FILE_ATTRIBUTE_READONLY;

		fileInfo.itemExists = true;

		fileInfo.dateModified = filetime2time_t(fileInfoWin.ftLastWriteTime);

		return 0;
	}


	template<typename aPCharType>
	static unsigned long getFileSize(const aPCharType* aFilename, sFileInformation::cpccFileSize_t &fileSize)
	{
		sFileInformation fileInfo;
		unsigned long errorCode = getFileInfo(aFilename, fileInfo);
		if (errorCode)
		{
			return errorCode;
		}

		fileSize = fileInfo.fileSize;
		return 0;
	}

};



// /////////////////////////////////
//
//  cpccFileSystemWin implementation
//
// /////////////////////////////////

inline const time_t cpccFileSystemWin::filetime2time_t(const FILETIME& ft)
{
	// A FILETIME is the number of 100-nanosecond intervals since January 1, 1601.
	// A time_t is the number of 1 - second intervals since January 1, 1970.
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return ull.QuadPart / 10000000ULL - 11644473600ULL;
}

#endif


// /////////////////////////////////
//
//  cpccFileSystemL1 
//
// /////////////////////////////////


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
	// On a Windows system this is implemented with a Windows specific GetFileAttributesEx(), or getfilesizeex(),
	// on linux this is implemented as a lstat64(), and
	// on the Macintosh it uses the Mac specific call getattrlist().


    // for windows use getfilesizeex()
    // https://docs.microsoft.com/en-gb/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	#ifdef _WIN32
		/*
		// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions
		// seems to fail under windows server 2003 32 bit
		struct _stat64 stat_buf;
		#ifdef UNICODE
			int rc = _wstat64(aFilename, &stat_buf);
		#else
			int rc = _stat64(aFilename, &stat_buf);
		#endif

		return (rc == 0) ? stat_buf.st_size : -1;
		*/
		cpccType_FileSize fileSize;
		unsigned long result = cpccFileSystemWin::getFileSize(aFilename, fileSize);
		if (result != 0)
			fileSize = -1;
		return fileSize;
	#else
		struct stat stat_buf;
		int rc = stat(aFilename, &stat_buf);
		return (rc == 0) ? stat_buf.st_size : -1;
	#endif
	
	
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


// /////////////////////////////////
//
//  cpccFileSystemL1 implementation
//
// /////////////////////////////////

template<typename aPCharType>
bool cpccFileSystemL1::deleteFolder(const aPCharType * aFoldername)
{
    if (!aFoldername)
        return false;
    
    // Windows: To recursively delete the files in a directory, use the SHFileOperation function.
	#ifdef UNICODE // for Unicode Windows 
		return _wrmdir(aFoldername) == 0;
	#else
        #ifdef _WIN32
            // 'rmdir': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _rmdir.
            return _rmdir(aFoldername) == 0;
        #else
            return rmdir(aFoldername) == 0;
        #endif
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
