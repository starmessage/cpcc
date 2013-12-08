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

#include "cpccPathHelper.h"
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


#if defined(__APPLE__)
/*
const cpcc_string cpccPathHelper::expandTilde(const cpcc_char *aPathWithTilde)
{
	// xtypaei merikes fores
	
	//	http://www.davidverhasselt.com/2009/09/16/expanding-a-leading-tilde-in-cc/
	//  http://www.dreamincode.net/forums/topic/197372-how-to-expand-a-path-linux/
	
	if (!aPathWithTilde)
		return cpcc_string(_T(""));
	
	if (strlen(aPathWithTilde)==0)
		return cpcc_string(_T(""));
	
	if (aPathWithTilde[0]!='~')
		return cpcc_string(aPathWithTilde);
		
	std::cout << "expandTilde:" << aPathWithTilde << "\n->\n"; 
	cpcc_string result;	
	
	wordexp_t exp_result;
	wordexp(aPathWithTilde, &exp_result, 0);
	result = exp_result.we_wordv[0];
	wordfree(&exp_result);
	
	std::cout << result << "\n";
	return result;
}
*/
#endif


const cpcc_string cpccPathHelper::getParentFolderOf(const cpcc_string &aFullpathFilename)
{
	//size_t pos = aFullpathFilename.find_last_of("/\\");
	const size_t pos = aFullpathFilename.find_last_of(getAllPathDelimiters());
	
	if(pos != cpcc_string::npos)
		return aFullpathFilename.substr(0,pos+1);
	else
		return aFullpathFilename;
};


const cpcc_string cpccPathHelper::extractFilename(const cpcc_string &aFullpathFilename)
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
	cpcc_string p(aPath);
	return (p.find_last_of(getAllPathDelimiters())+1 == p.length());
}


bool cpccPathHelper::startsWithPathDelimiter(const cpcc_char *aPath)
{	
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
	cpccPathHelper ph;
			
	// "#5349a: path delimiter"
	cpcc_char pDelimiter = ph.getPreferredPathDelimiter();
	assert(((pDelimiter== _T('/')) || (pDelimiter== _T('\\')) ) );
			
	// test extension replacing and getExtension
	{ 
		cpcc_string TestExt = ph.replaceExtension( _T("c:\\folderA\\SubFolderB\\filename.oldext"), _T("txt") );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare(  _T("c:\\folderA\\SubFolderB\\filename.txt" ) )==0);

		TestExt = ph.replaceExtension( _T("hello.filename.o"), _T("n") );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare( _T("hello.filename.n") )==0);

		TestExt = ph.replaceExtension( _T("hello.filename with long extension"), _T(".dat") );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		//cpccQmsg::Qmsg("getExtension", fs.getExtension(TestExt.c_str()).c_str());
		assert(TestExt.compare( _T("hello.dat") )==0);
		assert(ph.getExtension(TestExt.c_str()).compare( _T("dat") )==0);

		TestExt = ph.replaceExtension(_T("filename without extension"), _T(".ooo") );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());		
		assert(TestExt.compare( _T("filename without extension.ooo"))==0);

		TestExt = ph.replaceExtension( _T("\\\\network shares/a/mac path"), _T(".app") );
		//cpccQmsg::Qmsg("replaceExtension", TestExt.c_str());
		assert(TestExt.compare( _T("\\\\network shares/a/mac path.app") )==0);

		assert(ph.startsWithPathDelimiter( _T("/a/mac path")  ) && _T("#6928a: cpccPathHelper"));
		assert(!ph.startsWithPathDelimiter( _T("a/mac path")  ) && _T("#6928b: cpccPathHelper"));

#ifdef _WIN32
		assert(ph.endsWithPathDelimiter( _T(".\\a\\path\\") ) && _T("#6928c: cpccPathHelper"));
		assert(!ph.endsWithPathDelimiter( _T(".\\a\\path")  ) && _T("#6928d: cpccPathHelper"));
#endif
#ifdef __APPLE__
		assert(ph.endsWithPathDelimiter( _T(".\\a/path/") ) && _T("#6928c: cpccPathHelper"));
		assert(!ph.endsWithPathDelimiter( _T(".\\a/path")  ) && _T("#6928d: cpccPathHelper"));
#endif
		
		cpcc_string test = _T("c:/tmp/");
		ph.removeTrailingPathDelimiter(test);
		assert(test.compare( _T("c:/tmp") )==0);


		assert(ph.pathCat(_T("/folderroot/"), _T("/subfolder")).compare(_T("/folderroot/subfolder")) ==0);
#ifdef _WIN32
		assert(ph.pathCat(_T("\\folderroot\\"), _T("subfolder")).compare(_T("\\folderroot\\subfolder")) ==0);
#endif
#ifdef __APPLE__
		assert(ph.pathCat(_T("\\folderroot/"), _T("subfolder")).compare(_T("\\folderroot/subfolder")) ==0);
#endif
		
		cpcc_string expectedResult;
#ifdef _WIN32
		expectedResult = _T("/folderroot\\subfolder");
#endif
#ifdef __APPLE__
		expectedResult = _T("/folderroot/subfolder");
#endif
		assert(ph.pathCat(_T("/folderroot"), _T("/subfolder")).compare(expectedResult) ==0);
		assert(ph.pathCat(_T("/folderroot"), _T("subfolder")).compare(expectedResult) ==0);

	}
			
}

/////////////////////////////////////////////
// Selftest 
/////////////////////////////////////////////

#if defined(cpccPathHelper_DoSelfTest)


SELFTEST_BEGIN(cpccPathHelper_SelfTest)
	if (cpccPathHelper::config_RunSelfTest)
		cpccPathHelper::selfTest();
SELFTEST_END


#endif // cpccPathHelper_DoSelfTest
