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


class cpccLogFormatter
{
	
private:

	const cpcc_char *	m_IdentText = _T("| "); 
	// const cpcc_string	m_IdentText;   	
	const cpcc_string	m_tag;  // m_tag gets empty in winXP

	bool  				m_isEmpty,
						m_disableIfFileDoesNotExist,
						m_echoToConsole;
	static int			m_IdentLevel;
	
	// static std::atomic<bool> & isEnabled(void) { static std::atomic<bool> _enabled(true); return _enabled; };
	// static bool	& 	isEnabled(void) { static bool _enabled(true); return _enabled; };
	static bool			m_enabled;
	
public:
	static void		 increaseIdent(void) { ++m_IdentLevel; assert(m_IdentLevel > 0 && "#9541a: increased log ident to <=0"); }
	static void		 decreaseIdent(void) { --m_IdentLevel; assert(m_IdentLevel >= 0 && "#9541b: reducing log ident to <0"); }
	static void		 setEnabled(const bool enabled) { m_enabled = enabled;  }

public: // constructor / destructor
	/*
	Always declare a copy constructor and assignment operator
	Many classes shouldn't be copied or assigned. Then, to enforce your policy declare a deleted copy constructor as private and
	not supply a definition.
	Do the same for the assignment operator.

	*/
	cpccLogFormatter(const cpccLogFormatter& x) = delete;
	cpccLogFormatter& operator=(const cpccLogFormatter& x) = delete;

	explicit cpccLogFormatter(const cpcc_char *aTag, const bool disableIfFileDoesNotExist, const bool echoToConsole);
	
public: // functions
	bool 				isEmpty(void) const { return m_isEmpty; }
	static const cpcc_string &	getFilename(void);
	inline void			add(const cpcc_string &txt) { add(txt.c_str()); }
	void 				add(const cpcc_char* txt);


#ifdef _WIN32
#ifdef UNICODE
	void 				add(const char* txt) { wchar_from_char wtxt(txt); add(wtxt.get()); }
	void				addf(const wchar_t* format, ...);
#endif
#endif
	void 				addf(const char* format, ...);

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

	const bool	config_CheckIfLogHasErrors = true,
				config_CreateFileOnInfo = true,  // you might want to set this to false when compiling for release
				config_CreateFileOnWarning = true,
				config_CreateFileOnError = true,
				#ifndef cpccDEBUG
					config_EchoToCOUT = false;
				#else
					config_EchoToCOUT = true;
				#endif


private: 	// data
	cpccLogFormatter	error, warning, info;

private:
	// this is private because only &getInst() should create this object as singleton
	explicit cpccLogManager(void);
	cpccLogManager(const cpccLogManager& x) = delete;
	cpccLogManager& operator=(const cpccLogManager& x) = delete;

public:
	virtual ~cpccLogManager();	// in MSVC, this destructor is not called when run from a screensaver

	static cpccLogManager   &getInst(void);
    static cpccLogFormatter &getInfo(void) { return getInst().info; }
    static cpccLogFormatter &getWarning(void) { return getInst().warning; }
    static cpccLogFormatter &getError(void) { return getInst().error; }
    
public: // functions

	void initialize(const cpcc_char *appNameStem, const cpcc_char *macBundleId, const bool checkForIncompleteLog);

    // static bool    fileContainsText(const cpcc_char *fn, const cpcc_char *txt);

	
private:
	// find the appropriate folder and create it if it does not exist
	static cpcc_string getFolderForTheLogFile(const cpcc_char *aBundleID);

	static cpcc_string getAutoFullpathFilename(const cpcc_char *aFilename, const cpcc_char *aBundleID);

	
	static bool    logfileIsIncomplete(const cpcc_char *fn);

	void    copyToDesktop(void);

	inline bool hasErrors(void) const { return !error.isEmpty(); }
};




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
private:
	const cpcc_string startTag, endTag;

protected:
	cpcc_string tag;
	    

    explicit logBlockOfCode(const cpcc_char *aTag, const cpcc_char *aStartTag, const cpcc_char *aEndTag) :
        tag(aTag?aTag:_T("null-tag-at-logBlockOfCode")), startTag(aStartTag?aStartTag:_T("aStartTag")), endTag(aEndTag? aEndTag:_T("aEndTag"))
    {
        infoLog().addf(_T("%s: %s"), startTag.c_str(), tag.c_str());
		
        cpccLogFormatter::increaseIdent();
    }
    
public:
    virtual ~logBlockOfCode(void)
    {
        cpccLogFormatter::decreaseIdent();
        infoLog().addf(_T("%s: %s\n"), endTag.c_str(), tag.c_str());
    }
};


class logObjectLife: protected logBlockOfCode
{
public:
	explicit logObjectLife(const cpcc_char *aTag) : logBlockOfCode(aTag ? aTag : _T("null-tag-at-logObjectLife"), _T("creating"), _T("destroying"))
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


