/*  *****************************************
 *  File:		app.cpccScreenSaverAbstract.h
  *	Purpose:	Portable (cross-platform), light-weight, library
 *				screensaver interface with windows and OSX
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
#include "app.cpccScreenSaverInterface.h"
#include "gui.cpccWindow.h"
#include "core.cpccOS.h"
#include "app.cpccAppInfo.h"

class cpccScreenSaverAbstract: public cpccScreenSaverInterface
{
private:
    logObjectLife       objLog;
	
protected:	// data
	cpccMonitorList		m_monitorList;
	//time_t				m_ticksPassed = 0;
	cpccWindowBase*		DesktopWindowPtr;
	bool				m_windowIsOwned;
    bool				m_PreserveDeskopContents;
    cpcc_string			m_containerFolder;
	
protected: // constructor/destructor

	cpccScreenSaverAbstract():
			DesktopWindowPtr(NULL),
			m_windowIsOwned(true),
            m_PreserveDeskopContents(false),  // opaque by default
            objLog((cpcc_char *) _T("cpccScreenSaverAbstract"))
	{
        infoLog().addf(_T("Program: %s %s"), cpccAppInfo::ProgramName, cpccAppInfo::Version);
	}


	virtual ~cpccScreenSaverAbstract()
	{
		if (DesktopWindowPtr && (m_windowIsOwned))
		{
			delete(DesktopWindowPtr);		
			DesktopWindowPtr=NULL;
		}
	}
	
private:	// functions
	    
    // bit blit from buffer to screen
    virtual void flushOneFrame(void) override
    {	// appLog.addInfof("cpccScreenSaverAbstract.flushOneFrame() #%i", mFramesElapsed);
        if (DesktopWindowPtr)
            DesktopWindowPtr->flush();
        // appLog.addInfof("cpccScreenSaverAbstract.flushOneFrame() #%i exiting", mFramesElapsed);
    }
    
    
    
protected: // screensaver standard functions
    
    virtual void setContainerFolder(const cpcc_char *aFolder) override
    {   m_containerFolder = aFolder;   }
    
	virtual void initWithWindowHandle( cpccNativeWindowHandle wHandle, const int monitorId) override
	{
        infoLog().addf( _T("cpccScreenSaverAbstract.initWithWindowHandle(%X)"), (cpccNativeWindowHandle) wHandle);
		if (!DesktopWindowPtr)
			DesktopWindowPtr = new cpccWindow(wHandle);
		
		infoLog().addf( _T("TopLeft:%i,%i screen width:%i, height:%i"), DesktopWindowPtr->getTop(), DesktopWindowPtr->getLeft(), getWidth(), getHeight());
		
		unsigned long n = cpccOS::getListOfMonitors(m_monitorList);

		// report on monitors found
		infoLog().addf(_T("Number of monitors:%i"), n);
		for (unsigned long i = 0; i < n; ++i)
			infoLog().addf(_T("Monitor %i: Left %i, top %i, right %i, bottom %i"), i, m_monitorList[i].left, m_monitorList[i].top, m_monitorList[i].right, m_monitorList[i].bottom);

		infoLog().addf(_T("Screensaver container folder:%s"), m_containerFolder.c_str());
		// mSecondsTimer.resetTimer();
		m_windowIsOwned = true;
	}

    virtual void shutDown(void) override   {  }

    
public:	// other functions

	// inline double	getSecondsElapsed(void)		{ return mTimeElapsed_inSec; }
	virtual int		getWidth(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getWidth() : 0;	}
	virtual int		getHeight(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getHeight() : 0; }
    virtual bool    getPreserveDeskopContents() const   override   { return m_PreserveDeskopContents; }
    virtual void    setPreserveDeskopContents(const bool a)  override { m_PreserveDeskopContents = a; }

};



