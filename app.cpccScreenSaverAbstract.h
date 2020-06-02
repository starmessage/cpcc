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
#include <vector>

#include "app.cpccScreenSaverInterface.h"
#include "gui.cpccWindow.h"
#include "core.cpccHardware.h"



class cpccScreenSaverAbstract: public cpccScreenSaverInterface
{
private:
    logObjectLife       objLog;
	cpcc_string			m_containerFolder;

protected:	// data
	
    
	cpccWindowBase*		    DesktopWindowPtr;
	// bool				    m_PreserveDeskopContents;
    std::vector<cpccRecti>  m_drawRegions;
	
protected: // constructor/destructor

	cpccScreenSaverAbstract():
			DesktopWindowPtr(NULL),
			// m_PreserveDeskopContents(false),  // opaque by default
            objLog((cpcc_char *) _T("cpccScreenSaverAbstract"))
	{
        // infoLog().addf(_T("Program: %s %s"), cpccAppInfo::ProgramName, cpccAppInfo::Version);
		
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
		if (!aFolder)
		{
			m_containerFolder = _T("");
			errorLog().add("screensaver's container folder:NULL");
			return;
		}
			
		m_containerFolder = aFolder;
		infoLog().addf("screensaver's container folder:%s", aFolder);
    }


	const cpcc_char* getContainerFolder(void) const
	{
		return m_containerFolder.c_str();
	}

    
	void calcDrawRegionsAndOffset(const int monitorID)
	{
        #ifdef _WIN32
            if (monitorID == -1)
            {
                // DesktopWindowPtr->setOffset(-DesktopWindowPtr->getLeft(), -DesktopWindowPtr->getTop());

                cpccRecti rect = DesktopWindowPtr->getBounds();
                rect.left = 0;
                rect.top = 0;
                m_drawRegions.push_back(rect);
                return;
            }

            if (monitorID != -1)
            {
                cpccMonitorList		list;
                cpccHardware::getListOfMonitors(list);
                if (list.size() > 0)
                {
                    // report on monitors found
                    infoLog().addf(_T("Number of monitors for the drawRegions:%i"), list.size());
                    int min_left = 0, min_top = 0, i = 0;
                    for (const auto& monitor : list)
                    {
                        cpccRecti rect(monitor.left, monitor.top, monitor.right, monitor.bottom);
                        infoLog().addf(_T("Monitor rect:%s"), rect.asString().c_str());
                        m_drawRegions.push_back(rect);
                        if (i == 0) // first iteration
                        {   // put initial values
                            min_left = monitor.left;
                            min_top = monitor.top;
                        }
                        else
                        {
                            if (min_left > monitor.left)
                                min_left = monitor.left;
                            if (min_top > monitor.top)
                                min_top = monitor.top;
                        }
                        
                        ++i;
                    }
                    // remove the minimum
                    if (min_left != 0)
                        for (auto& r : m_drawRegions)
                            r.left -= min_left;
                    if (min_top != 0)
                        for (auto& r : m_drawRegions)
                            r.top -= min_top;

                    // infoLog().addf(_T("setOffset x=%i, y=%i"), -min_left, -min_top);
                    // DesktopWindowPtr->setOffset(-min_left, -min_top);
                    return;
                }
            }
            
        #endif
        m_drawRegions.push_back(DesktopWindowPtr->getBounds());
        
	}


    // todo: rename: initWithWindowHandleAndRect
	virtual void initWithWindowHandle( cpccNativeWindowHandle wHandle, const int monitorId) override
	{
        infoLog().addf( _T("cpccScreenSaverAbstract.initWithWindowHandle(%X)"), (cpccNativeWindowHandle) wHandle);
		if (!DesktopWindowPtr)
			DesktopWindowPtr = new cpccWindow(wHandle);
		
		// infoLog().addf( _T("TopLeft:%i,%i screen width:%i, height:%i"), DesktopWindowPtr->getTop(), DesktopWindowPtr->getLeft(), DesktopWindowPtr->getWidth(), DesktopWindowPtr->getHeight());
		infoLog().addf(_T("Screensaver container folder:%s"), getContainerFolder());
        calcDrawRegionsAndOffset(monitorId);
	}

    virtual void shutDown(void) override   {  }

};



