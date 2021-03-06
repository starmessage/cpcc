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
#include <stdexcept>

#include "io.cpccLog.h"
#include "io.cpccFileSystemMini.h"
#include "fs.cpccPathHelper.h"
#include "cpcc_SelfTest.h"
#include "core.cpccIdeMacros.h"
#include "core.cpccTryAndCatch.h"
#include "io.cpccLogFileWriterWithBuffer.h"
#include "fs.cpccUserFolders.h"

#define cpccLogOpeningStamp		_T("cpccLog starting")
#define cpccLogClosingStamp		_T("cpccLog closing. Bye bye...")

int		cpccLogFormatter::m_IdentLevel = 0;
bool    cpccLogFormatter::m_enabled(true);



///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogFormatter
//
///////////////////////////////////////////////////////////////////////////////


cpccLogFormatter::cpccLogFormatter(const cpcc_char *aTag, const bool disableIfFileDoesNotExist, const bool echoToConsole) :
	m_tag(aTag ? aTag : _T("NULL aTag ")),
	//m_IdentText(_T("| ")),
	m_disableIfFileDoesNotExist(disableIfFileDoesNotExist),
	m_echoToConsole(echoToConsole),
	m_isEmpty(true)
{
	// std::cout << "creating cpccLogFormatter with tag:" << m_tag << std::endl;
	if (m_tag.length() == 0)
		std::cerr << "Error: #5613a: m_tag.length()==0" << std::endl;
	
}

void cpccLogFormatter::add(const cpcc_char* txt)
{
	if (!txt)
	{
		std::cerr << "cpccLogFormatter::add(NULL)" << std::endl;
		return;
	}

	// todo: reorganize the sequence of tasks here.
	// Consider if logs should be kept in memory while !m_enabled

	if (!m_enabled)
		return;

	cpcc_string _fn(cpccLogFileWriterWithBuffer::getInstance().getFilename());


	if ((_fn.length()>0) && !cpccFileSystemMini::fileExists(_fn.c_str()) && (!m_disableIfFileDoesNotExist))
		cpccFileSystemMini::createEmptyFile(_fn.c_str());	// create a file so that the log can continue wrting on it.

	cpcc_string m_outputBuffer;
	if (moreThanOneSecondPassed())
    {
        m_outputBuffer.append( getCurrentTime(_T("     \t%F  %X")) );
        m_outputBuffer.append(_T("\n"));
    }

	// Error here under WinXP
    if (! (m_tag.length()>0) )
        std::cerr << "Error: #5613b: !(m_tag.length()>0) with text=" << txt << std::endl;
    
    m_outputBuffer.append(m_tag);

	for (int i = 0; i<m_IdentLevel; ++i)
		m_outputBuffer.append(m_IdentText);
    
	m_outputBuffer.append(txt);
	m_outputBuffer.append(_T("\n"));
	
	// cpccFileSystemMini::appendTextFile(m_filename, m_outputBuffer);
	cpccLogFileWriterWithBuffer::getInstance().add(m_outputBuffer.c_str());

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
	CPCC_TRY_AND_CATCH_TO_CERR(_vstprintf_s(buff, MAX_LOG_STRING, format, args), "_vstprintf_s(buff, MAX_LOG_STRING, format, args)");
#else
	CPCC_TRY_AND_CATCH_TO_CERR( vsprintf(buff, format, args) , "vsprintf(buff, format, args)");
#endif
	va_end(args);
	return buff;
}


#ifdef _WIN32
#ifdef UNICODE

