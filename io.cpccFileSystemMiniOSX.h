/*  *****************************************
 *  File:		cpccFileSystemMiniOSX.h
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
#include <pwd.h>
#include <unistd.h> // for access on unix, mac
#include <libproc.h>

#include "cpccUnicodeSupport.h"


class fileSystemOSX_helper
{
    
public:

static const cpcc_string getUserFolder_OSX(void)
{
    struct passwd* pwd = getpwuid(getuid());
    if (pwd)
        return std::string(pwd->pw_dir);
    return  std::string("~");
}


static const bool createFolder_Linux(const cpcc_char *aFilename, const mode_t permissions)
{
    
    
    /* tutorial:
     int mkdir(const char *pathname, mode_t mode);
     The argument mode specifies the permissions to use.
     It is modified by the processís umask in the usual way:
     the permissions of the created directory are (mode & ~umask & 0777).
     */
    mode_t previous_umask = umask(0);
    int rc=mkdir( aFilename, permissions );
    // chmod(): This POSIX function is deprecated. Use the ISO C++ conformant _chmod instead.
    // http://msdn.microsoft.com/en-us/library/1z319a54.aspx
    //	chmod(aFilename, permissions);
    umask(previous_umask);
    if (rc!=0)
        std::cerr << "Error no " <<  rc << " (" << strerror(rc) << ") while creating directory:" << aFilename << "\n";
    return (rc == 0);
}


static const bool  startsWithTilde_OSX(const char *aPath)
{
    if (aPath)
        if (strlen(aPath)>0)
            if (aPath[0]=='~')
                return true;
    return false;
}


static const std::string  expandTilde_OSX(const char *aPath)
{
    std::string result(aPath);
    if (result.length()>0)
        if (result.at(0)=='~')
            result.replace(0,1, getUserFolder_OSX());
    return result;
}


static const bool folderExists_OSX(const cpcc_char * aFoldername)
{
    cpcc_string finalPath(expandTilde_OSX(aFoldername));

    struct stat fileinfo;
    if (stat(finalPath.c_str(), &fileinfo) == -1)
    {	// On success, zero is returned.
        // On error, -1 is returned, and errno is set appropriately.
        return false;
    }
    else
        return (S_ISDIR(fileinfo.st_mode));
}



static const mode_t getFileOrFolderPermissions_OSX(const cpcc_char *aFilename)
{
    cpcc_string finalPath(expandTilde_OSX(aFilename));

    struct stat statInfo;
    stat(finalPath.c_str(), &statInfo);
    //  Octal numbers begin with 0.  hex numbers begin with 0x.
    return statInfo.st_mode & 0777;
}
    
};
