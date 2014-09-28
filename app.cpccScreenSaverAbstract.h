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
	//unsigned long int	mFramesElapsed;
	double				mTimeElapsed_inSec,
                        mPrev_TimeElapsed_inSec,
						mDeltaTime_inSec;
    logObjectLife       objLog;

protected:	// data
	cpccWindowBase*		DesktopWindowPtr;
	bool				clearBackgroundOnAnimate;
	
public: // constructor/destructor

	cpccScreenSaverAbstract():
			DesktopWindowPtr(NULL),
			clearBackgroundOnAnimate(true),    //todo: in windows must be true?
			//mFramesElapsed(0),
			mTimeElapsed_inSec(0.0),
            mPrev_TimeElapsed_inSec(0.0),
			mDeltaTime_inSec(0.0),
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
		
	void updateCounters(void)
	{
		// 1. update frame counter
		//mFramesElapsed++;

		// 2. update seconts counter
		mTimeElapsed_inSec = mSecondsTimer.getSecondsElapsed();

		// 3. update DT
		if (mPrev_TimeElapsed_inSec>0.0f)
			mDeltaTime_inSec = mTimeElapsed_inSec - mPrev_TimeElapsed_inSec;
		mPrev_TimeElapsed_inSec = mTimeElapsed_inSec;
	}


    // bit blit from buffer to screen
    void flushOneFrame(void)
    {	// appLog.addInfof("cpccScreenSaverAbstract.flushOneFrame() #%i", mFramesElapsed);
        if (DesktopWindowPtr)
            DesktopWindowPtr->flush();
        // appLog.addInfof("cpccScreenSaverAbstract.flushOneFrame() #%i exiting", mFramesElapsed);
    }
    
    
    void animateOneFrame(void)
    {	//appLog.addInfof("cpccScreenSaverAbstract.animateOneFrame() #%i", mFramesElapsed);
        updateCounters();
        //appLog.addInfof("cpccScreenSaverAbstract.animateOneFrame() #%i exiting", mFramesElapsed);
        animateOneFrameByDt((mDeltaTime_inSec==0)?0.04f:mDeltaTime_inSec);
    }
    
    
protected:  // abstract functions for the ancenstor to implement
    
    virtual void animateOneFrameByDt(const float dt)=0;
    
    
public: // screensaver standard functions	

    
	virtual void initWithWindowHandle(const cpccNativeWindowHandle wHandle, const int monitorId)
	{	infoLog().addf( _T("cpccScreenSaverAbstract.initWithWindowHandle():%X"), (cpccNativeWindowHandle) wHandle);
		if (!DesktopWindowPtr)
			DesktopWindowPtr = new cpccWindow(wHandle);
		
		infoLog().addf( _T("screen width: %i, height: %i"),  getWidth(), getHeight());
		mSecondsTimer.resetTimer();
	}


	virtual void eraseBackground(void)
	{	
		//infoLog().add("cpccScreenSaverAbstract.eraseBackground()");
		if (clearBackgroundOnAnimate) 
			if (DesktopWindowPtr)
				DesktopWindowPtr->clear();
		// appLog.addInfof("cpccScreenSaverAbstract.eraseBackground() #%i exiting", mFramesElapsed);
	}
    
	
	virtual void shutDown(void)
    { infoLog().add( "cpccScreenSaverAbstract::shutDown()"); }
    
	virtual bool hasConfigureSheet(void)=0;
	virtual void showConfigureSheet(void) { };

public:	// other functions

	//unsigned long int	getFramesElapsed(void)		{ return mFramesElapsed; }
	double				getDeltaTime_inSec(void)	{ return (mDeltaTime_inSec==0)?0.001f:mDeltaTime_inSec; }
	double				getSecondsElapsed(void)		{ return mTimeElapsed_inSec; }
	virtual int			getWidth(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getWidth() : 0;	}
	virtual int			getHeight(void)				{ return (DesktopWindowPtr) ? DesktopWindowPtr->getHeight() : 0; }


};



