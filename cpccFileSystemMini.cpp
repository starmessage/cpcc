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

#define 	cpccFileSystemMini_DoSelfTest	true


#include <assert.h>
#include <cstdio>
#include <iostream>
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
	#include <unistd.h> // for access on unix, mac	
	#include <CoreFoundation/CoreFoundation.h>
	#include <Carbon/Carbon.h> // for the DialogRef
	#include <libproc.h>
	
#endif

#include "cpccFileSystemMini.h"
#include "cpcc_SelfTest.h"

/////////////////////////////////////////////
// class cpccFileSystemMini_SelfTest
/////////////////////////////////////////////



const char	cpccFileSystemMini::getPreferredPathDelimiter(void) 
{ 
#ifdef _WIN32
	return '\\'; 
#elif defined(__APPLE__)
	return '/'; 
#endif
};

const char	*cpccFileSystemMini::getAllPathDelimiters(void) 
{ 
#ifdef _WIN32
	return "\\/"; 
#elif defined(__APPLE__)
	return "/"; 
#endif
};

const std::string cpccFileSystemMini::getFileSystemReport(void)
{
	std::string report("File System Report by cpccFileSystemMini\n----------------------\n");
	report.append("Temp folder:"	+ getFolder_Temp() + "\n");
	report.append("Desktop folder:" + getFolder_Desktop() + "\n");
	report.append("Fonts folder:"   + getFolder_Fonts() + "\n");
	report.append("App filename:"	+ getAppFilename() + "\n");
	report.append("App path:"		+ getAppFullPath() + "\n");

	report.append("End of file system report\n----------------------\n");
	return report;
};


const std::string cpccFileSystemMini::getFolder_Temp(void)
{
	// getenv("TEMP"); // does not work on mac
	// getenv("TMPDIR): returns: /var/folders/zv/zvUjUH8BFX0Sb5mxkslqWU+++TI/-Tmp-/
    // TMPDIR is what Posix recommends, I think.
	
	#ifdef _WIN32
		char buffer[MAX_PATH];
		GetTempPath(sizeof(buffer), buffer);
		assert(buffer && "#6753a: GetTempPath() failed");
		return  std::string(buffer);
	
	#elif defined(__APPLE__)
		char buffer[L_tmpnam +1];
		tmpnam(buffer); // in MS VC this does not contain a folder. It is just a filename
		assert(buffer && "#6753b: tmpnam() failed");
		return getParentFolderOf(std::string(buffer));
	
	#else
		assert(false && "Error #6753c: unsupported platform for getFolder_Temp()");	
		
	#endif
	return  std::string("");
};


const std::string cpccFileSystemMini::getFolder_Fonts(void)
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, szPath))) 
		{
		std::string result(szPath);
		IncludeTrailingPathDelimiter(result);	
		return result;
		}
	std::cerr << "Error #4824 in cpccFileSystemMini::getFolder_Fonts\n";
	
#elif defined(__APPLE__)
	return  std::string("/library/fonts/");
	
#else
	assert(false && "Error #5735: unsupported platform for getFolder_Fonts()");	
#endif	
	return std::string("");
}



const std::string cpccFileSystemMini::getFolder_Desktop(void)
{
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb762181%28v=vs.85%29.aspx
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szPath))) 
		return std::string(szPath);
	std::cerr << "Error #5414 in cpccFileSystemMini::getFolder_Desktop\n";
		
#elif defined(__APPLE__)
	return  std::string("~/Desktop");
	
#else
	assert(false && "Error #5493: unsupported platform for getFolder_Desktop()");	
#endif	
	return std::string("");
};


const std::string cpccFileSystemMini::getParentFolderOf(const std::string &aFullpathFilename)
{
	//size_t pos = aFullpathFilename.find_last_of("/\\");
	const size_t pos = aFullpathFilename.find_last_of(getAllPathDelimiters());
	
	if(pos != std::string::npos)
		return aFullpathFilename.substr(0,pos+1);
	else
		return aFullpathFilename;
};


const std::string cpccFileSystemMini::extractFilename(const std::string &aFullpathFilename)
{
	std::string result = aFullpathFilename;
	
	// const size_t last_slash_idx = result.find_last_of("\\/");
	const size_t last_slash_idx = result.find_last_of(getAllPathDelimiters());
	if (std::string::npos != last_slash_idx)
		result.erase(0, last_slash_idx + 1);
	
	return result;
};


