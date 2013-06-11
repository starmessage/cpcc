/*  *****************************************
 *  File:		cpccPathHelper.cpp
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

#define 	cpccPathHelper_DoSelfTest	true

#include <assert.h>
/*
#include <cstdio>
#include <iostream>
#include <errno.h>
*/

/*
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
*/

#include "cpccPathHelper.h"
#include "cpcc_SelfTest.h"




const char	cpccPathHelper::getPreferredPathDelimiter(void) 
{ 
#ifdef _WIN32
	return '\\'; 
#elif defined(__APPLE__)
	return '/'; 
#endif
};

const char	*cpccPathHelper::getAllPathDelimiters(void) 
{ 
#ifdef _WIN32
	return "\\/"; 
#elif defined(__APPLE__)
	return "/"; 
#endif
};





const std::string cpccPathHelper::getParentFolderOf(const std::string &aFullpathFilename)
{
	//size_t pos = aFullpathFilename.find_last_of("/\\");
	const size_t pos = aFullpathFilename.find_last_of(getAllPathDelimiters());
	
	if(pos != std::string::npos)
		return aFullpathFilename.substr(0,pos+1);
	else
		return aFullpathFilename;
};


const std::string cpccPathHelper::extractFilename(const std::string &aFullpathFilename)
{
	std::string result = aFullpathFilename;
	
	// const size_t last_slash_idx = result.find_last_of("\\/");
	const size_t last_slash_idx = result.find_last_of(getAllPathDelimiters());
	if (std::string::npos != last_slash_idx)
		result.erase(0, last_slash_idx + 1);
	
	return result;
};



void cpccPathHelper::excludeTrailingPathDelimiter(std::string &aPath)
{	
	//while (aPath.find_last_of("\\/") == aPath.length())
	while (aPath.find_last_of(getAllPathDelimiters()) == aPath.length())
		aPath.erase(aPath.length()-1);
};


void cpccPathHelper::includeTrailingPathDelimiter(std::string &aFolder)
{
	char delimiter = getPreferredPathDelimiter();
	std::string::size_type n = aFolder.size();
	if ((n > 0) && (aFolder[n - 1] != delimiter)) 
		aFolder = aFolder + delimiter;
};





std::string	cpccPathHelper::replaceExtension(const char *aFilename, const char *newExtension)
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


std::string		cpccPathHelper::getExtension(const char *aFilename)
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


void cpccPathHelper::selfTest(void)
{
	cpccPathHelper ph;
			
	// "#5349a: path delimiter"
	char pDelimiter = ph.getPreferredPathDelimiter();
	assert(((pDelimiter=='/') || (pDelimiter=='\\') ) );
			
	// test extension replacing and getExtension
	{
		std::string TestExt = ph.replaceExtension("c:\\folderA\\SubFolderB\\filename.oldext", "txt" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare("c:\\folderA\\SubFolderB\\filename.txt")==0);

		TestExt = ph.replaceExtension("hello.filename.o", "n" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare("hello.filename.n")==0);

		TestExt = ph.replaceExtension("hello.filename with long extension", ".dat" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		//cpccQmsg::Qmsg("getExtension", fs.getExtension(TestExt.c_str()).c_str());
		assert(TestExt.compare("hello.dat")==0);
		assert(ph.getExtension(TestExt.c_str()).compare("dat")==0);

		TestExt = ph.replaceExtension("filename without extension", ".ooo" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());		
		assert(TestExt.compare("filename without extension.ooo")==0);

		TestExt = ph.replaceExtension("\\\\network shares/a/mac path", ".app" );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare("\\\\network shares/a/mac path.app")==0);
	}
			
};

/////////////////////////////////////////////
// Selftest 
/////////////////////////////////////////////

#if defined(cpccPathHelper_DoSelfTest)


SELFTEST_BEGIN(cpccPathHelper_SelfTest)
	cpccPathHelper::selfTest();
SELFTEST_END


#endif // cpccPathHelper_DoSelfTest
