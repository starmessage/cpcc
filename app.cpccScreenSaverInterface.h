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
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once


#if defined(__APPLE__)
	#include <Cocoa/Cocoa.h>
#elif defined(_WIN32)
	#include <Windows.h>
#else
	#error Unknown platform for cpccScreenSaverInterface
#endif



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
		
public: // screensaver interface functions: calls that the operating system dispatches to the screensaver
    
    virtual ~cpccScreenSaverInterface() { }
    
	/// initWithWindowHandle should not do any drawing. Allocate the screensaver here
	/// monitodID: -1=preview, 0=first monitor, 1=second monitor, etc
#ifdef _WIN32
	virtual void initWithWindowHandle(HWND wHandle, const int monitorId ) = 0;
	virtual void showConfigureSheet(HWND wHandleOwner) = 0;
#endif
#ifdef __APPLE__
	virtual void initWithWindowHandle(NSView* wHandle, const int monitorId) = 0;
	// virtual void showConfigureSheet(NSView* wOwnerHandle) =0;
    virtual NSWindow* showConfigureSheet(NSView* wOwnerHandle) =0;

#endif

	virtual void animateOneFrame(void)=0;
	virtual void drawOneFrame(void)=0;
	virtual void flushOneFrame(void)=0;
	virtual void backgroundWasInvalidatedByOS(void) = 0;
	virtual bool getPreserveDeskopContents() const = 0;
	virtual void setPreserveDeskopContents(const bool a) = 0;

	/// free the allocated resources
	virtual void shutDown()=0;
	virtual bool hasConfigureSheet(void) = 0;
};


// Assuming the your custom class for the screensaver is 'mySsClass', 
// the following function 'createScreenSaver()' must be implemeted in your source code 
namespace cpccScreenSaverFactory
{
	cpccScreenSaverInterface* createScreenSaver(void);
}

// Example: in your mySsClass.cpp file, put:
// cpccScreenSaverInterface* createScreenSaver(void) { return new mySsClass; };
//      OR
// You can use the following macro to do the same job
#define DECLARE_SCREENSAVER_CLASS(T) namespace cpccScreenSaverFactory { cpccScreenSaverInterface* createScreenSaver(void) { return new T; }; }
// Example:
// DECLARE_SCREENSAVER_CLASS(mySsClass)

