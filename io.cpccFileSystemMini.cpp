/*  *****************************************
 *  File:		cpccFileSystemMini.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

// do not compile this if added in the xcode project files.
// the .mm must be included as well
#if defined(_WIN32) || defined (IMPORTED_BY_io_cpccFileSystemMini_mm)

#include <assert.h>
#include <cstdio>
// #include <stdio.h>  // for tempnam()
// #include <stdlib.h> // for free()
#include <iostream>
#include <fstream>
#include <errno.h>
#include <locale>
#include <codecvt>

#ifdef _WIN32
	#include	<io.h> // for _access on windows
	#include	<Shlobj.h> // for SHGetFolderPath()
	#pragma comment(lib,"Shell32.lib") // instruct visual studio to link the library

	#include	<Shlwapi.h>
	#pragma comment(lib,"Shlwapi.lib") // instruct visual studio to link the library
	#include	<tchar.h>
	#include	<sys/types.h>
	#include	<sys/stat.h>

#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #include "io.cpccFileSystemMiniOSX.h"
    #include <CoreFoundation/CoreFoundation.h>
    #if TARGET_OS_IPHONE
        #include <Foundation/Foundation.h>
    #else
        #include <Carbon/Carbon.h> // for the DialogRef
    #endif
#endif

#include "core.cpccIdeMacros.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#include "io.cpccSystemFolders.h"
#if defined(cpccFileSystemMini_DoSelfTest)
	#include "cpcc_SelfTest.h"
#endif


// todo: std::tmpfile() is considered better
cpcc_string cpccFileSystemMini::getTempFilename(void)
{
#ifdef TARGET_OS_IPHONE // all IOS devices
    // we must get a folder inside the application’s temporary directory
    NSString *tempPath = NSTemporaryDirectory();
    NSString *tempFile = [tempPath stringByAppendingPathComponent:[[NSProcessInfo processInfo] globallyUniqueString]];
    return [tempFile UTF8String];
    // std::string tmpFolder([NSTemporaryDirectory() UTF8String]);
#endif

    cpcc_char name[L_tmpnam];
    
#ifdef _WIN32
    if (tmpnam_s(name, sizeof(name))==0)
        return name;

#else    // UNIX and MacOS solution
    
    // see also tempnam()
    // http://pubs.opengroup.org/onlinepubs/9699919799/functions/tempnam.html
    // 'tempnam' is deprecated
    /*
    char * namePtr = tempnam(NULL, NULL);
    std::string result(namePtr?namePtr:"");
    if (namePtr)
        free(namePtr);
    
    return result;
    */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
     // 'tmpnam' is deprecated:
    if (std::tmpnam(name))
        return name;
    #pragma GCC diagnostic pop
    
#endif
    return _T("");
}

// std::mutex _fileAppendMutex, _writeToFileMutex;
// std::mutex cpccFileSystemMini::fileAppendMutex_;
// static std::mutex fileAppendMutex_;

// main class

cpcc_string cpccFileSystemMini::getFileSystemReport(void)
{
	cpcc_string report( _T("File System Report by cpccFileSystemMini\n----------------------\n"));

    #if !(TARGET_OS_IPHONE)
        report.append(_T("App full path filename:")	+ getAppFullPathFilename() + _T("\n"));
        report.append(_T("App path:")        + getAppFullPath() + _T("\n"));
    
    #endif
    report.append(_T("App filename:")    + getAppFilename() + _T("\n"));
    
	report.append(_T("System's Temp folder:")	+ getFolder_SystemsTemp() + _T("\n"));
	report.append(_T("User's Temp folder:")	+ getFolder_UsersTemp() + _T("\n"));
    report.append(_T("Current dir:")	+ cpccSystemFolders::getFolder_CurrentDir() + _T("\n"));
	report.append(_T("User's home folder:")	+ getFolder_UserHome() + _T("\n"));
	report.append(_T("Desktop folder:") + getFolder_Desktop() + _T("\n"));
	report.append(_T("Fonts folder:")   + getFolder_Fonts() + _T("\n"));
	report.append(_T("AppData path:")	+ cpccSystemFolders::getFolder_CommonAppData() + _T("\n"));
    #if !(TARGET_OS_IPHONE)
        report.append(_T("UserData path:")	+ cpccSystemFolders::getFolder_UserData() + _T("\n"));
    #endif
    
	report.append(_T("End of file system report\n----------------------\n"));
	return report;
}