cpccFileSize_t cpccFileSystemMini::getFileSize(const char *aFilename)
{	
	// http://www.codeproject.com/Articles/9016/Quick-and-Dirty-Series-C-FileSize-function
	
	// GetFileSizeEx()
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa364957%28v=vs.85%29.aspx
	
	struct stat stat_buf;
	int rc = stat(aFilename, &stat_buf);
	return (rc == 0) ? stat_buf.st_size : -1;
	
}; 


bool cpccFileSystemMini::copyFile(const char * sourceFile, const char * destFileOrFolder) 
{	std::string destFile=destFileOrFolder;
	
	if (folderExists(destFileOrFolder)) 
		{
		IncludeTrailingPathDelimiter(destFile);
		destFile = destFile + extractFilename(sourceFile);
		}
	
	return copyFileToaFile(sourceFile, destFile.c_str());
};


bool cpccFileSystemMini::deleteFile(const char * filename) 
{	
	/*
	 http://www.cplusplus.com/reference/clibrary/cstdio/remove/
	 If the file is successfully deleted, a zero value is returned.
	 On failure, a nonzero value is reurned and the errno variable is 
	 set to the corresponding error code. A string interpreting 
	 this value can be printed to the standard error stream by a call to perror.
	 */ 
	if (!fileExists(filename))
		{
		#ifdef _WIN32
			#pragma warning(disable : 4996)
		#endif
		std::cerr << "Error in cpccFileSystemMini::deleteFile. File does not exist: " << filename << std::endl;
		return true;
		};
	
	return (remove(filename)==0);
};


bool cpccFileSystemMini::fileAppend(const char* aFilename, const char *txt)
{
	FILE *fp; 
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	fp= fopen(aFilename, "at"); // write append
	if (!fp) return false;
	fprintf(fp,"%s",txt);
	fclose(fp);
	return true;
};


bool cpccFileSystemMini::fileExists(const char * aFilename)
{
#ifdef _WIN32
	struct _stat fileinfo;
	
	// On success, zero is returned. 
	// On error, -1 is returned, and errno is set appropriately. 
	if (_stat(aFilename, &fileinfo)==-1)
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
	
};



bool cpccFileSystemMini::folderExists(const char * aFilename)
{
#ifdef _WIN32
	DWORD attrib = GetFileAttributes(aFilename);
	return (! ( attrib == 0xFFFFFFFF || !(attrib & FILE_ATTRIBUTE_DIRECTORY) ) );
	// Other way:
	// return (PathIsDirectory( aFilename ) == FILE_ATTRIBUTE_DIRECTORY);
	
#elif defined(__APPLE__)
	struct stat fileinfo;
	
	if (stat(aFilename, &fileinfo) == -1)
	{	// On success, zero is returned. 
		// On error, -1 is returned, and errno is set appropriately. 
		return false;
	}
	else
		return (S_ISDIR(fileinfo.st_mode));
	
#else
	#error 	Error #5414: unsupported platform for folderExists()
#endif
	return false;
};



bool cpccFileSystemMini::copyFileToaFile(const char* sourceFile, const char* destFile)
{
	if (!fileExists(sourceFile)) 
		return false;

	// You may use std::fopen, std::fread, std::fwrite, and std::fclose, 
	// all of which are part of the standard C++ library (#include <cstdio>, very portable)
	char buf[BUFSIZ];
	size_t size;
	
	#ifdef _WIN32
		#pragma warning( disable : 4996 )
	#endif
	FILE* source = fopen(sourceFile, "rb");
	if (!source) 
		return false;
	
	FILE* dest = fopen(destFile, "wb");
	if (!dest) 
		return false;
	
	bool errorOccured=false;
	while ((!errorOccured) && (size = fread(buf, 1, BUFSIZ, source))) 
		errorOccured = ( fwrite(buf, 1, size, dest) != size );
	
	fclose(source);
	fclose(dest);
	
	return (!errorOccured);
};
	

void cpccFileSystemMini::ExcludeTrailingPathDelimiter(std::string &aPath)
{	
	//while (aPath.find_last_of("\\/") == aPath.length())
	while (aPath.find_last_of(getAllPathDelimiters()) == aPath.length())
		aPath.erase(aPath.length()-1);
};


void cpccFileSystemMini::IncludeTrailingPathDelimiter(std::string &aFolder)
{
	char delimiter = getPreferredPathDelimiter();
	std::string::size_type n = aFolder.size();
	if ((n > 0) && (aFolder[n - 1] != delimiter)) 
		aFolder = aFolder + delimiter;
};



bool cpccFileSystemMini::createEmptyFile(const char * aFilename)
{
	FILE *fp;
	#ifdef _WIN32
		#pragma warning(disable : 4996)
	#endif
	fp=fopen(aFilename,"w");
	if (fp==NULL) return false;
	fclose(fp);
	return true;	
	// other way: std::ofstream tmpOut(aFilename); 
};


