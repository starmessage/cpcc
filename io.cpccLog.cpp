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
#include <sstream>

#include "io.cpccLog.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#include "cpcc_SelfTest.h"
#include "core.cpccIdeMacros.h"
#include "cpccUnicodeSupport.h"

#define cpccLogOpeningStamp		_T("cpccLog starting")
#define cpccLogClosingStamp		_T("cpccLog closing. Bye bye...")



///////////////////////////////////////////////////////////////////////////////
//
// 	class cpccFileWithBuffer
//
//	class for writing to a text file.
//
//  It has an internal buffer where the writing is directed until the filename is specified.
//  When setFilename() is called, the buffer is flushed in the file and any future logging occurs directly to the file.
//  This allows the application to start logging events, even before it has the opportunity to calculate the filename.
//  This is useful because a logObject is usally a global or singleton variable and you do not know exactly when it starts being used.
//
//  If the file specified does not exist, logging is disabled. This is a way for the application to enable/disable the logging.
//  To enable the logging, the application must first create the file (unless it already exists) before calling setFilename()
//   
///////////////////////////////////////////////////////////////////////////////
class  cpccFileWriterWithBuffer
{
private:
	cpcc_string m_filename;
	cpcc_stringstream m_buffer;
	int maxBufferedLines;
	
public:
	cpccFileWriterWithBuffer(): maxBufferedLines(200) 	{  add("cpccFileWriterWithBuffer starting.\n"); }

	virtual ~cpccFileWriterWithBuffer()  { 	add("cpccFileWriterWithBuffer closing.\n"); }
	
	void setFilename(const cpcc_char* aFilename) 
	{ 
		assert((m_filename.length() == 0) && "#4721: cpccFileWriterWithBuffer.setFilename() already called.");
		if (aFilename)
			m_filename = aFilename;
		else
			return;

		if (cpccFileSystemMini::fileExists(m_filename.c_str())) // empty the file if it exists already
		{
			cpccFileSystemMini::createEmptyFile(m_filename);
			cpccFileSystemMini::appendTextFile(m_filename, m_buffer.str());
		}
	}

	const cpcc_string &getFilename(void) const { return m_filename; }


	void add(const cpcc_char* txt) 
	{
		if (m_filename.length() > 0)
		{
			if (cpccFileSystemMini::fileExists(m_filename.c_str()))
				cpccFileSystemMini::appendTextFile(m_filename, txt);
		}
		else
		{
			if (maxBufferedLines>0)
				m_buffer << txt;
			--maxBufferedLines;
		}
	}

};


static cpccFileWriterWithBuffer gbl_logWriterWithBuffer;


///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogFormatter
//
///////////////////////////////////////////////////////////////////////////////


void cpccLogFormatter::add(const cpcc_char* txt)
{
	if (!isEnabled())
		return;

	//if (m_filename.length() == 0)
	//	return;
	cpcc_string _fn(gbl_logWriterWithBuffer.getFilename());
	
	
	if ((_fn.length()>0) && !cpccFileSystemMini::fileExists(_fn.c_str()) && (!m_disableIfFileDoesNotExist))
		cpccFileSystemMini::createEmptyFile(_fn);	// create a file so that the log can continue wrting on it.

	cpcc_string m_outputBuffer(m_tag);

    if (moreThanOneSecondPassed())
    {
        cpcc_string currentTime = getCurrentTime(_T("     \t%F  %X"));
        currentTime.append(_T("\n"));
        m_outputBuffer.insert(0, currentTime);
    }
	
	for (int i = 0; i<m_IdentLevel; ++i)
		m_outputBuffer.append(m_IdentText);

	m_outputBuffer.append(txt);
	m_outputBuffer.append(_T("\n"));

	// cpccFileSystemMini::appendTextFile(m_filename, m_outputBuffer);
	gbl_logWriterWithBuffer.add(m_outputBuffer.c_str());

	if (m_echoToConsole)
	{
		cpcc_cout << m_outputBuffer;
#if defined(_WIN32)
		OutputDebugString(m_outputBuffer.c_str());
#endif
	}
	m_isEmpty = false;
}


cpcc_string cpccLogFormatter::toString(const cpcc_char* format, ...)
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