#ifdef _WIN32
cpcc_string cpccFileSystemMini::getFolder_Windows(void) 
{
	cpcc_char buffer[300+1];
	UINT rc = GetWindowsDirectory(buffer, 300);
	if (rc == 0 || rc > 300) 
		return _T("c:\\error-no3852-getting-windows-dir");
	return cpcc_string(buffer) + _T("\\");
}
#endif


cpcc_string cpccFileSystemMini::getFolder_UserHome(void) 
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g. C:\ProgramData
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, szPath))) 
	{
		cpcc_string result(szPath);
		cpccPathHelper::addTrailingPathDelimiter(result);
		return result;
	}
	std::cerr << "Error #6531 in getFolder_UserHome\n";
	
#elif defined(__APPLE__)
    return fileSystemOSX_helper::getUserFolder_OSX();
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_UserHome()");	
#endif	
	return cpcc_string( _T("") );
}



cpcc_string cpccFileSystemMini::getFolder_UsersCache(void)
{
#ifdef _WIN32	
	// is there a cache folder in windows? Until then, return the user's temp
	return  getFolder_UsersTemp();

#elif defined(__APPLE__)
	std::string ph;
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    /*
     The directory returned by this method may not exist. 
     This method simply gives you the appropriate location for the requested directory. 
     Depending on the application’s needs, it may be up to the developer to create the 
     appropriate directory and any in between.
     */
    if ([paths count]>0)
        ph = [[paths objectAtIndex:0] UTF8String];
    
	return ph;

#else
	assert(false && "Error #6753y: unsupported platform for getFolder_UsersCache()");

#endif
	return  cpcc_string(_T(""));
}


cpcc_string cpccFileSystemMini::getFolder_UsersTemp(void) 
{
	// getenv("TEMP"); // does not work on mac
	// getenv("TMPDIR): returns: /var/folders/zv/zvUjUH8BFX0Sb5mxkslqWU+++TI/-Tmp-/
    // TMPDIR is what Posix recommends, I think.
	
	#ifdef _WIN32	
		cpcc_char buffer[MAX_PATH+1]; 
		GetTempPath(MAX_PATH, buffer);	// this is the user's temp
		assert(buffer && _T("#6753a: GetTempPath() failed"));
		return  cpcc_string(buffer);
	
	#elif defined(__APPLE__)
        /*
         Objective-C:
         NSString * NSTemporaryDirectory ( void );
         Returns the path of the temporary directory for the current user.
         
         see also:
         http://www.cocoawithlove.com/2009/07/temporary-files-and-folders-in-cocoa.html
         */
		
    // std::string userTempFolder( fileSystemOSX_helper::expandTilde_OSX("~/Library/Caches/temp-cpcc"));
    // std::string userTempFolder( fileSystemOSX_helper::expandTilde_OSX("~/Library/Caches/com.StarMessageSoftware.StarMessage"));
        NSString *tmpDirectory = NSTemporaryDirectory();
        std::string userTempFolder = [tmpDirectory UTF8String];
    
		if (!folderExists(userTempFolder.c_str()))
			createFolder(userTempFolder.c_str());
		
		return userTempFolder;
	
	#else
		assert(false && "Error #6753c: unsupported platform for getFolder_UsersTemp()");	
		
	#endif
	return  cpcc_string( _T("") );
}


