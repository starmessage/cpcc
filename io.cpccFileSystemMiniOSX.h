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
#include <TargetConditionals.h>

#if TARGET_OS_MAC
    #if TARGET_OS_IPHONE
        #include <Foundation/Foundation.h>
    #else
        #include <libproc.h>
        #include <Cocoa/Cocoa.h>
    #endif
#endif

#include "cpccUnicodeSupport.h"
#include "fs.cpccUserFolders.h"


class fileSystemOSX_helper
{
    
public:



static const bool createFolder(const cpcc_char *aFilename, const mode_t permissions)
{
    NSString *path = [NSString stringWithCString:aFilename encoding:NSUTF8StringEncoding];
    NSError *error = nil;
    
    if (![[NSFileManager defaultManager] createDirectoryAtPath:path
                                   withIntermediateDirectories:YES
                                                    attributes:nil
                                                         error:&error])
    {
        NSLog(@"Create directory error: %@", error);
        return false;
    }
    return true;
}
    
    
#if !(TARGET_OS_IPHONE)
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
#endif

    
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
            result.replace(0,1, cpccUserFolders::getUserHomeDir());
    return result;
}


static const mode_t getFileOrFolderPermissions_OSX(const cpcc_char *aFilename)
{
    
#if TARGET_OS_IOS
    NSString *path = [NSString stringWithCString:aFilename encoding:NSUTF8StringEncoding];
    NSFileManager *fm = [NSFileManager defaultManager];
    NSError *error = nil;
    // Get the current permissions
    NSDictionary *currentPerms = [fm attributesOfFileSystemForPath:path error:&error];
    if (currentPerms)
    {
        // The corresponding value is an NSNumber object.
        // Use the shortValue method to retrieve the integer value for the permissions.
        mode_t permissions = [currentPerms[NSFilePosixPermissions] shortValue];
        return permissions;
        /*
        // Update the permissions with the new permission
        NSMutableDictionary *attributes = [currentPerms mutableCopy];
        attributes[NSFilePosixPermissions] = @(0444);
        if (![fm setAttributes:attributes ofItemAtPath:path error:&error]) {
            NSLog(@"Unable to make %@ read-only: %@", path, error);
         */
    }
    else
        NSLog(@"Unable to read permissions for %@: %@", path, error);

    return 0;
    
#else
    cpcc_string finalPath(expandTilde_OSX(aFilename));
    
    struct stat statInfo;
    stat(finalPath.c_str(), &statInfo);
    //  Octal numbers begin with 0.  hex numbers begin with 0x.
    return statInfo.st_mode & 0777;

#endif

}

    
};
