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
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include <fstream>
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

	if (!cpccFileSystemMini::fileExists(m_filename) && (m_disableIfFileDoesNotExist))
		return;

	cpcc_string m_outputBuffer(m_tag);

    if (moreThanOneSecondPassed())
    {
        cpcc_string currentTime = getCurrentTime(_T("     \t%F  %X"));
        currentTime.append("\n");
        m_outputBuffer.insert(0, currentTime);
    }
	
	for (int i = 0; i<m_IdentLevel; ++i)
		m_outputBuffer.append(m_IdentText);

	m_outputBuffer.append(txt);
	m_outputBuffer.append("\n");

	cpccFileSystemMini::appendTextFile(m_filename, m_outputBuffer);
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


bool    cpccLogSink::moreThanOneSecondPassed(void) // this is used to avoid writing the time for every log line
{
    static time_t previousLocalEpoch=0;
    // The value returned generally represents the number of seconds since 00:00 hours, Jan 1, 1970 UTC
    time_t localEpoch = time(NULL);
    
    if (localEpoch==previousLocalEpoch)
        return false;
    
    previousLocalEpoch=localEpoch;
    return true;
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

/*
cpccLogSink::~cpccLogSink()
{
	cpccFileSystemMini::appendTextFile("c:\\tmp\\b.txt", "this is the end");
	#if defined(_WIN32)
		OutputDebugString("~cpccLogSink() called");
	#endif
}
*/

///////////////////////////////////////////////////////////////////////////////
//
// class cpccLog
//
///////////////////////////////////////////////////////////////////////////////


class cpccLog
{
private: // configuation
	enum    {
            CreateFileOnInfo=true,  // you might want to set this to false when compiling for release
            CreateFileOnWarning=true,
            CreateFileOnError=true,
	#ifdef NDEBUG
            echoToCOUT=false
	#else
	        echoToCOUT=true
	#endif
            };
    
public: 	// data
	cpccLogSink	error, warning, info;
    
public:
	cpccLog(/* const char *aFilename*/  void ):
		error("ERROR>\t",  !CreateFileOnError, echoToCOUT),
		warning("Warning>\t", !CreateFileOnWarning, echoToCOUT),
		info("Info>\t",  !CreateFileOnInfo, echoToCOUT)
	{
		cpcc_string fn = getAutoFilename();
		const char *aFilename = fn.c_str();
        info.m_filename = aFilename;
        warning.m_filename = aFilename;
        error.m_filename = aFilename;
        
        // check previous run
        if (logfileIsIncomplete())
            copyToDesktop();
        
		// empty the file
		if (cpccFileSystemMini::fileExists(aFilename))
			cpccFileSystemMini::createEmptyFile(aFilename);
        

        info.add(cpccLogOpeningStamp);
        consolePut( "Log filename:" << aFilename );
        info.addf("Log filename:%s", aFilename);
        if (!cpccFileSystemMini::fileExists(aFilename))
            consolePut( "Disabling log becase file does not exist at:" << aFilename );
        
        info.add(_T( "Application build timestamp:" ) __DATE__ _T("  ")  __TIME__);
		info.add("More info about the cpcc cross platform library at: www.StarMessageSoftware.com/cpcclibrary");
        info.add(cpccFileSystemMini::getFileSystemReport().c_str());
	}
	
    
	virtual ~cpccLog()
	{	
		//cpccFileSystemMini::appendTextFile("c:\\tmp\\a.txt", cpcc_string("this is the end"));
		info.add(cpccLogClosingStamp);
		/*
        std::cout << cpccLogClosingStamp << std::endl;
		#if defined(_WIN32)
			OutputDebugString("~cpccLog() called");
		#endif
		*/
        if (containsText("ERROR>\t"))
            copyToDesktop();
	}
	

    
public: // functions
	bool hasErrors(void) { return !error.isEmpty(); }


	cpccPathString getAutoFilename(void) const
	{
        cpccPathString result(cpccPathString::sfUsersTemp);
        result.appendPathSegment(cpccFileSystemMini::getAppFilename().c_str());
        result.append(_T(".cpccLog.txt"));
        return result;
	}
	
    
    bool    containsText(const char *txt) const
    {
        const char *fn = info.m_filename.c_str();
        if (!cpccFileSystemMini::fileExists(fn))
            return false;
        
        std::ifstream thefile( fn);
        if (!thefile.good())
            return false;
        
        cpcc_string line;
        while (std::getline(thefile, line ))  // same as: while (getline( myfile, line ).good())
        {
            if (line.find(txt) != std::string::npos)
            {
                thefile.close();
                return true;
            }
        }
        thefile.close();
        return false;
    }
    
    
    bool    logfileIsIncomplete(void)
    {
        return (containsText(cpccLogOpeningStamp) && !containsText(cpccLogClosingStamp));
    }
    
    
    void    copyToDesktop(void)
    {
        const char *fn = info.m_filename.c_str();
        if (!cpccFileSystemMini::fileExists(fn))
            return;

        cpccFileSystemMini::copyFile(fn, cpccFileSystemMini::getFolder_Desktop().c_str());
    }
    
};



cpcc_char *	cpccLogSink::m_IdentText =  (cpcc_char *)"| ";
int	cpccLogSink::m_IdentLevel = 0;

namespace ns_cpccGlobals
{
	cpccLog globalAppLog;
}



/////////////////////////////////////////////////////////////////////////
// inside the program use the following functions to write to the 3 logs.
// all the 3 logs point to the same file.
cpccLogSink			&infoLog(void) { return ns_cpccGlobals::globalAppLog.info; }
cpccLogSink			&warningLog(void) { return ns_cpccGlobals::globalAppLog.warning; }
cpccLogSink			&errorLog(void) { return ns_cpccGlobals::globalAppLog.error; }


namespace ns_cpccGlobals
{
	logObjectLife  logFileMarker("logFileMarker");
}
