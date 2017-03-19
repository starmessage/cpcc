/*  *****************************************
 *  File:		cpccPathHelper.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include "cpccUnicodeSupport.h"

//////////////////////////////////////////////////////////
///		class cpccPathHelper
//////////////////////////////////////////////////////////



/** A small and portable (cross platform) C++ class 
	with basic filepath modification functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h
*/


/*
	https://developer.apple.com/library/content/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/FileSystemOverview/FileSystemOverview.html
UNIX-specific directories. 
The directories in this category are inherited from traditional UNIX installations. 
They are an important part of the system’s BSD layer but are more useful to software developers than end users. 
Some of the more important directories that are hidden include:
	/bin—Contains essential command-line binaries. Typically, you execute these binaries from command-line scripts.
	/dev—Contains essential device files, such as mount points for attached hardware.
	/etc—Contains host-specific configuration files.
	/sbin—Contains essential system binaries.
	/tmp—Contains temporary files created by apps and the system.
	/usr—Contains non-essential command-line binaries, libraries, header files, and other data.
	/var—Contains log files and other files whose content is variable. (Log files are typically viewed using the Console app.)

*/

/*
MAC directories
On the Mac, the correct location to store persistent user-related files for your application is in a directory 
with the same name as your application in the Application Support directory for the current user.
/Users/person/Library/Application Support/ExampleApp/
If you want to store user-related preferences, it is generally better to store them in the NSUserDefaults.
The correct way to get the path to the Application Support directory is to use the 
NSSearchPathForDirectoriesInDomains function passing NSApplicationSupportDirectory for the search path 
and NSUserDomainMask for the domain.
*/


/// path manipulation helper functions
class cpccPathHelper
{
private:
	/// if true then a the selftest function is called automatically on app start
	enum { config_RunSelfTest = true };

public:	

	static void				selfTest(void);
		
	static const cpcc_char	getPreferredPathDelimiter(void);
	static const cpcc_char*	getAllPathDelimiters(void);

	static void				removeLeadingPathDelimiter(cpcc_string &aPath);
	static void				removeTrailingPathDelimiter(cpcc_string &aPath);
	static void				addTrailingPathDelimiter(cpcc_string &aFolder);
	
	static cpcc_string		getParentFolderOf(const cpcc_string &aFullpathFilename);
	static cpcc_string		getParentFolderOf(const cpcc_char *aFilename) { return getParentFolderOf(cpcc_string(aFilename)); };
	
	static cpcc_string		extractFilename(const cpcc_string &aFullpathFilename);
	
	/// Returns a file path with a different extension. The newExtension parameter can be specified with or without a leading '.' 
	static cpcc_string		replaceExtension(const cpcc_char *aFilename, const cpcc_char *newExtension);
	static cpcc_string		getExtension(const cpcc_char *aFilename);

	/// concatenates two paths
	static cpcc_string		pathCat(const cpcc_char *a, const cpcc_char *b);
	
	static bool				endsWithPathDelimiter(const cpcc_char *aPath);
	static bool				startsWithPathDelimiter(const cpcc_char *aPath);
};



