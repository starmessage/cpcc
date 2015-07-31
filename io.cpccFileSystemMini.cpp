/*  *****************************************
 *  File:		cpccFileSystemMini.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
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



#include <assert.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <errno.h>

#ifdef _WIN32
	#include	<io.h> // for _access on windows
	#include	<Shlobj.h>
	#include	<Shlwapi.h>
	#pragma comment(lib,"Shlwapi.lib") // instruct visual studio to link the library
	#include	<tchar.h>
	#include	<sys/types.h>
	#include	<sys/stat.h>

#elif defined(__APPLE__)
    #include "io.cpccFileSystemMiniOSX.h"
    #include <CoreFoundation/CoreFoundation.h>
	#include <Carbon/Carbon.h> // for the DialogRef

#endif

#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#if defined(cpccFileSystemMini_DoSelfTest)
	#include "cpcc_SelfTest.h"
#endif


#if defined(__APPLE__)




#endif

// main class

const cpcc_string cpccFileSystemMini::getFileSystemReport(void)
{
	cpcc_string report( _T("File System Report by cpccFileSystemMini\n----------------------\n"));
	report.append(_T("App filename:")	+ getAppFilename() + _T("\n"));
	report.append(_T("App path:")		+ getAppFullPath() + _T("\n"));	
	report.append(_T("System's Temp folder:")	+ getFolder_SystemsTemp() + _T("\n"));
	report.append(_T("User's Temp folder:")	+ getFolder_UsersTemp() + _T("\n"));

	report.append(_T("User's home folder:")	+ getFolder_UserHome() + _T("\n"));
	report.append(_T("Desktop folder:") + getFolder_Desktop() + _T("\n"));
	report.append(_T("Fonts folder:")   + getFolder_Fonts() + _T("\n"));
	report.append(_T("AppData path:")	+ getFolder_CommonAppData() + _T("\n"));
	report.append(_T("UserData path:")	+ getFolder_UserData() + _T("\n"));

	report.append(_T("End of file system report\n----------------------\n"));
	return report;
}
 
#ifdef _WIN32
const cpcc_string cpccFileSystemMini::getFolder_Windows(void) const
{
	cpcc_char buffer[300];
	GetWindowsDirectory(buffer, 300);
	return cpcc_string(buffer) + "\\";
}
#endif


const cpcc_string cpccFileSystemMini::getFolder_UserHome(void) const
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g. C:\ProgramData
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, szPath))) 
	{
		cpcc_string result(szPath);
		cpccPathHelper ph;
		ph.addTrailingPathDelimiter(result);	
		return result;
	}
	std::cerr << "Error #6531 in getFolder_UserHome\n";
	
#elif defined(__APPLE__)
    return getUserFolder_OSX();
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_UserHome()");	
#endif	
	return cpcc_string( _T("") );
}


const cpcc_string  cpccFileSystemMini::getFolder_CommonAppData(void)
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g. C:\ProgramData
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath))) 
		{
		cpcc_string result(szPath);
		cpccPathHelper ph;
		ph.addTrailingPathDelimiter(result);	
		return result;
		}
	std::cerr << "Error #6531 in getFolder_CommonAppData\n";
	
#elif defined(__APPLE__)
	// http://apple.stackexchange.com/questions/28928/what-is-the-osx-equivalent-to-windows-appdata-folder
	// https://developer.apple.com/library/mac/documentation/General/Conceptual/MOSXAppProgrammingGuide/AppRuntime/AppRuntime.html#//apple_ref/doc/uid/TP40010543-CH2-SW9
	//return  std::string("/Library/Application Support/");
	//return  std::string("/library/preferences/");
	
	// finally I chose /users/shared
	return  std::string("/users/shared/");
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_AppData()");	
#endif	
	return cpcc_string( _T("") );
}


const cpcc_string cpccFileSystemMini::getFolder_UserData(void) const
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	// e.g C:\Users\JohnSmith\AppData\Roaming
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) 
		{
		cpcc_string result(szPath);
		cpccPathHelper ph;
		ph.addTrailingPathDelimiter(result);	
		return result;
		}
	std::cerr << "Error #6531 in getFolder_AppData::getFolder_UserData\n";
	
#elif defined(__APPLE__)
	// http://apple.stackexchange.com/questions/28928/what-is-the-osx-equivalent-to-windows-appdata-folder
	cpccPathHelper ph;
	return ph.pathCat(getFolder_UserHome().c_str(), "/Library/Preferences/");
	//return  ph.expandTilde("~/Library/Preferences/");
#else
	assert(false && "Error #5735: unsupported platform for getFolder_AppData()");	
#endif	
	return cpcc_string( _T("") );
}


const cpcc_string cpccFileSystemMini::getFolder_UsersTemp(void)
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
		std::string userTempFolder( expandTilde_OSX("~/Library/Caches/temp-cpcc"));
		createFolder(userTempFolder);
		return userTempFolder;
	
	#else
		assert(false && "Error #6753c: unsupported platform for getFolder_UsersTemp()");	
		
	#endif
	return  cpcc_string( _T("") );
}


const cpcc_string cpccFileSystemMini::getFolder_SystemsTemp(void)
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
		tmpnam(buffer); // in MS VC this does not contain a folder. It is just a filename
						// it returns the SYSTEM's temp
        #pragma GCC diagnostic pop
		assert(buffer && "#6753b: tmpnam() failed");
		cpccPathHelper ph;
		return ph.getParentFolderOf(std::string(buffer));
	
	#else
		assert(false && "Error #6753c: unsupported platform for getFolder_SystemsTemp()");	
		
	#endif
	return  cpcc_string( _T("") );
}


const cpcc_string cpccFileSystemMini::getFolder_Fonts(void)
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, szPath))) 
		{
		cpcc_string result(szPath);
		cpccPathHelper ph;
		ph.addTrailingPathDelimiter(result);	
		return result;
		}
	std::cerr << "Error #4824 in cpccFileSystemMini::getFolder_Fonts\n";
	
#elif defined(__APPLE__)
	return  std::string("/library/fonts/");
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_Fonts()");	
#endif	
	return cpcc_string( _T("") );
}



bool cpccFileSystemMini::copyFile(const cpcc_char * sourceFile, const cpcc_char * destFileOrFolder) 
{
#ifdef __APPLE__
    cpcc_string destFile=expandTilde_OSX(destFileOrFolder);
#else
    cpcc_string destFile=destFileOrFolder;
#endif
    
	if (folderExists(destFileOrFolder))
		{
		cpccPathHelper ph;
		ph.addTrailingPathDelimiter(destFile);
		destFile = destFile + ph.extractFilename(sourceFile);
		}

	return copyFileToaFile(sourceFile, destFile.c_str());
}



bool cpccFileSystemMini::appendTextFile(const cpcc_char* aFilename, const cpcc_char *txt)
{
    const cpcc_char *finalFilename = aFilename;
#ifdef __APPLE__
    if (startsWithTilde_OSX(aFilename))
        finalFilename = expandTilde_OSX(aFilename).c_str();
#endif
    
	FILE *fp; 
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	fp= cpcc_fopen(finalFilename, _T("at")); // write append
	if (!fp) return false;
	cpcc_fprintf(fp,_T("%s"),txt);
	fclose(fp);
	return true;
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

	cpcc_string finalPath(expandTilde_OSX( aFoldername));
	
	cpcc_string parentFolder = ph.getParentFolderOf(finalPath);
	
	int parentPermissions = getFileOrFolderPermissions_OSX(parentFolder.c_str());
	
	//std::cout << "permissions of " << parentFolder << " : " << parentPermissions << "\n";
	
	// http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
	// https://discussions.apple.com/thread/844719?start=0&tstart=0
	// http://www.linuxquestions.org/questions/programming-9/mkdir-in-c-setting-wrong-permissions-613250/

	// read/write/search permissions for owner and group, and with read/search permissions for others
	//mode_t p775 = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	// full permissions for all
	//mode_t p777 = S_IRWXU | S_IRWXG | S_IRWXO;
	return createFolder_Linux(finalPath.c_str(), parentPermissions);
#endif

    return(folderExists(aFoldername));
}


bool cpccFileSystemMini::folderExists(const cpcc_char * aFoldername) const
{
#ifdef _WIN32
	DWORD attrib = GetFileAttributes(aFoldername);
	return (! ( attrib == 0xFFFFFFFF || !(attrib & FILE_ATTRIBUTE_DIRECTORY) ) );
	// Other way:
	// return (PathIsDirectory( aFilename ) == FILE_ATTRIBUTE_DIRECTORY);
	
#elif defined(__APPLE__)
    return folderExists_OSX(aFoldername);
    
#else
	#error 	Error #5414: unsupported platform for folderExists()
#endif
	return false;
}


bool cpccFileSystemMini::fileExists(const cpcc_char * aFilename) const
{
#ifdef _WIN32
	struct _stat fileinfo;
	
	// On success, zero is returned. 
	// On error, -1 is returned, and errno is set appropriately. 
	if (_tstat(aFilename, &fileinfo)==-1)
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
	#error 	Error #5413: unsupported platform for fileExists()
	
#endif
	return false;
}


bool renameFile(const cpcc_char* filenameOld, const cpcc_char* filenameNew)
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
cpccFileSize_t	cpccFileSystemMini::writeToFile(const cpcc_char *aFilename, const char *buffer, const cpccFileSize_t bufSize, const bool appendToFile)
{
    if (!buffer)
        return -3;
    
    const cpcc_char *finalFilename = aFilename;
#ifdef __APPLE__
    if (startsWithTilde_OSX(aFilename))
        finalFilename = expandTilde_OSX(aFilename).c_str();
#endif
    
    #pragma warning(disable : 4996)
	FILE * pFile = cpcc_fopen (aFilename, (appendToFile)? _T("ab") : _T("wb") );
	if (pFile==NULL) 
		return -1;
	
	cpccFileSize_t res=fwrite (buffer, 1, bufSize, pFile);
	if( ferror( pFile ) )      
		res=-2;
		
    fclose (pFile);
    return res;
}
	
	
cpccFileSize_t	cpccFileSystemMini::readFromFile(const cpcc_char *aFilename, char *buffer, const cpccFileSize_t bufSize)
{
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	if (!buffer)
		return -3;

	FILE * pFile;
	pFile = cpcc_fopen(aFilename, _T("rb"));
	if (pFile==NULL) 
		return -1;
	
	cpccFileSize_t res=fread(buffer, 1, bufSize, pFile);
	if( ferror( pFile ) )      
		res=-2;
		
    fclose (pFile);
    return res;
}


cpccFileSize_t cpccFileSystemMini::getFileSize(const cpcc_char *aFilename)
{	
	std::ifstream f(aFilename, std::ios::binary | std::ios::ate);
	return static_cast<cpccFileSize_t>(f.tellg());
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
	FILE* source = cpcc_fopen(sourceFile, _T("rb"));
	if (!source) 
		return false;
	
	FILE* dest = cpcc_fopen(destFile, _T("wb"));
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
	return (writeToFile(aFilename, "", 0, false)==0);
	// other way: std::ofstream tmpOut(aFilename); 
}


const cpcc_string cpccFileSystemMini::getAppFilename(void)
{
	// remove the path part and leave only the filename
	cpccPathHelper ph;
	return ph.extractFilename(getAppFullPathFilename());
}


const cpcc_string cpccFileSystemMini::getAppFullPath(void)
{	cpccPathHelper ph;
	return ph.getParentFolderOf(getAppFullPathFilename());
}


const cpcc_string cpccFileSystemMini::getAppFullPathFilename(void)
{

#ifdef _WIN32
	_TCHAR fullPathfileName[MAX_PATH];
	if (GetModuleFileName(NULL,fullPathfileName, MAX_PATH))
		return cpcc_string(fullPathfileName);
	
#elif defined(__APPLE__)
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
	GetCurrentProcess(&psn);
	
	pid_t pid;
	GetProcessPID (&psn, &pid);
	
	if (proc_pidpath(pid, fullPathfileName, sizeof(fullPathfileName)) > 0)
		return std::string(fullPathfileName);
#else
	#error Error #5453: unsupported platform for getModuleFilename()	
#endif	
	return cpcc_string( _T(""));
}


const cpcc_string cpccFileSystemMini::getFolder_Desktop(void)
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szPath))) 
		return cpcc_string(szPath);
	std::cerr << "Error #5414 in cpccFileSystemMini::getFolder_Desktop\n";
		
#elif defined(__APPLE__)
	return  expandTilde_OSX(_T("~/Desktop"));
	
#else
	assert(false && "Error #5493: unsupported platform for getFolder_Desktop()");	
#endif	
	return cpcc_string(_T(""));
}


time_t		cpccFileSystemMini::getFileModificationDate(const cpcc_char * aFilename) const
{	// http://linux.die.net/man/2/stat

	if (!fileExists(aFilename) && !folderExists(aFilename))
		return 0;

	struct stat attrib;         // create a file attribute structure
	stat(aFilename, &attrib);     // get the attributes of afile.txt
	return attrib.st_mtime; 
}





#if defined(cpccFileSystemMini_DoSelfTest)

void cpccFileSystemMini::selfTest(void)
{
std::cout << "cpccFileSystemMini::SelfTest starting\n";

	
	cpccFileSystemMiniEx	fs;
	cpccPathHelper			ph;		

	// "#5349a: path delimiter"
	cpcc_char pDelimiter = ph.getPreferredPathDelimiter();
	assert(((pDelimiter=='/') || (pDelimiter=='\\') ) );
		
	//std::cout << "cpccFileSystemMini::SelfTest point1\n";

	// temp path is empty string
	cpcc_string tmpFolder = fs.getFolder_UsersTemp();
	assert(tmpFolder.length()>1 && "#5356a: cpccFileSystemMini::selfTest");
	
	//std::cout << "cpccFileSystemMini::SelfTest point2\n";
	
	ph.addTrailingPathDelimiter(tmpFolder);
				
	// fileExists or createEmptyFile
	cpcc_string tmpFile = tmpFolder + _T("selftest-cpccFileSystemMini.txt");
	std::cout << "\nTmpFile:" << tmpFile << "\n";
    
	fs.createEmptyFile(tmpFile);
	assert(fs.fileExists(tmpFile) && "#5356d: cpccFileSystemMini::selfTest");

	//std::cout << "cpccFileSystemMini::SelfTest point3\n";
	
	fs.createEmptyFile(tmpFile);
	assert(fs.getFileSize(tmpFile)==0 && "#5356e: cpccFileSystemMini::selfTest");
			
			
	const cpcc_char * fileContent= _T("kalimera sas");
	fs.appendTextFile(tmpFile.c_str(),fileContent);
			
	//std::cout << "cpccFileSystemMini::SelfTest point4\n";
	
	// getFileSize
	assert(fs.getFileSize(tmpFile)==cpcc_strlen(fileContent) && "#5356h: cpccFileSystemMini::selfTest");
			
	// fileExists or deleteFile
	fs.deleteFile(tmpFile);
	assert(!fs.fileExists(tmpFile) && "#5356g: cpccFileSystemMini::selfTest");
	
	//std::cout << "cpccFileSystemMini::SelfTest point5\n";
	
#ifdef _WIN32
	assert(fs.folderExists("c:\\") && "#5356h: cpccFileSystemMini::selfTest");
	assert(!fs.folderExists("c:\\non-existing-folder") && "#5356k: cpccFileSystemMini::selfTest");
#endif
#ifdef __APPLE__
	assert(fs.folderExists("/Library") && "#5356h: cpccFileSystemMini::selfTest");
	assert(!fs.folderExists("/non-existing-folder") && "#5356k: cpccFileSystemMini::selfTest");

#endif
	
	std::cout << "cpccFileSystemMini::SelfTest ended\n";
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
