
/*  *****************************************
 *  File:		app.cpccAppWin.h
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

#include "app.cpccAppBase.h"



class cpccAppWin: public cpccAppBase
{

public:		// abstract functions to be implemented by Windows or OSX specific code

	virtual cpcc_string getCommandLine(void)
	{
		return cpcc_string(::GetCommandLine());
	}
	
	
	virtual cpcc_string getAppDir(void)
	{
		cpcc_char tempDir[MAX_PATH];

		GetModuleFileName(NULL,tempDir, sizeof(tempDir)-1);
		/*
		#ifdef UNICODE
			WCHAR w_txt[MAX_PATH+1];
			GetModuleFileName(NULL,w_txt, sizeof(w_txt)-1);
			LPSTR pszTextA;
			UnicodeToAnsi(w_txt, &pszTextA);
			SafeStrCpy(tempDir, pszTextA, sizeof(tempDir)-1);
		#else
			GetModuleFileName(NULL,tempDir, sizeof(tempDir)-1);
		#endif
		*/

		// delete the part having the program.exe what remains is the path with a trailing \ 
		if (_tcsrchr(tempDir,'\\')) *(_tcsrchr(tempDir,'\\') +1)=0;

		return cpcc_string(tempDir);
	}

	


	
}; // end of class


	
