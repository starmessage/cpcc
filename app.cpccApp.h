
/*  *****************************************
 *  File:		app.cpccApp.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "cpccUnicodeSupport.h"
#include "core.cpccStringUtil.h" 
#include "io.cpccLog.h"

#if defined(__APPLE__) 

	#include <err.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <libproc.h>
	#include <limits.h>
	#include <mach/mach.h>
	#include <paths.h>
	#include <pwd.h>
	#include <regex.h>
	#include <spawn.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <ApplicationServices/ApplicationServices.h>
	#include <CoreFoundation/CoreFoundation.h>
	#include <crt_externs.h> // for _NSGetArgc(), _NSGetArgv()

#elif defined(_WIN32) 


#else
	#error #4567: Unknown platform for cpccApp
#endif


 /*
 For MacOS
 Here's a few similar functions that may be interesting.

 $ nm /usr/lib/libSystem.B.dylib | grep _NSGet | grep ' T '
 9002a55f T _NSGetNextSearchPathEnumeration
 9003a382 T __NSGetArgc
 90020f60 T __NSGetArgv
 90003074 T __NSGetEnviron
 90029e2d T __NSGetMachExecuteHeader
 90027506 T __NSGetProgname
 9014aa84 T _NSGetSectionDataInObjectFileImage
 90036106 T __NSGetExecutablePath
 */
	
class cpccApp
{
public:

	static cpcc_string getCommandLine(void)
	{
	#ifdef _WIN32
		return ::GetCommandLine();
	
	#elif defined(__APPLE__)
		/*
		 If you want to easily access the command line information from elsewhere
		 in your program, you can use _NSGetArgv(), _NSGetArgc(), _NSGetEnviron(),
		 and _NSGetProgname(). They're declared in crt_externs.h:
		 */

		int ret;
		char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

		pid_t pid = getpid();

		ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));
		if (ret <= 0)
		{
			fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
			fprintf(stderr, "    %s\n", strerror(errno));
		}

		return pathbuf;

	#endif

	}

    // old. Use getAppFullPath()
    /*
	static cpcc_string getAppDir(void) 
	{
	#ifdef _WIN32
		cpcc_char tempDir[MAX_PATH];

		GetModuleFileName(NULL, tempDir, sizeof(tempDir) - 1);
		
		// #ifdef UNICODE
		//	WCHAR w_txt[MAX_PATH+1];
		//	GetModuleFileName(NULL,w_txt, sizeof(w_txt)-1);
		//	LPSTR pszTextA;
		//	UnicodeToAnsi(w_txt, &pszTextA);
		//	SafeStrCpy(tempDir, pszTextA, sizeof(tempDir)-1);
		//#else
		//	GetModuleFileName(NULL,tempDir, sizeof(tempDir)-1);
		//#endif
		

		// delete the part having the program.exe What remains is the path with a trailing \ 
		if (_tcsrchr(tempDir, '\\')) *(_tcsrchr(tempDir, '\\') + 1) = 0;

		return tempDir;

	#elif defined(__APPLE__)
		// see also: 
		//  	int result = _NSGetExecutablePath(exepath, &bufsize);
		// 		proc_pidpath()
		// 		GetCurrentProcess
		ProcessSerialNumber psn;
		GetCurrentProcess(&psn);

		pid_t pid;
		GetProcessPID(&psn, &pid);

		char tempDir[5000];
		// uint32_t size = length;
		if (proc_pidpath(pid, tempDir, sizeof(tempDir) <= 0))
			std::cerr << "#5823: buffer for proc_pidpath too small\n";

		// delete up to the last trailing / 
		if (strrchr(tempDir, '/'))
			*(strrchr(tempDir, '/') + 1) = 0;

		return std::string(tempDir);
	
	#endif

	}
     */
    
	static void	getArgcArgv(cpcc_stringList& argList)
	{
		cmdline2Vector(getCommandLine().c_str(), argList);
	}


	static void	argcArgv2Vector(int argc, cpcc_char** argvPtr, cpcc_stringList& argList)
	{
		argList.clear();
		for (int i = 0; i < argc; ++i)
			// argList.push_back( std::string(argv[i],strlen(argv[i])).dup() );
			argList.push_back(cpcc_string(argvPtr[i]));
	}


	static void				cmdline2Vector(const cpcc_char* aCmdLine, cpcc_stringList& argList)
	{
		argList.clear();
		cpcc_char* token,
			separators[] = _T(" \t"),
			cmdLine[4096];

		// copy to local string
		if (cpcc_strlen(aCmdLine) >= 4096)
			return;

		#ifdef _WIN32
			#pragma warning(disable : 4996)
		#endif
		cpcc_strcpy(cmdLine, aCmdLine);

		// first call	http://msdn.microsoft.com/en-us/library/vstudio/2c8d19sb.aspx
		token = cpcc_strtok(cmdLine, separators);
		while (token != NULL)
		{
			argList.push_back(token);
			token = cpcc_strtok(NULL, separators);
		}
	}


	void logInformation(void)
	{
		cpcc_string _commandLine(getCommandLine());
		infoLog().addf(_T("Application command line: %s"), _commandLine.c_str());
		cpcc_stringList args;
		getArgcArgv(args);
		for (unsigned int i = 0; i < args.size(); i++)
			infoLog().addf(_T("arg[%i]:%s"), i, args[i].c_str());
	}


	cpccApp(void) 
	{ 
		infoLog().addf( _T("creating cpccApp"));
		logInformation(); 
	}

	~cpccApp(void) 
	{ 
		infoLog().addf( _T("destroying cpccApp"));
	}
};
