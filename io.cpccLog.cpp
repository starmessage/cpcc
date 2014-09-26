/*  *****************************************
 *  File:		cpccLog.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to write application information to a log file
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
#include "io.cpccLog.h"


///////////////////////////////////////////////////////////////////////////////
//
// class cpccLog
//
///////////////////////////////////////////////////////////////////////////////

#define cpccLogOpeningStamp		_T("cpccLog starting")
#define cpccLogClosingStamp		_T("cpccLog closing")


class cpccLog
{
private: // configuation
	enum { CreateFileOnInfo=false, CreateFileOnWarning=true, CreateFileOnError=true, echoToCOUT=true };
    
public: 	// data
	cpccLogFile	error, warning, info;
    
public:
	cpccLog(const char *aFilename):
    error("ERROR>\t", aFilename, !CreateFileOnError, echoToCOUT),
    warning("Warning>\t", aFilename, !CreateFileOnWarning, echoToCOUT),
    info("Info>\t", aFilename, !CreateFileOnInfo, echoToCOUT)
	{
		cpccFileSystemMini fs;
		// empty the file
		if (fs.fileExists(aFilename))
			fs.createEmptyFile(aFilename);
        else
		{
            std::cout << "Log not created at:" << aFilename << "\n";
			#if defined(_WIN32)
				OutputDebugString("Log not created at:");
				OutputDebugString(aFilename);
				OutputDebugString( "\n");
			#endif
		}

		info.add(cpccLogOpeningStamp);
		info.add(_T( "Application build timestamp:" ) __DATE__ _T("  ")  __TIME__);
		info.addf("Log saved at:%s", aFilename);
        std::cout << "Log saved at:" << aFilename << "\n";
		#if defined(_WIN32)
			OutputDebugString("Log at:");
			OutputDebugString(aFilename);
			OutputDebugString( "\n");
		#endif
		info.add("More info about the cpcc cross plaform library at: www.StarMessageSoftware.com/cpcclibrary\n");
	}
	
    
	~cpccLog()
	{	info.add(cpccLogClosingStamp);
	}
	
    
public: // functions
	bool hasErrors(void) { return !error.isEmpty(); }
	
};



///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogAutoFilename
//
///////////////////////////////////////////////////////////////////////////////

class cpccLogAutoFilename: public cpccLog
{
public:
	cpccLogAutoFilename():	cpccLog(getAutoFilename().c_str())
	{  }
    
	static cpcc_string getAutoFilename(void)
	{
		cpccFileSystemMini	fs;
		cpccPathHelper		ph;
		static cpcc_string result;
		result = fs.getFolder_UsersTemp();
		ph.addTrailingPathDelimiter(result);
		result.append(fs.getAppFilename() + _T(".cpccLog.txt"));
		return result;
	}
    
};



// lazy but early enough constructor for the logging object
cpccLogAutoFilename &appLogNew(void)
{
	static cpccLogAutoFilename m_appLog;
	return m_appLog;
}


/////////////////////////////////////////////////////////////////////////
// inside the program use the following functions to write to the 3 logs.
// all the 3 logs point to the same file.

cpccLogFile			&infoLog(void)		{	return appLogNew().info; }
cpccLogFile			&warningLog(void)	{	return appLogNew().warning; }
cpccLogFile			&errorLog(void)		{	return appLogNew().error; }




