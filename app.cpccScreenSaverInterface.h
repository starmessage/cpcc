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
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include "gui.cpccWindowBase.h"



/**
	cpccScreenSaverInterface
	This class is a separator bewteen the underlying operating system API for screensavers
		and your screensaver class
	It receives the screensaver related events from the OS.
	Configuration: if entering the configuration dialog of the screensaver, there is no need to 
		create the graphics (the sprites and the world) of the screensaver.
		In other words, do not create the whole screensaver when this class is created.
		You can create the graphics after the initWithWindowHandle() event
	Multiple monitors: this class is created once for each monitor. The parameter monitorId
		indicates the number of the monitor: 0=primary, 1=second monitor, 
		-1=preview under the screensaver configuration dialog of the OS.
	
*/



class cpccScreenSaverInterface
{
private:	// data
	
		
private:	// functions
		
	
protected:	// functions

	
public: // constructor/destructor
	cpccScreenSaverInterface()	{ 	}
    virtual ~cpccScreenSaverInterface() { }
		
public: // screensaver interface functions: calls that the operating system dispatches to the screensaver	

	/// initWithWindowHandle should not do any drawing. Allocate the screensaver here
	virtual void initWithWindowHandle(const cpccNativeWindowHandle wHandle,
                                      const int monitorId /* -1=preview, 0=first monitor, 1=second monitor, etc */)=0;
	
	virtual void animateOneFrame(void)=0;
	virtual void drawOneFrame(void)=0;
	virtual void flushOneFrame(void)=0;
	virtual void backgroundWasInvalidatedByOS(void) = 0;
    
	/// free the allocated screensaver
	virtual void shutDown()=0;
	
	virtual bool hasConfigureSheet(void)  { return false; }
	virtual void showConfigureSheet(void) { }

};


// Assuming the your custom class for the screensaver is 'mySsClass', 
// the following function 'createScreenSaver()' must be implemeted in your source code 
namespace cpccScreenSaverFactory
{
	cpccScreenSaverInterface* createScreenSaver(void);
}

// Example: in your mySsClass.cpp file, put:
// cpccScreenSaverInterface* createScreenSaver(void) { return new mySsClass; };

// You can also use the following macro to do the same job
#define DECLARE_SCREENSAVER_CLASS(T) namespace cpccScreenSaverFactory { cpccScreenSaverInterface* createScreenSaver(void) { return new T; }; }
// Example:
// DECLARE_SCREENSAVER_CLASS(mySsClass)

