﻿/*  *****************************************
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

#include "cpccUnicodeSupport.h"




class cpccLogSink
{
private:
	const cpcc_char *	m_tag;
	static cpcc_char *	m_IdentText;
	bool  				m_isEmpty,
						m_disableIfFileDoesNotExist,
						m_echoToConsole;
	static int			m_IdentLevel;
	
public:
	cpcc_string			m_filename;

public:
	static void		 increaseIdent(void) { ++m_IdentLevel; }
	static void		 decreaseIdent(void) { --m_IdentLevel; assert(m_IdentLevel >= 0 && "#9541: reducing log ident to <0"); }

public: // constructor / destructor
	cpccLogSink(const cpcc_char *aTag, const bool disableIfFileDoesNotExist, const bool echoToConsole) :
		m_tag(aTag),
		m_disableIfFileDoesNotExist(disableIfFileDoesNotExist),
		m_echoToConsole(echoToConsole),
		m_isEmpty(true)
 	{ }
		
	// virtual ~cpccLogSink();

	

public: // functions
	bool 				isEmpty(void) { return m_isEmpty; }
	const cpcc_string &	getFilename(void) const { return m_filename; };
	void 				add(const cpcc_char* txt);
	void 				addf(const cpcc_char* format, ...);
	static cpcc_string 	toString(const cpcc_char* format, ...);
    
	///  Get the current datetime as a human readable string
	/**
		This function is used to create the timestamp label for the log entries 
	 
		 @param  fmt the format specifier according to C++ strftime()
		 @return the current datetime formatted with the standard function strftime
	*/
	static cpcc_string 	getCurrentTime(const cpcc_char * fmt);
};


// aliases for the 3 log levels
cpccLogSink			&infoLog(void);
cpccLogSink			&warningLog(void);
cpccLogSink			&errorLog(void);

#ifdef NDEBUG
    //#define COMMENT SLASH(/)
    //#define SLASH(s) /##s
	//#define debugLog()	COMMENT
    #define debugLog()	if (false) infoLog()
#else
	#define debugLog()	infoLog()
#endif

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
        infoLog().addf("%s: %s", startTag.c_str(), tag.c_str());
        cpccLogSink::increaseIdent();
    }
    
    virtual ~logBlockOfCode(void)
    {
        cpccLogSink::decreaseIdent();
        infoLog().addf("%s: %s", endTag.c_str(), tag.c_str());
    }
};


class logObjectLife: protected logBlockOfCode
{
public:
	logObjectLife(const cpcc_char *aTag) : logBlockOfCode(aTag, "creating", "destroying")
	{ }
};


class logFunctionLife: protected logBlockOfCode
{
public:
    logFunctionLife(const cpcc_char *aTag) : logBlockOfCode(aTag, "Entering", "Exiting")
    { }
};


class logTimeCountrer : public logFunctionLife
{
private:
	cpccTimeCounter timer;

public:

	logTimeCountrer(const cpcc_char *aTag) : logFunctionLife(aTag)
	{
		// timer.resetTimer();
	}

	virtual ~logTimeCountrer(void)
	{
        tag.append(cpccLogSink::toString(". Duration:%.3f sec",timer.getSecondsElapsed()));
	}

};





