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
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>


//////////////////////////////////////////////////////////
///		class cpccPathHelper
//////////////////////////////////////////////////////////



/** A small and portable (cross platform) C++ class 
	with basic filepath modification functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h
*/


/*
	future typedefs:
	
	#ifdef UNICODE //Test to see if we're using wchar_ts or not.
		typedef std::wstring PathString;
	#else
		typedef std::string PathString;
	#endif


*/


/// path manipulation helper functions
class cpccPathHelper
{
public:	// class metadata and selftest
	const char * getClassVersion(void) { return "1.00"; };
	static void selfTest(void);

protected:

public:
	
	virtual const char	 getPreferredPathDelimiter(void);
	virtual const char	*getAllPathDelimiters(void);

	virtual void excludeTrailingPathDelimiter(std::string &aPath);
	virtual void includeTrailingPathDelimiter(std::string &aFolder);
	
	virtual const std::string getParentFolderOf(const std::string &aFullpathFilename);
	virtual const std::string getParentFolderOf(const char *aFilename) { return getParentFolderOf(std::string(aFilename)); };
	
	virtual const std::string extractFilename(const std::string &aFullpathFilename);
	
	/// Returns a file path with a different extension. The newExtension parameter can be specified with or without a leading '.' 
	std::string		replaceExtension(const char *aFilename, const char *newExtension);
	std::string		getExtension(const char *aFilename);
	

};



