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
 *  Download:	https://github.com/starmessage/cpcc
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
	
	cpccWindowBase*		DesktopWindowPtr;
	// bool				m_PreserveDeskopContents;
    cpcc_string			m_containerFolder;
	
protected: // constructor/destructor

	cpccScreenSaverAbstract():
			DesktopWindowPtr(NULL),
			// m_PreserveDeskopContents(false),  // opaque by default
            objLog((cpcc_char *) _T("cpccScreenSaverAbstract"))
	{
        infoLog().addf(_T("Program: %s %s"), cpccAppInfo::ProgramName, cpccAppInfo::Version);
		reportComputerMonitors();
	}


	virtual ~cpccScreenSaverAbstract()
	{
		if (DesktopWindowPtr)
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
    {
        m_containerFolder = aFolder;
        infoLog().addf("screensaver's container folder:%s", aFolder);
    }
    

	void reportComputerMonitors(void)
	{
		cpccMonitorList		_monitorList;
		unsigned long n = cpccOS::getListOfMonitors(_monitorList);

		// report on monitors found
		infoLog().addf(_T("Number of monitors:%i"), n);
		for (unsigned long i = 0; i < n; ++i)
			infoLog().addf(_T("Monitor %i: Left %i, top %i, right %i, bottom %i"), i, _monitorList[i].left, _monitorList[i].top, _monitorList[i].right, _monitorList[i].bottom);
	}


    // todo: rename: initWithWindowHandleAndRect
	virtual void initWithWindowHandle( cpccNativeWindowHandle wHandle, const int monitorId) override
	{
        infoLog().addf( _T("cpccScreenSaverAbstract.initWithWindowHandle(%X)"), (cpccNativeWindowHandle) wHandle);
		if (!DesktopWindowPtr)
			DesktopWindowPtr = new cpccWindow(wHandle);
		
		infoLog().addf( _T("TopLeft:%i,%i screen width:%i, height:%i"), DesktopWindowPtr->getTop(), DesktopWindowPtr->getLeft(), getWidth(), getHeight());
		infoLog().addf(_T("Screensaver container folder:%s"), m_containerFolder.c_str());
	}

    virtual void shutDown(void) override   {  }

    
public:	// other functions

	// inline double	getSecondsElapsed(void)		{ return mTimeElapsed_inSec; }
	// todo: kapoios ta xrisimopoiei auta ? Isos de tha eprepe giati mporei na yparxei parathyro poy ksekinaei apo to 10,10
	virtual int		getWidth(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getWidth() : 0;	}
	virtual int		getHeight(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getHeight() : 0; }
    
};



