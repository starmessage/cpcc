
/*  *****************************************
 *  File:		app.cpccAppMac.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <iostream>

#include <ApplicationServices/ApplicationServices.h>


//////////////////////////////////////////////////
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

#include <CoreFoundation/CoreFoundation.h>
// #include <NSSystemDirectories.h>
#include <crt_externs.h> // for _NSGetArgc(), _NSGetArgv()
// #include <Carbon/Carbon.h> // for the DialogRef

/*
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

//////////////////////////////////////////////////


#include "app.cpccAppBase.h"



class cpccAppMac: public cpccAppBase
{

private:
	pid_t	getProcessID(void) { return  getpid(); } ;

public:		

	virtual cpcc_string getCommandLine(void)
	{	
		/*
		 If you want to easily access the command line information from elsewhere 
		 in your program, you can use _NSGetArgv(), _NSGetArgc(), _NSGetEnviron(), 
		 and _NSGetProgname(). They're declared in crt_externs.h:
		 */
		
		int ret;
		char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
		
		pid_t pid = getProcessID();
		
		ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
		if ( ret <= 0 ) 
			{
				fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
				fprintf(stderr,	"    %s\n", strerror(errno));
			}

		return std::string(pathbuf);
	}

	
	
	virtual cpcc_string getAppDir(void)
	{
		// see also: 
		//  	int result = _NSGetExecutablePath(exepath, &bufsize);
		// 		proc_pidpath()
		// 		GetCurrentProcess
		ProcessSerialNumber psn;
		GetCurrentProcess(&psn);
		
		pid_t pid;
		GetProcessPID (&psn, &pid);
		
		char tempDir[5000];
		// uint32_t size = length;
		if (proc_pidpath(pid, tempDir, sizeof(tempDir) <= 0))
			std::cerr << "#5823: buffer for proc_pidpath too small\n";
		
		// delete up to the last trailing / 
		if (strrchr(tempDir,'/')) 
			*(strrchr(tempDir,'/') +1)=0;

		return std::string(tempDir);
	}

	


	
}; // end of class


	
