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
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
#pragma once

#include <ctime>
#include <iostream> 
// #include <typeinfo>     // to find automatically the class name, eg.    cout << typeid(*this).name() << endl;
#include <assert.h>
#include <string> 

#include "cpccTimeCounter.h" // must be included before any windows.h. Otherwise it can produce: warning C4005: 'AF_IPX' : macro redefinition
#include "cpccUnicodeSupport.h"




class cpccLogSink
{
private:
	const cpcc_char *m_tag;
	cpcc_string		 m_filename;
	bool  			 m_isEmpty,
					 m_disableIfFileDoesNotExist,
					 m_echoToConsole;

	static int		 m_IdentLevel;
	static cpcc_char *m_IdentText;

public:
	static void		 increaseIdent(void) { ++m_IdentLevel; }
	static void		 decreaseIdent(void) { --m_IdentLevel; assert(m_IdentLevel >= 0 && "#9541: reducing log ident to <0"); }

public: // constructor / destructor
	cpccLogSink(const cpcc_char *aTag, const cpcc_char *aFilename, const bool disableIfFileDoesNotExist, const bool echoToConsole) :
		m_filename(aFilename),
		m_tag(aTag),
		m_disableIfFileDoesNotExist(disableIfFileDoesNotExist),
		m_echoToConsole(echoToConsole),
		m_isEmpty(true)
 	{ }
	
    
public: // functions
	bool 				isEmpty(void) { return m_isEmpty; }
	const cpcc_string &	getFilename(void) const { return m_filename; };
	void 				add(const cpcc_char* txt);
	void 				addf(const cpcc_char* format, ...);
	static cpcc_string 	toString(const cpcc_char* format, ...);
    
	///  Get the current datetime as a human readable string
	/**
		This function is used to create the timestamp field of the logRecord
	 
		 @param  fmt the format specifier according to C++ strftime()
		 @return the current datetime formatted with the standard function strftime
	*/
	static cpcc_string 	getCurrentTime(const cpcc_char * fmt);
};




// lazy but early enough constructor for the logging object

cpccLogSink			&infoLog(void);
cpccLogSink			&warningLog(void);
cpccLogSink			&errorLog(void);


/////////////////////////////////////////////////////////////////////////
// this class is used to automatically log creation/desctuction messages
// when the container object is created / destructed

class logObjectLife
{
protected:
    cpcc_string tag;
public:
	logObjectLife(const cpcc_char *aTag) : tag(aTag) 
	{ 
		infoLog().addf("creating: %s", tag.c_str()); 
		cpccLogSink::increaseIdent();
	}

    ~logObjectLife(void)  
	{ 
		cpccLogSink::decreaseIdent();
		infoLog().addf("destroying: %s", tag.c_str());
	}
};



class logTimeCountrer : public logObjectLife
{
private:
	cpccTimeCounter timer;
public:

	logTimeCountrer(const cpcc_char *aTag) : logObjectLife(aTag)
	{
		timer.resetTimer();
	}

	~logTimeCountrer(void)
	{
        tag.append(cpccLogSink::toString(". Duration:%.3f sec",timer.getSecondsElapsed()));
	}

};