cpcc_string cpccFileSystemMini::getFolder_SystemsTemp(void) 
{
	// getenv("TEMP"); // does not work on mac
	// getenv("TMPDIR): returns: /var/folders/zv/zvUjUH8BFX0Sb5mxkslqWU+++TI/-Tmp-/
    // TMPDIR is what Posix recommends, I think.
	
	/* for OSX:
		Don't use tmpnam() or tempnam(). They are insecure. 
		Don't assume /tmp. 
		Use NSTemporaryDirectory() in conjunction with mkdtemp(). 
		NSTemporaryDirectory() will give you a better directory to use, however it can return nil. 
		NSString * tempDir = NSTemporaryDirectory();
		if (tempDir == nil)
			tempDir = @"/tmp";
	*/
	
	#ifdef _WIN32
		cpcc_char buffer[MAX_PATH+1]; 
		GetTempPath(MAX_PATH, buffer); // this is the user's temp
		assert(buffer && _T("#6753a: GetTempPath() failed"));
		return  cpcc_string(buffer);
	
	#elif defined(__APPLE__)
		char buffer[L_tmpnam +1];
    
        // the following 3 pragma directives is to suppress the warning that tmpnam is deprecated
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		char * pointer = tmpnam(buffer); // in MS VC this does not contain a folder. It is just a filename
		#pragma unused(pointer)
    
    
				// it returns the SYSTEM's temp
        #pragma GCC diagnostic pop
		assert(pointer && "#6753b: tmpnam() failed");
		cpccPathHelper ph;
		return ph.getParentFolderOf(std::string(buffer));
	
	#else
		assert(false && "Error #6753c: unsupported platform for getFolder_SystemsTemp()");	
		
	#endif
	return  cpcc_string( _T("") );
}


cpcc_string cpccFileSystemMini::getFolder_Fonts(void) 
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, szPath))) 
		{
		cpcc_string result(szPath);
		cpccPathHelper::addTrailingPathDelimiter(result);
		return result;
		}
	cpcc_cerr << _T("Error #4824 in cpccFileSystemMini::getFolder_Fonts\n");
	
#elif defined(__APPLE__)
	return  std::string("/library/fonts/");
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_Fonts()");	
#endif	
	return cpcc_string( _T("") );
}



bool cpccFileSystemMini::copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder) 
{
	/*
#ifdef __APPLE__
    cpcc_string destFile=fileSystemOSX_helper::expandTilde_OSX(destFileOrFolder);
#else
    cpcc_string destFile=destFileOrFolder;
#endif
    */

	if (folderExists(destFileOrFolder))	// if the destination is a folder, copy the file inside the folder
		{
		cpcc_string destFile = destFileOrFolder;
		cpccPathHelper::addTrailingPathDelimiter(destFile);
		destFile = destFile + cpccPathHelper::extractFilename(sourceFile);
		return copyFileToaFile(sourceFile, destFile.c_str());
		}

	return copyFileToaFile(sourceFile, destFileOrFolder);
}

bool cpccFileSystemMini::writeTextFile(const cpcc_char* aFilename, const cpcc_char *aTxt, const bool inUTF8)
{
    if (!aFilename)
        return false;

    cpcc_ofstream _file(aFilename);
    if (!_file.good())
    {
#pragma warning(suppress : 4996)
        cpcc_cerr << _T("Error saving file ") << aFilename << _T(" Error message:") << strerror(errno) << _T("\n");
        return false;
    }

    if (inUTF8) // write the BOM of UTF-8 and set locale
    {
        // _file << UTF8_BOM;
        //_file.write(BOM_UTF8, 3);

        // http://www.cplusplus.com/forum/beginner/107125/
        std::locale my_utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
        _file.imbue(my_utf8_locale);

        // _file << helper_to_utf8(aTxt, cpcc_strlen(aTxt));
    }


    _file << aTxt;

    _file.close();
    return true;
}

bool cpccFileSystemMini::appendTextFile(const cpcc_char* aFilename, const cpcc_char *txt)
{
// when mutex is used, an exception "mutex lock failed: Invalid argument" occurs when the log add() function is called on program termination
// #define USE_MUTEX_F
#ifdef USE_MUTEX_F
	static std::mutex fileAppendMutex_;
    std::lock_guard<std::mutex> autoMutex(fileAppendMutex_);
#endif
    
// #define USE_FOPEN_F
#ifdef  USE_FOPEN_F
    
	FILE *fp; 
	#pragma warning(suppress : 4996)
	fp= cpcc_fopen(aFilename,  _T("at")); // write append // todo: UNICODE
	if (!fp) return false;
	cpcc_fprintf(fp,_T("%s"),txt);
	fclose(fp);
	return true;
#else
    // std::ofstream outfile("thefile.txt", std::ios_base::app | std::ios_base::out);
    // ios::app = opens the file in append mode.
    // static std::ofstream _f; // with static is hangs in WinXP
	// std::ofstream _f;
	cpcc_ofstream _f;

    _f.open(aFilename, std::ios_base::app | std::ios_base::out);
    if (!_f.good())
        return false;
    
    _f << txt;
    // _f << fflush;
    _f.close();
    return true;
#endif
}



