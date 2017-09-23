/*  *****************************************
 *  File:		cpccLog.h
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
 
#pragma once

#include "cpccTimeCounter.h" // must be included before any windows.h. Otherwise it can produce: warning C4005: 'AF_IPX' : macro redefinition

#include <ctime>
#include <iostream> 
#include <assert.h>
#include <string> 
#include <atomic>

#include "core.cpccIdeMacros.h"
#include "cpccUnicodeSupport.h"


/*
struct cpccLogConfig
{
	// bool		checkForIncompleteLog;
	//cpcc_char *	AppNameStem;
    cpcc_string	AppNameStem;
#ifdef __APPLE__
    char *      bundleID;
#endif
};
*/


// this variable must be allocated somewhere in the main program's files to provide configuration parameters to the log class
// extern const cpccLogConfig &getLogConfig(void);
// extern const cpccLogConfig globalLogConfig;

class cpccLogFormatter
{
    
private:
	const cpcc_char *	m_tag;
	const cpcc_char *	m_IdentText = (cpcc_char *) _T("| ");
	bool  				m_isEmpty,
						m_disableIfFileDoesNotExist,
						m_echoToConsole;
	static int			m_IdentLevel;
	static std::atomic<bool> & isEnabled(void) { static std::atomic<bool> _enabled(true); return _enabled; };
	
public:
	static void		 increaseIdent(void) { ++m_IdentLevel; }
	static void		 decreaseIdent(void) { --m_IdentLevel; assert(m_IdentLevel >= 0 && "#9541: reducing log ident to <0"); }
	static void		 setEnabled(const bool enabled) { isEnabled() = enabled;  }

public: // constructor / destructor
	cpccLogFormatter(const cpcc_char *aTag, const bool disableIfFileDoesNotExist, const bool echoToConsole) :
		m_tag(aTag),
		m_disableIfFileDoesNotExist(disableIfFileDoesNotExist),
		m_echoToConsole(echoToConsole),
		m_isEmpty(true)
 	{ }


public: // functions
	bool 				isEmpty(void) const { return m_isEmpty; }
	static const cpcc_string &	getFilename(void);
	void				add(const std::string &txt) { add(txt.c_str()); }
	void 				add(const cpcc_char* txt);
	void 				addf(const cpcc_char* format, ...);
	// void				markLogClosure(void);
	static cpcc_string 	toString(const cpcc_char* format, ...);
    
	///  Get the current datetime as a human readable string
	/**
		This function is used to create the timestamp label for the log entries 
	 
		 @param  fmt the format specifier according to C++ strftime()
		 @return the current datetime formatted with the standard function strftime
	*/
	static cpcc_string 	getCurrentTime(const cpcc_char * fmt);
    static bool         moreThanOneSecondPassed(void);
};


// aliases for the 3 log levels
cpccLogFormatter			&infoLog(void);
cpccLogFormatter			&warningLog(void);
cpccLogFormatter			&errorLog(void);
#ifndef cpccDEBUG
    //#define COMMENT SLASH(/)
    //#define SLASH(s) /##s
	//#define debugLog()	COMMENT
    #define debugLog()	if (false) infoLog()
#else
	#define debugLog()	infoLog()
#endif

///////////////////////////////////////////////////////////////////////////////
//
// class cpccLogManager
//
///////////////////////////////////////////////////////////////////////////////


class cpccLogManager
{
private: // configuation

	enum {
		config_CheckIfLogHasErrors = true,
		config_CreateFileOnInfo = true,  // you might want to set this to false when compiling for release
		config_CreateFileOnWarning = true,
		config_CreateFileOnError = true,
		#ifndef cpccDEBUG
			echoToCOUT = false
		#else
			echoToCOUT = true
		#endif
		};

	bool config_checkForIncompleteLog;


public: 	// data
	cpccLogFormatter	error, warning, info;

public:
	explicit cpccLogManager(const bool checkForIncompleteLog);

	virtual ~cpccLogManager();	// in MSVC, this destructor is not called when run from a screensaver


public: // functions

	void initialize( const char *appNameStem, const char *macBundleId);

	
private:
	// find the appropriate folder and create it if it does not exist
	static cpcc_string getFolderForTheLogFile(const char *aBundleID);

	cpcc_string getAutoFullpathFilename(const cpcc_char *aFilename, const char *aBundleID) const;

	static bool    fileContainsText(const cpcc_char *fn, const cpcc_char *txt);

	static bool    logfileIsIncomplete(const cpcc_char *fn);

	void    copyToDesktop(void);

	inline bool hasErrors(void) const { return !error.isEmpty(); }
};

// you must allocate somewhere in the cpp code this object.
extern cpccLogManager _singletonAppLog;

/////////////////////////////////////////////////////////////////////////
//
//		helper classes
//
//
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// this class is used to automatically log creation/desctuction messages
// when the container object is created / destructed
class logBlockOfCode
{
protected:
    cpcc_string tag, startTag, endTag;
    
public:
    logBlockOfCode(const cpcc_char *aTag, const cpcc_char *aStartTag, const cpcc_char *aEndTag) :
        tag(aTag), startTag(aStartTag), endTag(aEndTag)
    {
        infoLog().addf(_T("%s: %s"), startTag.c_str(), tag.c_str());
        cpccLogFormatter::increaseIdent();
    }
    
    virtual ~logBlockOfCode(void)
    {
        cpccLogFormatter::decreaseIdent();
        infoLog().addf(_T("%s: %s"), endTag.c_str(), tag.c_str());
    }
};


class logObjectLife: protected logBlockOfCode
{
public:
	explicit logObjectLife(const cpcc_char *aTag) : logBlockOfCode(aTag, _T("creating"), _T("destroying"))
	{ }
};


class logFunctionLife: protected logBlockOfCode
{
public:
    explicit logFunctionLife(const cpcc_char *aTag) : logBlockOfCode(aTag, _T("Entering"), _T("Exiting"))
    { }
};


class logTimeCountrer : public logFunctionLife
{
private:
	cpccTimeCounter timer;

public:

	explicit logTimeCountrer(const cpcc_char *aTag) : logFunctionLife(aTag)
	{
		// timer.resetTimer();
	}

	virtual ~logTimeCountrer(void)
	{
        tag.append(cpccLogFormatter::toString(_T(". Duration:%.3f sec"),timer.getSecondsElapsed()));
	}

};


