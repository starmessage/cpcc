/*  *****************************************
 *  File:		cpccFileSystemMini.h
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
///		class cpccFileSystemMini
//////////////////////////////////////////////////////////


typedef long cpccFileSize_t;
/** A small and portable (cross platform) C++ class with basic file system functions.
	Simply add the .h and the .cpp in your application.
	Dependencies: cpcc_SelfTest.h
	
	It was created because the boost::filesystem is too complicated and needs considerable effort 
	to be included in your application.
*/


/*
	future typedefs:
	
	#ifdef UNICODE //Test to see if we're using wchar_ts or not.
		typedef std::wstring PathString;
	#else
		typedef std::string PathString;
	#endif


*/


class cpccFileSystemMini
{
public:	// class metadata
	const char * getClassVersion(void) { return "1.03"; };
	
protected:
	/// the destFile must be a file specification, not a folder specification
	virtual bool copyFileToaFile(const char* sourceFile, const char* destFile);

public:
	
	// folder functions --------------------------------
	virtual bool folderExists(const char * aFilename);
	virtual bool folderExists(const std::string& aFilename) { return folderExists(aFilename.c_str()); };	

	virtual const std::string getFolder_Desktop(void);
	virtual const std::string getFolder_Temp(void);
	virtual const std::string getFolder_Fonts(void);
	
	virtual const char	 getPreferredPathDelimiter(void);
	virtual const char	*getAllPathDelimiters(void);

	virtual void ExcludeTrailingPathDelimiter(std::string &aPath);
	virtual void IncludeTrailingPathDelimiter(std::string &aFolder);
	
	virtual const std::string getParentFolderOf(const std::string &aFullpathFilename);
	virtual const std::string getParentFolderOf(const char *aFilename) { return getParentFolderOf(std::string(aFilename)); };
	
	
	// file functions --------------------------------
	virtual bool createEmptyFile(const char * aFilename);
	virtual bool createEmptyFile(const std::string & aFilename) {	return createEmptyFile(aFilename.c_str());	};

	virtual bool fileExists(const char * aFilename);
	virtual bool fileExists(const std::string &aFilename)	{ return fileExists(aFilename.c_str()); };

	virtual bool fileAppend(const char* aFilename, const char *txt);
	virtual bool fileAppend(const std::string aFilename, const std::string text) { return fileAppend(aFilename.c_str(), text.c_str());  };
	
	virtual bool deleteFile(const char * filename);
	virtual bool deleteFile(const std::string aFilename)  {  return deleteFile(aFilename.c_str()); };
	
	virtual bool copyFile(const char * sourceFile, const char * destFileOrFolder);	
	virtual bool copyFile(const std::string &sourceFile, const std::string &destFileOrFolder) {	return copyFile(sourceFile.c_str(), destFileOrFolder.c_str()); }; 

	virtual cpccFileSize_t getFileSize(const char *aFilename); 
	virtual cpccFileSize_t getFileSize(const std::string &aFilename)  {	return getFileSize(aFilename.c_str()); 	};
		
	virtual const std::string extractFilename(const std::string &aFullpathFilename);
	
	/// Returns a file path with a different extension. The newExtension parameter can be specified with or without a leading '.' 
	std::string		replaceExtension(const char *aFilename, const char *newExtension);
	std::string		getExtension(const char *aFilename);

	// Other functions --------------------------------
	virtual const std::string getAppFullPath(void);
	virtual const std::string getAppFullPathFilename(void);
	virtual const std::string getAppFilename(void);
	virtual const std::string getFileSystemReport(void);

	static void selfTest(void);

};



