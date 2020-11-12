/*  *****************************************
 *  File:		fs.cpccPathHelper.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include "cpccTesting.h"

#ifndef _WIN32
    #ifndef _T
        #define _T(s) s
    #endif
#endif

// ////////////////////////////////////////////////////////
// 
// 		class cpccPathHelper
// 
// ////////////////////////////////////////////////////////



/** A small and portable (cross platform) C++ class 
	with basic filepath modification functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h
*/



class cpccPathHelper
{
public:

	enum { config_RunSelfTest = true };
    #ifdef UNICODE
        typedef wchar_t char_t;
    #else
        typedef char    char_t;
    #endif
    typedef std::basic_string<char_t>    string_t;
    
public:	

	static void				selfTest(void);

	static const char_t		getPreferredPathDelimiter(void);
	static const char_t*    getAllPathDelimiters(void);

	static void				removeLeadingPathDelimiter(string_t &aPath);
	static void				removeTrailingPathDelimiter(string_t  &aPath);
	static void				addTrailingPathDelimiter(string_t  &aFolder);

	// static string_t 		getParentFolderOf(const string_t  &aFullpathFilename);
	// static string_t 		getParentFolderOf(const char_t *aFilename) { return getParentFolderOf(string_t (aFilename)); };
    static string_t 		getParentFolderOf(const char_t* aFilename);

	static string_t 		extractFilename(const string_t  &aFullpathFilename);

	/// Returns a file path with a different extension. The newExtension parameter can be specified with or without a leading '.' 
	static string_t 		replaceExtension(const char_t *aFilename, const char_t*newExtension);
	static string_t 		getExtension(const char_t *aFilename);

	/// concatenates two paths
	static string_t 		pathCat(const char_t *a, const char_t *b);

	static bool				endsWithPathDelimiter(const char_t *aPath);
	static bool				startsWithPathDelimiter(const char_t *aPath);
};



// ///////////////////////////////////////////////////////
//
// 		class cpccPathHelper
//		implementation
//
// ///////////////////////////////////////////////////////


inline void	cpccPathHelper::removeTrailingPathDelimiter(string_t & aPath)
{
    while (endsWithPathDelimiter(aPath.c_str()))
        aPath.erase(aPath.length() - 1);
}


inline void	cpccPathHelper::addTrailingPathDelimiter(string_t & aFolder)
{
    if (endsWithPathDelimiter(aFolder.c_str()))
        return;
    aFolder = aFolder + getPreferredPathDelimiter();
}


inline void	cpccPathHelper::removeLeadingPathDelimiter(string_t & aPath)
{
	//while (aPath.find_last_of("\\/") == aPath.length())
	while (aPath.find_first_of(getAllPathDelimiters()) == 0)
		aPath.erase(0, 1);
}


inline bool cpccPathHelper::startsWithPathDelimiter(const char_t *aPath)
{
    if (!aPath)
        return false;
    
    string_t    p(aPath);
    return (p.find_first_of(getAllPathDelimiters()) == 0);
}


inline bool cpccPathHelper::endsWithPathDelimiter(const char_t *aPath)
{
    if (!aPath)
        return false;
    
    string_t   p(aPath);
    if (p.length() == 0)
        return false;

    return (p.find_last_of(getAllPathDelimiters())+1 == p.length());
}


inline std::basic_string<cpccPathHelper::char_t> cpccPathHelper::extractFilename(const string_t  &aFullpathFilename)
{
    string_t  result( aFullpathFilename );
    
    // const size_t last_slash_idx = result.find_last_of("\\/");
    const size_t last_slash_idx = result.find_last_of(getAllPathDelimiters());
    if (string_t ::npos != last_slash_idx)
        result.erase(0, last_slash_idx + 1);
    
    return result;
}


inline const cpccPathHelper::char_t	cpccPathHelper::getPreferredPathDelimiter(void)
{
#ifdef _WIN32
	return _T('\\');
#elif defined(__APPLE__)
	return '/';
#endif
}



inline const cpccPathHelper::char_t* cpccPathHelper::getAllPathDelimiters(void)
{
#ifdef _WIN32
	return _T("\\/");
#elif defined(__APPLE__)
	return "/";
#endif
}


