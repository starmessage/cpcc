/*  *****************************************
 *  File:       fs.cpccFileSystem.h
 *	Purpose:    Portable (cross-platform), light-weight, file system library.
 *	            It was created because the boost::filesystem is too complicated and
 *	            needs considerable effort to be included in your application.
 *              This header only class, has only a few cross platform file system
 *              functions, but it is a stand alone header include.
 *	Help:       The character type is passed as template parameter, so you can define
 *	            that from your code according to UNICODE or other considerations
 *	            e.g. typedef cpccFileSystemL1<wchar_t>		cpccFS;
 *	*****************************************
 *  Library:    Cross Platform C++ Classes (cpcc)
 *  Copyright:  StarMessage software.
 *  License:    Free for opensource projects.
 *  	            Commercial license for closed source projects.
 *	Web:        http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:   https://github.com/starmessage/cpcc
 *	email:      sales -at- starmessage.info
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



// /////////////////////////////////
//
//  cpccFileSystem
//
// /////////////////////////////////


class cpccFileSystem
{


public:  // functions   

	template<typename aPCharType>
    static long long getFileSize(const aPCharType *aFilename);

    template<typename aPCharType>
    static time_t	getModificationDate(const aPCharType* aFilename);

	template<typename aPCharType>
    static bool fileExists(const aPCharType *aFilename);

    template<typename aPCharType>
    static bool folderExists(const aPCharType* aFoldername);

    template<typename aPCharType>
    static bool deleteFile(const aPCharType* aFilename);

    template<typename aPCharType>
    static bool deleteFolder(const aPCharType * aFoldername);
    
    template<typename aPCharType>
    static bool renameFile(const aPCharType* filenameOld, const aPCharType* filenameNew);

public: // utility functions
    #ifdef _WIN32
        static const time_t filetime2time_t(const FILETIME& ft);
    #endif
};


// /////////////////////////////////
//
//  cpccFileSystem implementation
//
// /////////////////////////////////

#ifdef _WIN32

    inline const time_t cpccFileSystem::filetime2time_t(const FILETIME& ft)
    {
        // A FILETIME is the number of 100-nanosecond intervals since January 1, 1601.
        // A time_t is the number of 1 - second intervals since January 1, 1970.
        ULARGE_INTEGER ull;
        ull.LowPart = ft.dwLowDateTime;
        ull.HighPart = ft.dwHighDateTime;
        return ull.QuadPart / 10000000ULL - 11644473600ULL;
    }

#endif

template<typename aPCharType>
inline time_t		cpccFileSystem::getModificationDate(const aPCharType* aFilename)
{	// http://linux.die.net/man/2/stat

    if (!fileExists(aFilename) && !folderExists(aFilename))
        return 0;

#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA fileInfoWin;
    BOOL result = GetFileAttributesEx(aFilename, GetFileExInfoStandard, &fileInfoWin); // Returns non zero if succeed
    if (!result) // If the function fails, the return value is zero (0). To get extended error information, call GetLastError.
    {
        // GetLastError();
        return 0;
    }
    return filetime2time_t(fileInfoWin.ftLastWriteTime);
#else
    // _stat does not work on XP
    cpcc_struct_stat attrib;         // create a file attribute structure
    cpcc_stat(aFilename, &attrib);     // get the attributes
    return attrib.st_mtime;

#endif

}


template<typename aPCharType>
inline long long cpccFileSystem::getFileSize(const aPCharType *aFilename)
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
        // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/ns-fileapi-win32_file_attribute_data
        WIN32_FILE_ATTRIBUTE_DATA fileInfoWin;
        BOOL result = GetFileAttributesEx(aFilename, GetFileExInfoStandard, &fileInfoWin); // Returns non zero if succeed
        if (!result) // If the function fails, the return value is zero (0). To get extended error information, call GetLastError.
        {
            // GetLastError();
            return 0;
        }
        
        LARGE_INTEGER size;
        size.HighPart = fileInfoWin.nFileSizeHigh;
        size.LowPart = fileInfoWin.nFileSizeLow;
        return size.QuadPart;
    #else
        struct stat stat_buf;
        int rc = stat(aFilename, &stat_buf);
        return (rc == 0) ? stat_buf.st_size : -1;
    #endif

}


template<typename aPCharType>
inline bool cpccFileSystem::fileExists(const aPCharType *aFilename)
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
        #error     Error #5458: unsupported platform for fileExists()
    #endif
    return false;
}


template<typename aPCharType>
inline bool cpccFileSystem::renameFile(const aPCharType* filenameOld, const aPCharType* filenameNew)
{
    if ((!filenameOld) || (!filenameNew))
        return false;
    /*
    If the file referenced by dest_file exists prior to calling rename(),
    the behavior is implementation-defined. On POSIX systems, the destination
    file is removed. On Windows systems, the rename() fails.
    This creates issues when trying to write portable code.
    */
    #ifdef UNICODE
        return (_trename(filenameOld, filenameNew)==0);
    #else
        return (std::rename(filenameOld, filenameNew)==0);
    #endif
}


template<typename aPCharType>
inline bool cpccFileSystem::deleteFile(const aPCharType* aFilename)
{
    if (!aFilename)
        return false;
    /*
     If the file is successfully deleted, a zero value is returned.
     On failure, a nonzero value is returned and the errno variable is
     set to the corresponding error code. A string interpreting
     this value can be printed to the standard error stream by a call to perror.
     */
    #ifdef UNICODE
        return (_wremove(aFilename)==0);
    #else
        return (remove(aFilename)==0);
    #endif

}


template<typename aPCharType>
inline bool cpccFileSystem::deleteFolder(const aPCharType * aFoldername)
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
inline bool cpccFileSystem::folderExists(const aPCharType * aFoldername)
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
