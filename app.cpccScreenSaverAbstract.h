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


#include "cpccTimeCounter.h"
#include "app.cpccScreenSaverInterface.h"
#include "gui.cpccWindow.h"


class cpccScreenSaverAbstract: public cpccScreenSaverInterface_PerMonitor
{
private:
	cpccTimeCounter		mSecondsTimer;
	double				mTimeElapsed_inSec;
    logObjectLife       objLog;

protected:	// data
	cpccWindowBase*		DesktopWindowPtr;
	
public: // constructor/destructor

	cpccScreenSaverAbstract():
			DesktopWindowPtr(NULL),
			mTimeElapsed_inSec(0.0),
            objLog((char *) "cpccScreenSaverAbstract")
	{  }


	virtual ~cpccScreenSaverAbstract()
	{
		if (DesktopWindowPtr)
		{
			delete(DesktopWindowPtr);		
			DesktopWindowPtr=NULL;
		}
	}
	
private:	// functions
		
    
    void animateOneFrame(void)
    {
        //appLog.addInfof("cpccScreenSaverAbstract.animateOneFrame() #%i", mFramesElapsed);
        static double mPrev_TimeElapsed_inSec=0.0;
        
        // 1. update seconts counter
        double mTimeElapsed_inSec = mSecondsTimer.getSecondsElapsed();

        // 2. update DT
        float mDeltaTime_inSec = (mPrev_TimeElapsed_inSec>0.0 ? (float)(mTimeElapsed_inSec - mPrev_TimeElapsed_inSec) : 0.04f);
    
        // 3. store the new time
        mPrev_TimeElapsed_inSec = mTimeElapsed_inSec;
        
        // 4. call the animation now that I know the dt
        animateOneFrameByDt(mDeltaTime_inSec);
    }
    

    // bit blit from buffer to screen
    void flushOneFrame(void)
    {	// appLog.addInfof("cpccScreenSaverAbstract.flushOneFrame() #%i", mFramesElapsed);
        if (DesktopWindowPtr)
            DesktopWindowPtr->flush();
        // appLog.addInfof("cpccScreenSaverAbstract.flushOneFrame() #%i exiting", mFramesElapsed);
    }
    
    
protected:  // abstract functions for the ancenstor to implement
    
    virtual void animateOneFrameByDt(const float dt)=0;
    
public: // screensaver standard functions	

    
	virtual void initWithWindowHandle(const cpccNativeWindowHandle wHandle, const int monitorId)
	{
        infoLog().addf( _T("cpccScreenSaverAbstract.initWithWindowHandle(%X)"), (cpccNativeWindowHandle) wHandle);
		if (!DesktopWindowPtr)
			DesktopWindowPtr = new cpccWindow(wHandle);
		
		infoLog().addf( _T("screen width: %i, height: %i"),  getWidth(), getHeight());
		mSecondsTimer.resetTimer();
	}

    
	virtual void shutDown(void)    { infoLog().add( "cpccScreenSaverAbstract::shutDown()"); }
    
	virtual bool hasConfigureSheet(void)=0;
	virtual void showConfigureSheet(void) { };

public:	// other functions

	inline double	getSecondsElapsed(void)		{ return mTimeElapsed_inSec; }
	virtual int		getWidth(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getWidth() : 0;	}
	virtual int		getHeight(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getHeight() : 0; }


};


