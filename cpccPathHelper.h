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
	static const cpcc_char * getClassVersion(void) { return _T("1.01"); };
	static void  selfTest(void);
	
	/// if true then a the selftest function is called automatically on app start
	enum { config_RunSelfTest=true };

protected:

public:

#if defined(__APPLE__)
	// virtual const cpcc_string expandTilde(const cpcc_char *aPathWithTilde);
#endif
	
	virtual const cpcc_char	 getPreferredPathDelimiter(void);
	virtual const cpcc_char	*getAllPathDelimiters(void);

	virtual void removeLeadingPathDelimiter(cpcc_string &aPath);
	virtual void removeTrailingPathDelimiter(cpcc_string &aPath);
	virtual void addTrailingPathDelimiter(cpcc_string &aFolder);
	
	virtual const cpcc_string getParentFolderOf(const cpcc_string &aFullpathFilename);
	virtual const cpcc_string getParentFolderOf(const cpcc_char *aFilename) { return getParentFolderOf(cpcc_string(aFilename)); };
	
	virtual const cpcc_string extractFilename(const cpcc_string &aFullpathFilename);
	
	/// Returns a file path with a different extension. The newExtension parameter can be specified with or without a leading '.' 
	cpcc_string		replaceExtension(const cpcc_char *aFilename, const cpcc_char *newExtension);
	cpcc_string		getExtension(const cpcc_char *aFilename);

	/// concatenates two paths
	cpcc_string		pathCat(const cpcc_char *a, const cpcc_char *b);
	
	bool	endsWithPathDelimiter(const cpcc_char *aPath);
	bool	startsWithPathDelimiter(const cpcc_char *aPath);
};