inline cpccPathHelper::string_t  cpccPathHelper::getParentFolderOf(const char_t *aFullpathFilename)
{
    string_t  result;
    if (!aFullpathFilename)
        return result;

    result = aFullpathFilename;
    removeTrailingPathDelimiter(result); // exception

    const size_t pos = result.find_last_of(getAllPathDelimiters());
    /*
    std::cout << "getParentFolderOf(" << aFullpathFilename << ") pos of shlash=";
    if (pos==string_t ::npos)
        std::cout << "npos";
        else
            std::cout << pos;
    std::cout << std::endl;
    */

    if (pos != string_t::npos)
    {
        if (pos + 1 < result.length()) // otherwise it is the last character and there is nothing more to delete
            result.erase(pos + 1);
    }

    /* about erase(a)
     a is the position of the first character to be erased.
     If this is greater than the string length, it throws out_of_range.
     Note: The first character in str is denoted by a value of 0 (not 1).
    */

    addTrailingPathDelimiter(result);
    return result;
};



inline cpccPathHelper::string_t 		cpccPathHelper::getExtension(const char_t* aFilename)
{
    string_t empty;
    if (!aFilename)
        return empty;

    string_t  fName(aFilename);
    size_t pos = fName.rfind(_T("."));

    if (pos == string_t::npos)  //No extension.
        return empty;

    // remove from start until the dot (including the dot)
    fName.erase(0, pos + 1); // A value of string::npos indicates all characters until the end of the string
    return fName;
}




inline cpccPathHelper::string_t 	cpccPathHelper::replaceExtension(const char_t* aFilename, const char_t* newExtension)
{
    if (!aFilename)
    {
        string_t empty;
        return empty;
    }

    string_t  fName(aFilename);

    if (!newExtension)
        return fName;

    size_t pos = fName.rfind(_T('.'));

    if ((pos == cpccPathHelper::string_t::npos)  //No extension.
        ||
        (pos == 0)) //. is at the front. Not an extension.
    {
        if (newExtension[0] != _T('.'))
            fName.append(_T("."));
        fName.append(newExtension);
        return fName;
    }

    // remove the current extension and keep the dot
    fName.erase(pos, std::string::npos); // A value of string::npos indicates all characters until the end of the string
    if (newExtension[0] != _T('.'))
        fName.append(_T("."));
    fName.append(newExtension);
    return fName;
}


inline cpccPathHelper::string_t 		cpccPathHelper::pathCat(const char_t* a, const char_t* b)
{
    string_t  result;
    if (a)
        result = a;

    if (!b)
        return result;
    
    string_t  part2 = b;

    addTrailingPathDelimiter(result);
    removeLeadingPathDelimiter(part2);
    return result + part2;
}