const std::string cpccFileSystemMini::getAppFilename(void)
{
	// remove the path part and leave only the filename
	return extractFilename(getAppFullPathFilename());
};


const std::string cpccFileSystemMini::getAppFullPath(void)
{
	return getParentFolderOf(getAppFullPathFilename());
};


const std::string cpccFileSystemMini::getAppFullPathFilename(void)
{

#ifdef _WIN32
	_TCHAR fullPathfileName[MAX_PATH];
	if (GetModuleFileName(NULL,fullPathfileName, MAX_PATH))
		return std::string(fullPathfileName);
	
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
	return std::string("");
	
};



std::string	cpccFileSystemMini::replaceExtension(const char *aFilename, const char *newExtension)
{
	if (!aFilename)
		return std::string("");

	std::string fName(aFilename);

	if (!newExtension) 
		return fName;

    size_t pos = fName.rfind('.');

    if ((pos == std::string::npos )  //No extension.
		||
		(pos == 0)) //. is at the front. Not an extension.
        {
		if (newExtension[0]!='.')
			fName.append(".");
		fName.append(newExtension);
		return fName;
		}
	    
	// remove the current extension and keep the dot
    fName.erase(pos, std::string::npos ); // A value of string::npos indicates all characters until the end of the string
	if (newExtension[0]!='.')
		fName.append(".");
	fName.append(newExtension);
	return fName;
};


std::string		cpccFileSystemMini::getExtension(const char *aFilename)
{
	if (!aFilename)
		return std::string("");

	std::string fName(aFilename);

    size_t pos = fName.rfind(".");

    if (pos == std::string::npos )  //No extension.
		return std::string("");
	    
	// remove from start until the dot (including the dot)
    fName.erase(0, pos+1); // A value of string::npos indicates all characters until the end of the string
	return fName;
};


void cpccFileSystemMini::selfTest(void)
{
	cpccFileSystemMini fs;
			
	// "#5349a: path delimiter"
	char pDelimiter = fs.getPreferredPathDelimiter();
	assert(((pDelimiter=='/') || (pDelimiter=='\\') ) );
			

	// temp path is empty string
	std::string tmpFolder = fs.getFolder_Temp();
	assert(tmpFolder.length()>1);
			
	fs.IncludeTrailingPathDelimiter(tmpFolder);
				
	// fileExists or createEmptyFile
	std::string tmpFile = tmpFolder + "selftest-cpccFileSystemMini.txt";
	//std::cout << "\nTmpFile:" << tmpFile << "\n";
	fs.createEmptyFile(tmpFile);
	assert(fs.fileExists(tmpFile));
			
			
	const char * fileContent="kalimera sas";
	fs.fileAppend(tmpFile.c_str(),fileContent);
			
	// getFileSize
	assert(fs.getFileSize(tmpFile)==strlen(fileContent));
			
	// fileExists or deleteFile
	fs.deleteFile(tmpFile);
	assert(!fs.fileExists(tmpFile));

	// test extension replacing and getExtension
	{
		std::string TestExt = fs.replaceExtension("c:\\folderA\\SubFolderB\\filename.oldext", "txt" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare("c:\\folderA\\SubFolderB\\filename.txt")==0);

		TestExt = fs.replaceExtension("hello.filename.o", "n" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare("hello.filename.n")==0);

		TestExt = fs.replaceExtension("hello.filename with long extension", ".dat" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		//cpccQmsg::Qmsg("getExtension", fs.getExtension(TestExt.c_str()).c_str());
		assert(TestExt.compare("hello.dat")==0);
		assert(fs.getExtension(TestExt.c_str()).compare("dat")==0);

		TestExt = fs.replaceExtension("filename without extension", ".ooo" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());		
		assert(TestExt.compare("filename without extension.ooo")==0);

		TestExt = fs.replaceExtension("\\\\network shares/a/mac path", ".app" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare("\\\\network shares/a/mac path.app")==0);
	}

			
};

/////////////////////////////////////////////
// Selftest cpccFileSystemMini
/////////////////////////////////////////////

#if defined(cpccFileSystemMini_DoSelfTest)


SELFTEST_BEGIN(cpccFileSystemMini_SelfTest)
	// MessageBox(NULL, "selftest of cpccFileSystemMini"	, NULL, NULL);
	cpccFileSystemMini::selfTest();
SELFTEST_END


#endif // cmi_run_FSM_SelfTest