void cpccLogFormatter::addf(const wchar_t* format, ...)
{
// todo: Use toString()
	const int MAX_LOG_STRING = 8000;
	wchar_t buff[MAX_LOG_STRING + 1]; // = { 0 };

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

#endif
#endif

void cpccLogFormatter::addf(const char* format, ...)
{
// todo: Use toString()
	const int MAX_LOG_STRING = 8000 ;
	char buff[MAX_LOG_STRING + 1]; // = { 0 };

	va_list args;
	va_start(args, format);
#if (_MSC_VER >= 1400) // Visual Studio 2005
	// vsprintf_s( buff, MAX_LOG_STRING, format, args);
	//  _vstprintf_s for automatic unicode/ non-unicode

	CPCC_TRY_AND_CATCH_TO_CERR( vsprintf_s(buff, MAX_LOG_STRING, format, args), "vsprintf_s(buff, MAX_LOG_STRING, format, args)");
	// vsprintf_s(buff, MAX_LOG_STRING, format, args);
#else
	CPCC_TRY_AND_CATCH_TO_CERR( vsprintf(buff, format, args), "vsprintf(buff, format, args)");
#endif
	va_end(args);

	add(buff);
}


const cpcc_string &	cpccLogFormatter::getFilename(void)
{
    return cpccLogFileWriterWithBuffer::getInstance().getFilename();
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
	return buffer;  // e.g. 13:44:04
}





///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogManager
//
///////////////////////////////////////////////////////////////////////////////

// constructor
cpccLogManager::cpccLogManager(void):
        error(_T("ERROR>\t"),  !config_CreateFileOnError, config_EchoToCOUT),
        warning(_T("Warning>\t"), !config_CreateFileOnWarning, config_EchoToCOUT),
        info(_T("Info>\t"),  !config_CreateFileOnInfo, config_EchoToCOUT)
{
#ifdef cpccDEBUG
	cpcc_cout << _T("cpccLogManager constructor\n");
#endif
    info.add(cpccLogOpeningStamp);
#ifdef cpccDEBUG
    info.add(_T("Compiled in DEBUG mode"));
#else
    info.add(_T("Compiled in Release mode"));
#endif
    
    info.add(_T( "Application build timestamp:" ) __DATE__ _T("  ")  __TIME__);
    info.add(_T("More info about the cpcc cross platform library at:\n     \twww.StarMessageSoftware.com/cpcclibrary"));
#ifdef cpccDEBUG
    info.add(cpccFileSystemMini::getFileSystemReport());
#endif
}


cpccLogManager::~cpccLogManager()    // in MSVC, this destructor is not called
{
    //cpccFileSystemMini::appendTextFile("c:\\tmp\\a.txt", cpcc_string("this is the end"));
    info.add(cpccLogClosingStamp);
    if (hasErrors())
        copyToDesktop();
}


cpccLogManager &cpccLogManager::getInst(void)
{
	// static cpccLogManager _instance; // this does not work on WinXP inside a dll

	// std::cout << "getLogManagerSingleton()" << std::endl;
	static cpccLogManager *_instancePtr=NULL;
	// std::cout << "getLogManagerSingleton() step2" << std::endl;
	if (!_instancePtr)
		_instancePtr = new cpccLogManager;
	return *_instancePtr;
}


void cpccLogManager::initialize(const cpcc_char *appNameStem, const cpcc_char *macBundleId, const bool checkForIncompleteLog)
	{
		// info.addf("cpccLogManager::initialize() called");
		const cpcc_char *bundleID = NULL; // ignored in Windows
		#ifdef __APPLE__
			bundleID = macBundleId;
		#endif 

		info.addf("Compiler C/C++ standard:%s", cppcIDE::getCompilerVersion());
		cpcc_string fn = getAutoFullpathFilename(appNameStem, bundleID);
		// check previous run
		if ((checkForIncompleteLog && logfileIsIncomplete(fn.c_str()))
			||
            (config_CheckIfLogHasErrors && cpccFileSystemMini::fileContainsText(fn.c_str(), _T("ERROR>\t")))
			)
			copyToDesktop();
		
		// empty the file
		if (cpccFileSystemMini::fileExists(fn.c_str()))
			cpccFileSystemMini::createEmptyFile(fn.c_str());

		cpccLogFileWriterWithBuffer::getInstance().setFilename(fn.c_str());

		
		consolePut(_T("Log filename:") << fn);
		info.addf(_T("Log filename:%s"), fn.c_str());
        
        cpcc_string appfilename(cpccFileSystemMini::getAppFullPathFilename());
        consolePut(_T("c Initialize with App filename:") << appfilename);
        info.addf(_T("Initialize with App filename:%s"), appfilename.c_str());
				
		if (!cpccFileSystemMini::fileExists(fn.c_str()))
			consolePut(_T("Disabling log becase file does not exist at:") << fn.c_str());
	}



	

cpcc_string cpccLogManager::getFolderForTheLogFile(const cpcc_char *aBundleID)
{
	cpccPathString result(cpccUserFolders::getUsersCacheDir());
	if (aBundleID)
		if (cpcc_strlen( aBundleID)>1)
			result.appendPathSegment(aBundleID);

	if (!result.pathExists())
	{
		bool folderCreated = false;
		CPCC_TRY_AND_CATCH_TO_CERR(folderCreated = cpccFileSystemMini::createFolder(result.c_str()), "getFolderForTheLogFile()");
		if (!folderCreated)
			cpcc_cerr << _T("#8672: getFolderForTheLogFile() could not create folder:") << result << _T("\n");
	}
	return std::move(result);
}


cpcc_string cpccLogManager::getAutoFullpathFilename(const cpcc_char *aFilename, const cpcc_char *aBundleID)
{
        cpccPathString result(getFolderForTheLogFile(aBundleID));
		if (aFilename)
			result.appendPathSegment(aFilename);	
		else // chose an automatic filename
			result.appendPathSegment(cpccFileSystemMini::getAppFilename().c_str());

        result.append(_T(".cpccLog.txt"));
        return std::move(result);
}
	

    
bool    cpccLogManager::logfileIsIncomplete(const cpcc_char *fn)
{
    return (cpccFileSystemMini::fileContainsText(fn, cpccLogOpeningStamp) && !cpccFileSystemMini::fileContainsText(fn, cpccLogClosingStamp));
}
    
    
void    cpccLogManager::copyToDesktop(void)
{
    const cpcc_char *fn = cpccLogFileWriterWithBuffer::getInstance().getFilename().c_str();
    if (!cpccFileSystemMini::fileExists(fn))
        return;

    cpccFileSystemMini::copyFile(fn, cpccUserFolders::getDesktop().c_str());
}
    



/////////////////////////////////////////////////////////////////////////
// inside your program use the following functions to write to the 3 levels of the logs.
// all the 3 logs point to the same file.
/*
 cpccLogFormatter			&infoLog(void)		{ return cpccLogManager::getInst().info; }
 cpccLogFormatter			&warningLog(void)	{ return cpccLogManager::getInst().warning;  }
 cpccLogFormatter			&errorLog(void)		{ return cpccLogManager::getInst().error;  }
*/

cpccLogFormatter            &infoLog(void)        { return cpccLogManager::getInfo(); }
cpccLogFormatter            &warningLog(void)    { return cpccLogManager::getWarning();  }
cpccLogFormatter            &errorLog(void)        { return cpccLogManager::getError();  }