TEST_RUN(cpccPathHelper_selfTest)
{
    const bool skipThisTest = false;
    
    if (skipThisTest)
    {
        TEST_ADDNOTE("Test skipped");
        return;
    }

    const cpccPathHelper::char_t  *emptypath = _T("");

    TEST_EXPECT(!cpccPathHelper::endsWithPathDelimiter(emptypath), _T("#8163a: endsWithPathDelimiter"))
    TEST_EXPECT(!cpccPathHelper::startsWithPathDelimiter(emptypath), _T("#8163b: startsWithPathDelimiter"))

    cpccPathHelper::char_t pDelimiter = cpccPathHelper::getPreferredPathDelimiter();
    TEST_EXPECT(((pDelimiter== _T('/')) || (pDelimiter== _T('\\')) ), _T("#9376a: pDelimiter"))
    
    const cpccPathHelper::char_t* aPath= _T("/folder1/subfolder/");
    TEST_EXPECT(cpccPathHelper::endsWithPathDelimiter(aPath), _T("#9376b: endsWithPathDelimiter"))
    TEST_EXPECT(cpccPathHelper::startsWithPathDelimiter(aPath), _T("#9376c: startsWithPathDelimiter"))
    
    cpccPathHelper::string_t  TestExt = cpccPathHelper::replaceExtension( _T("c:\\folderA\\SubFolderB\\filename.oldext"), _T("txt") );
    TEST_EXPECT(TestExt.compare(  _T("c:\\folderA\\SubFolderB\\filename.txt" ) )==0, _T("#9376d: replaceExtension"));

    TestExt = cpccPathHelper::replaceExtension( _T("hello.filename.o"), _T("n") );
    TEST_EXPECT(TestExt.compare( _T("hello.filename.n") )==0, _T("#9376e: replaceExtension"));

    TestExt = cpccPathHelper::replaceExtension( _T("hello.filename with long extension"), _T(".dat") );
    TEST_EXPECT(TestExt.compare( _T("hello.dat") )==0,  _T("#9376f: replaceExtension"));
    TEST_EXPECT(cpccPathHelper::getExtension(TestExt.c_str()).compare( _T("dat") )==0,  _T("#9376g: getExtension"));

    TestExt = cpccPathHelper::replaceExtension(_T("filename without extension"), _T(".ooo") );
    TEST_EXPECT(TestExt.compare( _T("filename without extension.ooo"))==0, _T("#9376h: replaceExtension"));

    TestExt = cpccPathHelper::replaceExtension( _T("\\\\network shares/a/mac path"), _T(".app") );
    TEST_EXPECT(TestExt.compare( _T("\\\\network shares/a/mac path.app") )==0, _T("#9376j: replaceExtension"));

    TEST_EXPECT(cpccPathHelper::startsWithPathDelimiter( _T("/a/mac path")  ) , _T("#9376k: startsWithPathDelimiter"));
    TEST_EXPECT(!cpccPathHelper::startsWithPathDelimiter( _T("a/mac path")  ) , _T("#9376m: startsWithPathDelimiter"));

    #ifdef _WIN32
        TEST_EXPECT(cpccPathHelper::endsWithPathDelimiter( _T(".\\a\\path\\") ) , _T("#9376p: endsWithPathDelimiter"));
        TEST_EXPECT(!cpccPathHelper::endsWithPathDelimiter( _T(".\\a\\path")  ) , _T("#6928q: endsWithPathDelimiter"));
    #endif
    #ifdef __APPLE__
        TEST_EXPECT(cpccPathHelper::endsWithPathDelimiter( _T(".\\a/path/") ) , _T("#6928r: endsWithPathDelimiter"));
        TEST_EXPECT(!cpccPathHelper::endsWithPathDelimiter( _T(".\\a/path") ) , _T("#6928s: endsWithPathDelimiter"));
    #endif
    
    cpccPathHelper::string_t  test = _T("c:/tmp/");
    cpccPathHelper::removeTrailingPathDelimiter(test);
    TEST_EXPECT(test.compare( _T("c:/tmp") )==0, _T("#6928u: removeTrailingPathDelimiter"));

    cpccPathHelper::string_t  rootFolder;
    rootFolder += cpccPathHelper::getPreferredPathDelimiter();
    cpccPathHelper::string_t  parentFolder(cpccPathHelper::getParentFolderOf(rootFolder.c_str()));
    TEST_EXPECT((parentFolder.compare(rootFolder)==0) , _T("#7267: parentFolderOf('/')"));

    
    TEST_EXPECT(cpccPathHelper::pathCat(_T("/folderroot/"), _T("/subfolder")).compare(_T("/folderroot/subfolder")) ==0 , _T("#7267a: pathCat"));
    #ifdef _WIN32
        TEST_EXPECT(cpccPathHelper::pathCat(_T("\\folderroot\\"), _T("subfolder")).compare(_T("\\folderroot\\subfolder")) ==0,  _T("#6928w: endsWithPathDelimiter"));
    #endif
    #ifdef __APPLE__
        TEST_EXPECT(cpccPathHelper::pathCat(_T("\\folderroot/"), _T("subfolder")).compare(_T("\\folderroot/subfolder")) ==0, _T("#4972e: pathCat()"));
    #endif
    
    cpccPathHelper::string_t  expectedResult;
    #ifdef _WIN32
        expectedResult = _T("/folderroot\\subfolder");
    #endif
    #ifdef __APPLE__
        expectedResult = _T("/folderroot/subfolder");
    #endif
    TEST_EXPECT(cpccPathHelper::pathCat(_T("/folderroot"), _T("/subfolder")).compare(expectedResult) ==0, _T("#4972a: pathCat()"));
    TEST_EXPECT(cpccPathHelper::pathCat(_T("/folderroot"), _T("subfolder")).compare(expectedResult) ==0, _T("#4972b: pathCat()"));
    
    cpccPathHelper::string_t  aLongPath ( _T("/folder/Subfolder/file.txt"));
    cpccPathHelper::string_t  aParentPath = cpccPathHelper::getParentFolderOf(aLongPath.c_str());
    TEST_EXPECT((aParentPath== _T("/folder/Subfolder/")) , _T("#4972x: getParentFolderOf()"));
    cpccPathHelper::string_t  aParentPath2 = cpccPathHelper::getParentFolderOf(aParentPath.c_str());
    TEST_EXPECT((aParentPath2== _T("/folder/")) , _T("#4972j: getParentFolderOf()"));

}
