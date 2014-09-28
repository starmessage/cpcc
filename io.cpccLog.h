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
#include <typeinfo>     // to find automatically the class name, eg.    cout << typeid(*this).name() << endl;

#include "cpccUnicodeSupport.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"


class cpccLogFile
{
private:
	const cpcc_char *m_tag;
	cpcc_string		 m_filename;
	bool  			 m_isEmpty,
					 m_disableIfFileDoesNotExist,
					 m_echoToConsole;
	
public: // constructor / destructor
	cpccLogFile(const cpcc_char *aTag, const cpcc_char *aFilename, const bool disableIfFileDoesNotExist, const bool echoToConsole):
		m_filename(aFilename),
		m_tag(aTag),
		m_disableIfFileDoesNotExist(disableIfFileDoesNotExist),
		m_echoToConsole(echoToConsole),
		m_isEmpty(true)
 	{ }
	
    
public: // functions
	bool isEmpty(void) { return m_isEmpty; }
	
	void add(const cpcc_char* txt)
	{	
		if (m_filename.length()==0)
			return;
		
		cpccFileSystemMini fs;
		if (!fs.fileExists(m_filename) && (m_disableIfFileDoesNotExist) ) 
			return;
			
		cpcc_string output = getCurrentTime( _T("%X") );
		output.append( " " );
		output.append( m_tag );
		output.append( txt );
		output.append( "\n" );
		fs.fileAppend(m_filename, output);
		if (m_echoToConsole)
		{
			std::cout << output;
			#if defined(_WIN32)
				OutputDebugString(output.c_str());		
			#endif
		}
		m_isEmpty = false;
	}
	
	
	void addf(const cpcc_char* format, va_list args)
	{
		const int MAX_LOG_STRING=8000;
		cpcc_char buff[MAX_LOG_STRING];
		#if (_MSC_VER >= 1400) // Visual Studio 2005
			// vsprintf_s( buff, MAX_LOG_STRING, format, args);
			_vstprintf_s( buff, MAX_LOG_STRING, format, args);
		#else
			vsprintf(buff, format, args);
		#endif
		add(buff);
	}
	
    
	void addf(const cpcc_char* format, ...)
	{ 
		va_list args;
		va_start(args, format);
		addf(format, args);
		va_end(args);
	}

	///  Get the current datetime as a human readable string
	/**
		This function is used to create the timestamp field of the logRecord
	 
		 @param  fmt the format specifier according to C++ strftime()
		 @return the current datetime formatted with the standard function strftime
	*/
	static cpcc_string getCurrentTime(const cpcc_char * fmt)
	{
		time_t t = time(0);   // get time now
		#pragma warning( disable : 4996 )
		struct tm timeStruct = *localtime( & t );
		
		cpcc_char buffer[100];
		
		// More information about date/time format
		// http://www.cplusplus.com/reference/clibrary/ctime/strftime/
		// strftime(buffer, sizeof(buffer), fmt, &timeStruct);
		cpcc_strftime(buffer, sizeof(buffer), fmt, &timeStruct);
		return cpcc_string(buffer);  // e.g. 13:44:04
	}
};




// lazy but early enough constructor for the logging object

cpccLogFile			&infoLog(void);
cpccLogFile			&warningLog(void);
cpccLogFile			&errorLog(void);


/////////////////////////////////////////////////////////////////////////
// this class is used to automatically log creation/desctuction messages
// when the container object is created / destructed

class logObjectLife
{
private:
    std::string tag;
public:
    logObjectLife(char *aTag): tag(aTag) { infoLog().addf("creating: %s", tag.c_str() ); }
    ~logObjectLife(void)                 { infoLog().addf("destroying: %s", tag.c_str());}
    
};
