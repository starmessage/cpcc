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
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#define 	cpccPathHelper_DoSelfTest	true

#include <assert.h>

#include "io.cpccPathHelper.h"
#include "cpcc_SelfTest.h"

#if defined(__APPLE__)
	#include <wordexp.h>
#endif


const cpcc_char	cpccPathHelper::getPreferredPathDelimiter(void) 
{ 
#ifdef _WIN32
	return _T('\\'); 
#elif defined(__APPLE__)
	return _T('/'); 
#endif
}


const cpcc_char	*cpccPathHelper::getAllPathDelimiters(void) 
{ 
#ifdef _WIN32
	return _T("\\/"); 
#elif defined(__APPLE__)
	return _T("/"); 
#endif
}




cpcc_string cpccPathHelper::getParentFolderOf(const cpcc_string &aFullpathFilename)
{
	cpcc_string result = aFullpathFilename;
    removeTrailingPathDelimiter(result);
    
    const size_t pos = result.find_last_of(getAllPathDelimiters());
    /*
    std::cout << "getParentFolderOf(" << aFullpathFilename << ") pos of shlash=";
    if (pos==cpcc_string::npos)
        std::cout << "npos";
        else
            std::cout << pos;
    std::cout << std::endl;
    */
    
	if(pos != cpcc_string::npos)
    {
        if (pos+1 < result.length()) // otherwise it is the last character and there is nothing more to delete
            result.erase(pos+1);
    }
    
    /* about erase(a)
     a is the position of the first character to be erased.
     If this is greater than the string length, it throws out_of_range.
     Note: The first character in str is denoted by a value of 0 (not 1).
    */
    
    addTrailingPathDelimiter(result);
    return result;
    /*
	if(pos != cpcc_string::npos)
		return aFullpathFilename.substr(0,pos+1);
	else
		return aFullpathFilename;
     */
    
};


cpcc_string cpccPathHelper::extractFilename(const cpcc_string &aFullpathFilename)
{
	cpcc_string result = aFullpathFilename;
	
	// const size_t last_slash_idx = result.find_last_of("\\/");
	const size_t last_slash_idx = result.find_last_of(getAllPathDelimiters());
	if (cpcc_string::npos != last_slash_idx)
		result.erase(0, last_slash_idx + 1);
	
	return result;
}


bool cpccPathHelper::endsWithPathDelimiter(const cpcc_char *aPath)
{
    if (!aPath)
        return false;
    
    if (cpcc_strlen(aPath)==0)
        return false;
    
	cpcc_string p(aPath);
    return (p.find_last_of(getAllPathDelimiters())+1 == p.length());
}


bool cpccPathHelper::startsWithPathDelimiter(const cpcc_char *aPath)
{
    if (!aPath)
        return false;
    
    if (cpcc_strlen(aPath)==0)
        return false;
    
	cpcc_string p(aPath);
	return (p.find_first_of(getAllPathDelimiters()) == 0);
}


void cpccPathHelper::removeTrailingPathDelimiter(cpcc_string &aPath)
{	
	while ( endsWithPathDelimiter(aPath.c_str() ) )
		aPath.erase(aPath.length()-1);
	
}


void cpccPathHelper::addTrailingPathDelimiter(cpcc_string &aFolder)
{
  	if ( endsWithPathDelimiter(aFolder.c_str()))
		return;
	aFolder = aFolder + getPreferredPathDelimiter();
}


void cpccPathHelper::removeLeadingPathDelimiter(cpcc_string &aPath)
{	
	//while (aPath.find_last_of("\\/") == aPath.length())
 	while (aPath.find_first_of(getAllPathDelimiters()) == 0)
		aPath.erase(0,1);
}


cpcc_string	cpccPathHelper::replaceExtension(const cpcc_char *aFilename, const cpcc_char *newExtension)
{
	if (!aFilename)
		return cpcc_string(_T(""));

	cpcc_string fName(aFilename);

	if (!newExtension) 
		return fName;

    size_t pos = fName.rfind(_T('.'));

    if ((pos == cpcc_string::npos )  //No extension.
		||
		(pos == 0)) //. is at the front. Not an extension.
        {
		if (newExtension[0]!= _T('.'))
			fName.append( _T(".") );
		fName.append(newExtension);
		return fName;
		}
	    
	// remove the current extension and keep the dot
    fName.erase(pos, std::string::npos ); // A value of string::npos indicates all characters until the end of the string
	if (newExtension[0]!= _T('.'))
		fName.append( _T(".") );
	fName.append(newExtension);
	return fName;
}


cpcc_string		cpccPathHelper::pathCat(const cpcc_char *a, const cpcc_char *b)
{
	cpcc_string result=a;
	cpcc_string part2=b;

	addTrailingPathDelimiter(result);
	removeLeadingPathDelimiter(part2);
	return result + part2;
}