#ifdef UNICODE
	#define		cpcc_rename			_trename 		
 #else 
	#define		cpcc_rename			std::rename 	
 #endif



bool cpccFileSystemMini::createFolder(const cpcc_char * aFoldername) 
{
	if (folderExists(aFoldername))
		return true;

#ifdef _WIN32
	::CreateDirectory(aFoldername, NULL);
#elif defined(__APPLE__)
    /* it will probably fail if the folder starts with tilde (~)
        expand the tilde before calling the function
     */
	cpccPathHelper ph;

	cpcc_string finalPath(fileSystemOSX_helper::expandTilde_OSX( aFoldername));
	
	cpcc_string parentFolder = ph.getParentFolderOf(finalPath);
	
	int parentPermissions = fileSystemOSX_helper::getFileOrFolderPermissions_OSX(parentFolder.c_str());
	
	//std::cout << "permissions of " << parentFolder << " : " << parentPermissions << "\n";
	
	// http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
	// https://discussions.apple.com/thread/844719?start=0&tstart=0
	// http://www.linuxquestions.org/questions/programming-9/mkdir-in-c-setting-wrong-permissions-613250/

	// read/write/search permissions for owner and group, and with read/search permissions for others
	//mode_t p775 = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	// full permissions for all
	//mode_t p777 = S_IRWXU | S_IRWXG | S_IRWXO;
    #if TARGET_OS_IPHONE
        return fileSystemOSX_helper::createFolder(finalPath.c_str(), parentPermissions);
    #elif TARGET_OS_MAC
        return fileSystemOSX_helper::createFolder_Linux(finalPath.c_str(), parentPermissions);
    #endif
    
#endif

    return(folderExists(aFoldername));
}


bool cpccFileSystemMini::renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew)
{
	/*
	If the file referenced by dest_file exists prior to calling rename(), 
	the behavior is implementation-defined. On POSIX systems, the destination 
	file is removed. On Windows systems, the rename() fails.
	This creates issues when trying to write portable code. 
	*/
	return (cpcc_rename(filenameOld, filenameNew)==0);
}

/**
	to create an empty file (replacing any existing one) call this as
	writeToFile(aFilename, "", 0, false)
*/
size_t	cpccFileSystemMini::writeToFile(const cpcc_char *aFilename, const char *buffer, const size_t bufSize, const bool appendToFile)
{
    if (!buffer)
        return -3;

    //static std::mutex _writeToFileMutex;
    //std::lock_guard<std::mutex> autoMutex(_writeToFileMutex);
    #pragma warning(disable : 4996)
	FILE * pFile = cpcc_fopen (aFilename, (appendToFile)? _T("ab") : _T("wb") );  // todo: unicode
	if (pFile==NULL) 
		return -1;
	
	size_t res = fwrite(buffer, 1, bufSize, pFile);
	if( ferror( pFile ) )      
		res=-2;
		
    fclose (pFile);
    return res;
}
	
	
size_t	cpccFileSystemMini::readFromFile(const cpcc_char *aFilename, char *buffer, const size_t bufSize)
{
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	if (!buffer)
		return -3;

	FILE * pFile;
	pFile = cpcc_fopen(aFilename, _T("rb"));	// todo: unicode
	if (pFile==NULL) 
		return -1;
	
	size_t res=fread(buffer, 1, bufSize, pFile);
	if( ferror( pFile ) )      
		res=-2;
		
    fclose (pFile);
    return res;
}




