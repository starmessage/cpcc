/*  *****************************************
 *  File:		fs.cpccUserFolders.cocoa.mm
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2019 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:   https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include "fs.cpccUserFolders.h"
#include <Foundation/Foundation.h>

/*  user's home directory, even on sandboxed applications
 
    #include <sys/types.h>
    #include <pwd.h>

    const char *home = getpwuid(getuid())->pw_dir;
    NSString *path = [[NSFileManager defaultManager]
                      stringWithFileSystemRepresentation:home
                      length:strlen(home)];
    NSString *realHomeDirectory = [[NSURL fileURLWithPath:path isDirectory:YES] path];
 */


std::string cpccUserFolders::getUsersTempDir(std::string* err)
{
    /*
         Objective-C:
         NSString * NSTemporaryDirectory ( void );
         Returns the path of the temporary directory for the current user.
         
         see also:
         http://www.cocoawithlove.com/2009/07/temporary-files-and-folders-in-cocoa.html
         */
    
        /*
         ToDo:
         under macos 10.7 the NSTemporaryDirectory() gives a message in the terminal:
         objc[277]: Object 0x103129120 of class __NSCFString autoreleased with no pool in place - just leaking - break on objc_autoreleaseNoPool() to debug
         objc[277]: Object 0x1031186f0 of class __NSCFData autoreleased with no pool in place - just leaking - break on objc_autoreleaseNoPool() to debug
         */
        // std::cout << "leak:before NSTemporaryDirectory()\n";
        NSString *tempDir = NSTemporaryDirectory();
        // std::cout << "leak:after NSTemporaryDirectory()\n";
        if (tempDir == nil) // NSTemporaryDirectory() can return nil
            tempDir = @"/tmp";
        
		return [tempDir UTF8String];
}


std::string cpccUserFolders::getUsersCacheDir(std::string* err)
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    /*
     The directory returned by this method may not exist. 
     This method simply gives you the appropriate location for the requested directory. 
     Depending on the application’s needs, it may be up to the developer to create the 
     appropriate directory and any in between.
     */
    if ([paths count]>0)
        return [[paths objectAtIndex:0] UTF8String];
    else
        if (err)
            err->append("#5821: Failed at NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask)");

	return "";
}


std::string cpccUserFolders::getDesktop(std::string *err)
{   // 1.
    // There's no guarantee that the user's desktop is at $HOME/Desktop
    // The proper way to get it is via NSSearchPathForDirectoriesInDomains
    // 2.
    // For sandboxed applications this folder is returned
    // ~/Library/Container/com.yourcompany.yourappname/data/desktop
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES );
    if ([paths count]>0)
        return [[paths objectAtIndex:0] UTF8String];
    else
        if (err)
            *err = "Error #5723 in getDesktopFolder()";
    return "";
}
    
    
std::string cpccUserFolders::getUserData(std::string* err) 
{
    std::string ph;
    
    // The directory returned by this method may not exist.
    // This method simply gives you the appropriate location for the requested directory.
    // Depending on the application’s needs, it may be up to the developer to create the appropriate directory and any in between.
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    if ([paths count]>0)
    {
        ph = [[paths objectAtIndex:0] UTF8String];
        ph.append("/Preferences");
    }
    else
        if (err)
            *err = "Error #5723e in getUserData()";

    //std::cout << "getFolder_UserData() returned:" << ph << std::endl;
    return ph;
}

