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
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#include "cpcc_SelfTest.h"


#define cpccLogOpeningStamp		_T("cpccLog starting")
#define cpccLogClosingStamp		_T("cpccLog closing")



///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogSink
//
///////////////////////////////////////////////////////////////////////////////


void cpccLogSink::add(const cpcc_char* txt)
{
	if (m_filename.length() == 0)
		return;

	cpccFileSystemMiniEx fs;
	if (!fs.fileExists(m_filename) && (m_disableIfFileDoesNotExist))
		return;

	static cpcc_string m_outputBuffer;
	m_outputBuffer = m_tag;

	static cpcc_string previousTime, currentTime;
	currentTime = getCurrentTime(_T("%X"));

	if (currentTime != previousTime)	// log the time only when it changes, in a separate line
	{
		previousTime = currentTime;
		currentTime.append("\n");
		m_outputBuffer.insert(0, currentTime);
	}
	
	for (int i = 0; i<m_IdentLevel; ++i)
		m_outputBuffer.append(m_IdentText);

	m_outputBuffer.append(txt);
	m_outputBuffer.append("\n");

	fs.appendTextFile(m_filename, m_outputBuffer);
	if (m_echoToConsole)
	{
		std::cout << m_outputBuffer;
#if defined(_WIN32)
		OutputDebugString(m_outputBuffer.c_str());
#endif
	}
	m_isEmpty = false;
}


cpcc_string cpccLogSink::toString(const cpcc_char* format, ...)
{
	const int MAX_LOG_STRING = 8000;
	cpcc_char buff[MAX_LOG_STRING + 1];

	va_list args;
	va_start(args, format);
#if (_MSC_VER >= 1400) // Visual Studio 2005
	// vsprintf_s( buff, MAX_LOG_STRING, format, args);
	_vstprintf_s(buff, MAX_LOG_STRING, format, args);
#else
	vsprintf(buff, format, args);
#endif
	va_end(args);

	return buff;
}


void cpccLogSink::addf(const cpcc_char* format, ...)
{
	const int MAX_LOG_STRING = 8000;
	cpcc_char buff[MAX_LOG_STRING + 1];

	va_list args;
	va_start(args, format);
#if (_MSC_VER >= 1400) // Visual Studio 2005
	// vsprintf_s( buff, MAX_LOG_STRING, format, args);
	_vstprintf_s(buff, MAX_LOG_STRING, format, args);
#else
	vsprintf(buff, format, args);
#endif
	va_end(args);

	add(buff);
}


///  Get the current datetime as a human readable string
/**
This function is used to create the timestamp field of the logRecord

@param  fmt the format specifier according to C++ strftime()
@return the current datetime formatted with the standard function strftime
*/
cpcc_string cpccLogSink::getCurrentTime(const cpcc_char * fmt)
{
	time_t t = time(0);   // get time now
#pragma warning( disable : 4996 )
	struct tm timeStruct = *localtime(&t);

	cpcc_char buffer[100];

	// More information about date/time format
	// http://www.cplusplus.com/reference/clibrary/ctime/strftime/
	// strftime(buffer, sizeof(buffer), fmt, &timeStruct);
	cpcc_strftime(buffer, sizeof(buffer), fmt, &timeStruct);
	return cpcc_string(buffer);  // e.g. 13:44:04
}


///////////////////////////////////////////////////////////////////////////////
//
// class cpccLog
//
///////////////////////////////////////////////////////////////////////////////


class cpccLog
{
private: // configuation
	enum    {
            CreateFileOnInfo=true,  // you might want to set this to false when compiling for production
            CreateFileOnWarning=true,
            CreateFileOnError=true,
            echoToCOUT=true
            };
    
public: 	// data
	cpccLogSink	error, warning, info;
    
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
        
        info.add(cpccLogOpeningStamp);
        consolePut( "Log filename:" << aFilename );
        info.addf("Log filename:%s", aFilename);
        if (!fs.fileExists(aFilename))
            consolePut( "Disabling log becase file does not exist at:" << aFilename );
        
        info.add(_T( "Application build timestamp:" ) __DATE__ _T("  ")  __TIME__);
		info.add("More info about the cpcc cross platform library at: www.StarMessageSoftware.com/cpcclibrary\n");
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

cpcc_char *	cpccLogSink::m_IdentText =  (cpcc_char *)"| ";
int	cpccLogSink::m_IdentLevel = 0;


/////////////////////////////////////////////////////////////////////////
// inside the program use the following functions to write to the 3 logs.
// all the 3 logs point to the same file.

cpccLogSink			&infoLog(void)		{ return appLogNew().info; }
cpccLogSink			&warningLog(void)	{ return appLogNew().warning; }
cpccLogSink			&errorLog(void)		{ return appLogNew().error; }