bool cpccFileSystemMini::deleteFile(const cpcc_char* aFilename)
{
	/*
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
}


bool	cpccFileSystemMini::copyFileToaFile(const cpcc_char* sourceFile, const cpcc_char* destFile)
{
	// You may use std::fopen, std::fread, std::fwrite, and std::fclose, 
	// all of which are part of the standard C++ library (#include <cstdio>, very portable)
	// http://msdn.microsoft.com/en-us/library/yeby3zcb%28v=vs.90%29.aspx

	char buf[4096];
	size_t size;
	
	#ifdef _WIN32
		#pragma warning( disable : 4996 )
	#endif
	FILE* source = cpcc_fopen(sourceFile, _T("rb")); // todo: unicode
	if (!source) 
		return false;
	
	FILE* dest = cpcc_fopen(destFile, _T("wb")); // todo: unicode
	if (!dest) 
	{
		fclose(source);
		return false;
	}

	bool errorOccured=false;
	while ((!errorOccured) && (size = fread(buf, 1, BUFSIZ, source))) 
		errorOccured = ( fwrite(buf, 1, size, dest) != size );
	
	fclose(source);
	fclose(dest);
	
	return (!errorOccured);
}


bool cpccFileSystemMini::createEmptyFile(const cpcc_char * aFilename)
{
	return (writeToFile(aFilename, "", 0, false) == 0);
	// other way: std::ofstream tmpOut(aFilename); 
}


cpcc_string cpccFileSystemMini::getAppFilename(void)
{
	// remove the path part and leave only the filename
	return cpccPathHelper::extractFilename(getAppFullPathFilename());
}


#ifndef cpccTARGET_IOS
cpcc_string cpccFileSystemMini::getAppFullPath(void)
{
	return cpccPathHelper::getParentFolderOf(getAppFullPathFilename());
}
#endif


cpcc_string cpccFileSystemMini::getAppFullPathFilename(void)
{

#ifdef _WIN32
	_TCHAR fullPathfileName[MAX_PATH];
	if (GetModuleFileName(NULL,fullPathfileName, MAX_PATH))
		return cpcc_string(fullPathfileName);
	
#elif TARGET_OS_IPHONE
    NSString *path = [NSBundle mainBundle].bundlePath;
    return [path UTF8String];
    
#elif TARGET_OS_MAC
	// see also:
	// proc_pidpath()
	// GetCurrentProcess
	
	/*
	 http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
	 _NSGetExecutablePath
	 http://stackoverflow.com/questions/799679/programatically-retrieving-the-absolute-path-of-an-os-x-command-line-app/1024933#1024933
	 http://astojanov.wordpress.com/2011/11/16/mac-os-x-resolve-absolute-path-using-process-pid/
	 */
	char fullPathfileName[4096];
	ProcessSerialNumber psn;
    // needs the framework ApplicationServices.framework
    // it is deprecated
	GetCurrentProcess(&psn);
	
	pid_t pid;
	GetProcessPID (&psn, &pid);
	
	if (proc_pidpath(pid, fullPathfileName, sizeof(fullPathfileName)) > 0)
		return std::string(fullPathfileName);
    
#else
	#error Error 5453: unsupported platform for getModuleFilename()
#endif	
	return cpcc_string( _T(""));
}



cpcc_string cpccFileSystemMini::getFolder_Desktop(void) 
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szPath))) 
		return cpcc_string(szPath);
	std::cerr << "Error #5414 in cpccFileSystemMini::getFolder_Desktop\n";
		
#elif defined(__APPLE__)
	return  fileSystemOSX_helper::expandTilde_OSX(_T("~/Desktop"));
	
#else
	assert(false && "Error #5493: unsupported platform for getFolder_Desktop()");	
#endif	
	return cpcc_string(_T(""));
}


time_t		cpccFileSystemMini::getFileModificationDate(const cpcc_char * aFilename) 
{	// http://linux.die.net/man/2/stat

	if (!fileExists(aFilename) && !folderExists(aFilename))
		return 0;

	// todo: _stat does not work on XP
	cpcc_struct_stat attrib;         // create a file attribute structure
	cpcc_stat(aFilename, &attrib);     // get the attributes of afile.txt
	return attrib.st_mtime; 
}




/////////////////////////////////////////////
// class cpccPathString
/////////////////////////////////////////////