void cpccLogFormatter::addf(const cpcc_char* format, ...)
{
	const int MAX_LOG_STRING = 8000 ;
	cpcc_char buff[MAX_LOG_STRING + 1]; // = { 0 };

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


const cpcc_string &	cpccLogFormatter::getFilename(void)
{
    return gbl_logWriterWithBuffer.getFilename();
}


bool    cpccLogFormatter::moreThanOneSecondPassed(void) // this is used to avoid writing the time for every log line
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
cpcc_string cpccLogFormatter::getCurrentTime(const cpcc_char * fmt)
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



int					cpccLogFormatter::m_IdentLevel = 0;
// std::atomic<bool>	cpccLogFormatter::m_enabled=true;


///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogManager
//
///////////////////////////////////////////////////////////////////////////////


void cpccLogManager::initialize(const char *appNameStem, const char *macBundleId)
	{
		const char *bundleID = NULL;
		#ifdef __APPLE__
			bundleID = macBundleId;
		#endif 

        info.addf("cpccLogManager::initialize() called");
		cpcc_string fn = getAutoFullpathFilename(appNameStem, bundleID);
		// check previous run
		if ((config_checkForIncompleteLog && logfileIsIncomplete(fn.c_str()))
			||
			(config_CheckIfLogHasErrors && fileContainsText(fn.c_str(), _T("ERROR>\t")))
			)
			copyToDesktop();


		// empty the file
		if (cpccFileSystemMini::fileExists(fn.c_str()))
			cpccFileSystemMini::createEmptyFile(fn.c_str());
		gbl_logWriterWithBuffer.setFilename(fn.c_str());

		consolePut(_T("Log filename:") << fn.c_str());
		info.addf(_T("Log filename:%s"), fn.c_str());
		if (!cpccFileSystemMini::fileExists(fn.c_str()))
			consolePut(_T("Disabling log becase file does not exist at:") << fn.c_str());
	}


cpccLogManager::cpccLogManager(const bool checkForIncompleteLog):
		error(_T("ERROR>\t"),  !config_CreateFileOnError, echoToCOUT),
		warning(_T("Warning>\t"), !config_CreateFileOnWarning, echoToCOUT),
		info(_T("Info>\t"),  !config_CreateFileOnInfo, echoToCOUT),
		config_checkForIncompleteLog(checkForIncompleteLog)
	{
		        
        info.add(cpccLogOpeningStamp);
		#ifdef cpccDEBUG
			info.add("Compiled in DEBUG mode");
		#else
			info.add("Compiled in Release mode");
		#endif
        
        info.add(_T( "Application build timestamp:" ) __DATE__ _T("  ")  __TIME__);
		info.add(_T("More info about the cpcc cross platform library at: www.StarMessageSoftware.com/cpcclibrary"));
        info.add(cpccFileSystemMini::getFileSystemReport().c_str());
	}
	
    
cpccLogManager::~cpccLogManager()	// in MSVC, this destructor is not called
	{	
		//cpccFileSystemMini::appendTextFile("c:\\tmp\\a.txt", cpcc_string("this is the end"));
		info.add(cpccLogClosingStamp);
        if (hasErrors())
            copyToDesktop();
	}
	

cpcc_string cpccLogManager::getFolderForTheLogFile(const char *aBundleID)
	{
		cpccPathString result(cpccFileSystemMini::getFolder_UsersCache());
		if (aBundleID)
			result.appendPathSegment(aBundleID);
		if (!result.pathExists())
			if (!cpccFileSystemMini::createFolder(result.c_str()))
				cpcc_cerr << _T("#8672: getFolderForTheLogFile() could not create folder:") << result << _T("\n");
		return result;
	}


cpcc_string cpccLogManager::getAutoFullpathFilename(const cpcc_char *aFilename, const char *aBundleID) const
	{
        cpccPathString result(getFolderForTheLogFile(aBundleID));
		if (aFilename)
			result.appendPathSegment(aFilename);	
		else // chose an automatic filename
			result.appendPathSegment(cpccFileSystemMini::getAppFilename().c_str());

        result.append(_T(".cpccLog.txt"));
        return result;
	}
	
    
bool    cpccLogManager::fileContainsText(const cpcc_char *fn, const cpcc_char *txt)
    {
        if (!cpccFileSystemMini::fileExists(fn))
            return false;
        
        cpcc_ifstream thefile( fn);
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
    
    
 bool    cpccLogManager::logfileIsIncomplete(const cpcc_char *fn)
    {
        return (fileContainsText(fn, cpccLogOpeningStamp) && !fileContainsText(fn, cpccLogClosingStamp));
    }
    
    
void    cpccLogManager::copyToDesktop(void)
    {
        const cpcc_char *fn = gbl_logWriterWithBuffer.getFilename().c_str();
        if (!cpccFileSystemMini::fileExists(fn))
            return;

        cpccFileSystemMini::copyFile(fn, cpccFileSystemMini::getFolder_Desktop().c_str());
    }
    






/*
cpccLogManager &getInstance_cpccLog(void)
{
	static cpccLogManager _singletonAppLog(globalLogConfig.logFilename, globalLogConfig.checkForIncompleteLog, globalLogConfig.checkHasErrors);
	return _singletonAppLog;
}
*/

/////////////////////////////////////////////////////////////////////////
// inside your program use the following functions to write to the 3 levels of the logs.
// all the 3 logs point to the same file.
cpccLogFormatter			&infoLog(void)		{ return _singletonAppLog.info; }
cpccLogFormatter			&warningLog(void)	{ return _singletonAppLog.warning;  }
cpccLogFormatter			&errorLog(void)		{ return _singletonAppLog.error;  }


/*
void initializeLog(const cpccLogConfig &aConfig)
{
	static bool _initialized = false;
	if (!_initialized)
		_singletonAppLog.initialize(aConfig);
	_initialized = true;
}
*/
