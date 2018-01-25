/*  *****************************************
 *  File:		io.cpccLogFileWriterWithBuffer.h
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to write application information to a log file
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2018 StarMessage software.
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

#include "cpccUnicodeSupport.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccPathHelper.h"
#include "core.cpccIdeMacros.h"
#include "core.cpccTryAndCatch.h"


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
class  cpccLogFileWriterWithBuffer
{
private:
	cpcc_string m_filename;
	cpcc_stringstream m_buffer;
	int maxBufferedLines;
	
private:
	/* 
	Always declare a copy constructor and assignment operator
	Many classes shouldn't be copied or assigned. Then, to enforce your policy declare a deleted copy constructor as private and 
	not supply a definition. 
	Do the same for the assignment operator.

	*/
	cpccLogFileWriterWithBuffer(const cpccLogFileWriterWithBuffer& x) = delete;
	cpccLogFileWriterWithBuffer& operator=(const cpccLogFileWriterWithBuffer& x) = delete;

	explicit cpccLogFileWriterWithBuffer(): maxBufferedLines(200), m_filename(_T("")) 	{  add( _T("cpccLogFileWriterWithBuffer starting.\n")); }

public:
	virtual ~cpccLogFileWriterWithBuffer()  { 	add( _T("cpccLogFileWriterWithBuffer closing.\n")); }
	
public:

	static cpccLogFileWriterWithBuffer &getInstance(void)
	{
		// static cpccLogFileWriterWithBuffer _inst;
		static cpccLogFileWriterWithBuffer* _instPtr=NULL;
		if (!_instPtr)
			_instPtr = new cpccLogFileWriterWithBuffer;
		return *_instPtr;
	}

	void setFilename(const cpcc_char* aFilename) 
	{ 
		assert((m_filename.length() == 0) && "#4721: cpccLogFileWriterWithBuffer.setFilename() already called.");
		if (aFilename)
			m_filename = aFilename;
		else
			return;

		if (cpccFileSystemMini::fileExists(m_filename.c_str())) // empty the file if it exists already
			cpccFileSystemMini::createEmptyFile(m_filename.c_str());
			
		add(m_buffer.str().c_str());
	}

	const cpcc_string &getFilename(void) const { return m_filename; }


	void add(const cpcc_char* txt) 
	{
		if (m_filename.length() > 0)
		{
			if (cpccFileSystemMini::fileExists(m_filename.c_str()))
            {
                try
                {
                    cpccFileSystemMini::appendTextFile(m_filename.c_str(), txt);
                }
                catch ( ... )
                {
                    std::cerr << "Exception in cpccLogFileWriterWithBuffer.add(""" << txt << """)" << std::endl;
                    throw std::runtime_error("Exception #6246 in cpccLogFileWriterWithBuffer.add() when calling cpccFileSystemMini::appendTextFile()");
                }
            }
		}
		else
		{
			if (maxBufferedLines>0)
			{
				m_buffer << txt;
				--maxBufferedLines;
			}
		}
	}

};

