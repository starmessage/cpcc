
/*  *****************************************
 *  File:		app.cpccAppBase.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <vector>
#include "cpccUnicodeSupport.h"
#include "core.cpccStringUtil.h" 
#include "io.cpccLog.h"




class cpccAppBase
{

public:		// abstract functions to be implemented by Windows or OSX specific code
    virtual ~cpccAppBase() { }
    
    virtual cpcc_string getCommandLine(void)=0;
	virtual cpcc_string getAppDir(void)=0;

	void logInformation(void)
	{
        cpcc_string _commandLine(getCommandLine());
		infoLog().addf( _T("Application command line: %s"), _commandLine.c_str());
		cpcc_stringList args;
		getArgcArgv(args);
		for (unsigned int i=0; i<args.size(); i++)
			infoLog().addf( _T("arg[%i]:%s") , i , args[i].c_str());
	}

public:		// platform independent functions 


	void				getArgcArgv(cpcc_stringList &argList)
	{
		cmdline2Vector(getCommandLine().c_str(), argList);
	}

	
protected:	// platform independent functions 


	static void				argcArgv2Vector(int argc, cpcc_char **argvPtr, cpcc_stringList &argList)
	{
		argList.clear();
		for(int i=0 ; i<argc ; ++i)
			// argList.push_back( std::string(argv[i],strlen(argv[i])).dup() );
			argList.push_back( cpcc_string(argvPtr[i]) );
	}
	

	static void				cmdline2Vector(const cpcc_char *aCmdLine, cpcc_stringList &argList)
	{
		argList.clear();
		cpcc_char	*token, 
					separators[]=_T(" \t"),
					cmdLine[4096];

		// copy to local string
		if (cpcc_strlen(aCmdLine)>=4096)
			return;
		#ifdef _WIN32
			#pragma warning(disable : 4996)
		#endif
		cpcc_strcpy(cmdLine,aCmdLine);

		// first call	http://msdn.microsoft.com/en-us/library/vstudio/2c8d19sb.aspx
		token = cpcc_strtok( cmdLine,  separators);
		while (token != NULL)
		{
			argList.push_back( token );
			token = cpcc_strtok( NULL,  separators);
		}
	}
	
}; // end of class


	
