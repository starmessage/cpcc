/*  *****************************************
 *  File:		cpccScreenSaverInterface.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, library
 *				screensaver interface with windows and OSX
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include "gui.cpccNativeWindowHandle.h"

/**
	cpccScreenSaverInterfacePerMonitor
	This class is a separator bewteen the underlying operating system API for screensavers
		and your screensaver class
	It receives the screensaver related events from the OS.
	Configuration: if entering the configuration dialog of the screensaver, there is no need to 
		create the graphics (the world) of the screensaver.
		In other words, do not create the whole screensaver when this class is created.
	Multiple monitors: this class is created once for each monitor. The parameter monitorId
		indicates the number of the monitor: 0=primary, 1=second monitor, 
		-1=preview under the screensaver configuration dialog of the OS.
	
*/


class cpccScreenSaverInterface_App
{

};


class cpccScreenSaverInterface_PerMonitor
{
private:

protected:	// data

		
private:	// functions
		
	
protected:	// functions

	
public: // constructor/destructor

	
public: // screensaver interface functions: calls that the operating system dispatches to the screensaver	

	virtual void initWithWindowHandle(const NativeWindowHandle wHandle,
                                      const int monitorId /* -1=preview, 0=first monitor, 1=second monitor, etc */)=0;
	virtual void animateOneFrame(void)=0;
	virtual void drawOneFrame()=0;
	virtual void eraseBackground(void)=0;
	virtual void shutDown()		
	{  };
	
	virtual bool hasConfigureSheet(void)=0;
	virtual void showConfigureSheet(void) 
	{ };

public:	// other functions

	
};


#define DECLARE_SCREENSAVER_CLASS(x)
 