cpcc_string		cpccPathHelper::getExtension(const cpcc_char *aFilename)
{
	if (!aFilename)
		return cpcc_string( _T("") );

	cpcc_string fName(aFilename);

    size_t pos = fName.rfind( _T(".") );

    if (pos == cpcc_string::npos )  //No extension.
		return cpcc_string( _T("") );
	    
	// remove from start until the dot (including the dot)
    fName.erase(0, pos+1); // A value of string::npos indicates all characters until the end of the string
	return fName;
}



void cpccPathHelper::selfTest(void)
{
#ifdef cpccDEBUG

	if (!cpccPathHelper::config_RunSelfTest)
		return;

	// "#5349a: path delimiter"
	cpcc_char pDelimiter = cpccPathHelper::getPreferredPathDelimiter();
	assert(((pDelimiter== _T('/')) || (pDelimiter== _T('\\')) ) );
			
	// test extension replacing and getExtension
	{ 
		cpcc_string TestExt = cpccPathHelper::replaceExtension( _T("c:\\folderA\\SubFolderB\\filename.oldext"), _T("txt") );
		assert(TestExt.compare(  _T("c:\\folderA\\SubFolderB\\filename.txt" ) )==0);

		TestExt = cpccPathHelper::replaceExtension( _T("hello.filename.o"), _T("n") );
		assert(TestExt.compare( _T("hello.filename.n") )==0);

		TestExt = cpccPathHelper::replaceExtension( _T("hello.filename with long extension"), _T(".dat") );
		assert(TestExt.compare( _T("hello.dat") )==0);
		assert(cpccPathHelper::getExtension(TestExt.c_str()).compare( _T("dat") )==0);

		TestExt = cpccPathHelper::replaceExtension(_T("filename without extension"), _T(".ooo") );
		assert(TestExt.compare( _T("filename without extension.ooo"))==0);

		TestExt = cpccPathHelper::replaceExtension( _T("\\\\network shares/a/mac path"), _T(".app") );
		assert(TestExt.compare( _T("\\\\network shares/a/mac path.app") )==0);

		assert(cpccPathHelper::startsWithPathDelimiter( _T("/a/mac path")  ) && _T("#6928a: cpccPathHelper"));
		assert(!cpccPathHelper::startsWithPathDelimiter( _T("a/mac path")  ) && _T("#6928b: cpccPathHelper"));

        #ifdef _WIN32
            assert(cpccPathHelper::endsWithPathDelimiter( _T(".\\a\\path\\") ) && _T("#6928c: cpccPathHelper"));
            assert(!cpccPathHelper::endsWithPathDelimiter( _T(".\\a\\path")  ) && _T("#6928d: cpccPathHelper"));
        #endif
        #ifdef __APPLE__
            assert(cpccPathHelper::endsWithPathDelimiter( _T(".\\a/path/") ) && _T("#6928c: cpccPathHelper"));
            assert(!cpccPathHelper::endsWithPathDelimiter( _T(".\\a/path")  ) && _T("#6928d: cpccPathHelper"));
        #endif
		
		cpcc_string test = _T("c:/tmp/");
		cpccPathHelper::removeTrailingPathDelimiter(test);
		assert(test.compare( _T("c:/tmp") )==0);

        cpcc_string parentFolder(getParentFolderOf("/"));
        assert((parentFolder.compare( _T("/") )==0) && "#7267: parentFolderOf('/')");

        
		assert(cpccPathHelper::pathCat(_T("/folderroot/"), _T("/subfolder")).compare(_T("/folderroot/subfolder")) ==0);
        #ifdef _WIN32
            assert(cpccPathHelper::pathCat(_T("\\folderroot\\"), _T("subfolder")).compare(_T("\\folderroot\\subfolder")) ==0);
        #endif
        #ifdef __APPLE__
            assert(cpccPathHelper::pathCat(_T("\\folderroot/"), _T("subfolder")).compare(_T("\\folderroot/subfolder")) ==0);
        #endif
		
		cpcc_string expectedResult;
        #ifdef _WIN32
            expectedResult = _T("/folderroot\\subfolder");
        #endif
        #ifdef __APPLE__
            expectedResult = _T("/folderroot/subfolder");
        #endif
		assert(cpccPathHelper::pathCat(_T("/folderroot"), _T("/subfolder")).compare(expectedResult) ==0);
		assert(cpccPathHelper::pathCat(_T("/folderroot"), _T("subfolder")).compare(expectedResult) ==0);
        
        cpcc_string aLongPath ( _T("/folder/Subfolder/file.txt"));
        cpcc_string aParentPath = cpccPathHelper::getParentFolderOf(aLongPath);
        assert((aParentPath== _T("/folder/Subfolder/")) && _T("#4972a: getParentFolderOf()"));
        cpcc_string aParentPath2 = cpccPathHelper::getParentFolderOf(aParentPath);
        assert((aParentPath2== _T("/folder/")) && _T("#4972b: getParentFolderOf()"));

	}
#endif
    
}

/////////////////////////////////////////////
// Selftest 
/////////////////////////////////////////////

#if defined(cpccPathHelper_DoSelfTest)


SELFTEST_BEGIN(cpccPathHelper_SelfTest)
	
		cpccPathHelper::selfTest();
SELFTEST_END


#endif // cpccPathHelper_DoSelfTest