void		cpccPathString::appendPathSegment(const cpcc_char* aPathSegment)
{
	assign(cpccPathHelper::pathCat(c_str(), aPathSegment));
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(cpccFileSystemMini_DoSelfTest)

void cpccFileSystemMini::selfTest(void)
{
#ifdef cpccDEBUG
    

	// "#5349a: path delimiter"
	cpcc_char pDelimiter = cpccPathHelper::getPreferredPathDelimiter();
	assert(((pDelimiter=='/') || (pDelimiter=='\\') ) );
		
	//std::cout << "cpccFileSystemMini::SelfTest point1\n";

	// temp path is empty string
	cpcc_string tmpFolder = getFolder_UsersTemp();
	assert(tmpFolder.length()>1 && "#5356a: cpccFileSystemMini::selfTest");
	
	//std::cout << "cpccFileSystemMini::SelfTest point2\n";
	
	cpccPathHelper::addTrailingPathDelimiter(tmpFolder);
	
    if(!folderExists(tmpFolder.c_str()))
        createFolder(tmpFolder);
    
    assert(folderExists(tmpFolder.c_str()) && _T("#5356p: cpccFileSystemMini::selfTest"));
    
	// fileExists or createEmptyFile
	cpcc_string tmpFile = tmpFolder + _T("selftest-cpccFileSystemMini.txt");
	// cpcc_cout << _T("\nTmpFile:") << tmpFile << _T("\n");
    
	bool created = createEmptyFile(tmpFile.c_str());
	if (!created)
		std::cerr << "createEmptyFile() failed" << std::endl;

	assert(fileExists(tmpFile.c_str()) && _T("#5356d: cpccFileSystemMini::selfTest"));

	// std::cout << "cpccFileSystemMini::SelfTest point3\n";
	
	createEmptyFile(tmpFile.c_str());
	// std::cout << "cpccFileSystemMini::SelfTest point3.1\n";

	assert(getFileSize(tmpFile.c_str())==0 && _T("#5356e: cpccFileSystemMini::selfTest"));
			
	// std::cout << "cpccFileSystemMini::SelfTest point3.2\n";

	
	#ifdef UNICODE
		// text to test UNICODE
		// const cpcc_char * fileContent= _T("kalimera sas, καλημέρα σας.");
		const cpcc_char * fileContent = _T("kalimera sas!!");
	#else
		const cpcc_char * fileContent = _T("kalimera sas!!");
	#endif

	appendTextFile(tmpFile.c_str(),fileContent);
			
	// std::cout << "cpccFileSystemMini::SelfTest point4\n";
	
	// getFileSize
	long long filesize1 = getFileSize(tmpFile.c_str()),
		 filesize2 = cpcc_strlen(fileContent);
	assert(filesize1 == filesize2 && _T("#5356h: cpccFileSystemMini::selfTest"));
			
	// fileExists or deleteFile
	deleteFile(tmpFile.c_str());
	assert(!fileExists(tmpFile.c_str()) && _T("#5356g: cpccFileSystemMini::selfTest"));
	
    // check if the appendFile also creates the file if it does not already exist
    appendTextFile(tmpFile.c_str(), "test to create a file" );
    assert(fileExists(tmpFile.c_str()) && _T("#5356r: cpccFileSystemMini::selfTest"));
    deleteFile(tmpFile.c_str());
    
    
	// std::cout << "cpccFileSystemMini::SelfTest point5\n";
	
#ifdef _WIN32
	assert(folderExists(_T("c:\\")) && _T("#5356h: cpccFileSystemMini::selfTest"));
	assert(!folderExists(_T("c:\\non-existing-folder")) && _T("#5356k: cpccFileSystemMini::selfTest"));
#endif
#ifdef __APPLE__
	assert(folderExists("/Library") && "#5356h: cpccFileSystemMini::selfTest");
	assert(!folderExists("/non-existing-folder") && "#5356k: cpccFileSystemMini::selfTest");

#endif
	
	
#endif
}

#endif


/////////////////////////////////////////////
// Selftest cpccFileSystemMini
/////////////////////////////////////////////

#if defined(cpccFileSystemMini_DoSelfTest)


    SELFTEST_BEGIN(cpccFileSystemMini_SelfTest)
        cpccFileSystemMini::selfTest();
    SELFTEST_END


#endif // cpccFileSystemMini_DoSelfTest

#endif  // of #if defined(_WIN32) || defined (IMPORTED_BY_io_cpccFileSystemMini_mm)
