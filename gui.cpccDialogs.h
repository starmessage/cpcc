
/*  *****************************************
 *  File:		gui.cpccDialogs.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, c++ library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once

#if defined(__APPLE__) 
	
#elif defined(_WIN32) 
	#include <windows.h>
	
#else
	#error #4367h: Unknown platform for cpccDialogs
#endif


class cpccDialogs
{
public:
	static void showMessage(const char *aTitle, const char *aMessage)
	{
	#if defined(__APPLE__) 
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"OK"];
        NSString *titleText = [NSString stringWithCString:aTitle encoding:NSASCIIStringEncoding];
        [alert setMessageText:titleText];
			
        NSString *infoText = [NSString stringWithCString:aMessage encoding:NSASCIIStringEncoding];
        [alert setInformativeText:infoText];
			
        [alert setAlertStyle:NSWarningAlertStyle];
        [alert runModal];
        [alert release];
	#elif defined(_WIN32) 
		MessageBox(0, aMessage, aTitle, MB_OK);
	#endif
	}
};


