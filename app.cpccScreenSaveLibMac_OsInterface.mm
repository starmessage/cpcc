
/*  *****************************************
 *  File:		app.cpccScreenSaveLibMac_OsInterface.mm
 *  Version:	see function getClassVersion()
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

#include	"io.cpccLog.h"
#include	"app.cpccScreenSaverInterface.h"
#import		<ScreenSaver/ScreenSaver.h>
#include 	"app.cpccApp.h"
#include 	"core.cpccOS.h"

/*
 Screensavers need to be compiled 32/64-bit with garbage collection supported or required 
 in the 64-bit architecture if they are to work on 64-bit systems. 
 */

/* multiple displays
 http://www.cocoabuilder.com/archive/cocoa/124621-screensaverview-with-multiple-displays.html
 I'm creating a screensaver, and I would like to be able to draw
 something different on displays other than the primary. Through trial
 and error, I've learned that initWithFrame() gets called for each
 display, although it doesn't seem like any useful information is
 passed into initWithFrame() or animateOneFrame() to help the developer
 know which display is being used.
 
 By using a counter in my ScreenSaverView and incrementing it each time
 initWithFrame() is called (where isPreview is false) I am able to
 count how many displays there are, and it seems like animateOneFrame()
 is called in order of display: animateOneFrame() for display 1,
 animateOneFrame() for display 2, and then back to 1 again, etc. So by
 using another counter, I can [sort of] keep track of to which display
 I'm drawing, though it doesn't seem like that great a way of doing
 this, nor does it seem that foolproof.
*/

// example
// http://cocoadevcentral.com/articles/000088.php
// OpenGL example
// http://cocoadevcentral.com/articles/000089.php



/*
 http://www.mactech.com/articles/mactech/Vol.20/20.06/ScreenSaversInCocoa/index.html
 
 Call Order
 When OS X starts your screen saver, the following things happen:
 
 1. The screen fades to black. 
 
 2. Your module is instantiated and its init routine is called. 
 
 3. The window is created. 
 
 4. Your module is installed in the window.
 
 5.	Your window is activated and its order is set. The drawRect call is called at this point.
 Note You should draw your initial state in the drawRect call.
 This initial state will be visible during the fade in that follows.
 
 6.	The screen fades in, revealing your window in the front. 
 
 7.	Your startAnimation method is called. 
 This method should not do any drawing.
 Note If you override this method,be sure to call the method in your super class.
 
 8.	Drawing continues. Your animateOneFrame is called repeatedly. 
 
 9.	The user takes some action. 
 
 10.	Your stopAnimation method is called.
 Note If you override this method,be sure to call the method in your superclass.
 Note The stopAnimation or startAnimation methods do not immediately start or stop animation. 
 In particular, it is not safe to assume that your animateOneFrame method will not execute 
 (or continue to execute) after you call stopAnimation.
 Do not attempt to use stopAnimation to gate against animation occurring while you modify 
 preferences or other state variables. It is not a mutex.
 */

 
 /*
	creation of the screensaver object in MACOSX
	You have to declare your screensaver class as NSPrincipalClass in the project
	so that when OSX knows which class to instanciate when it starts the screensaver
	The setting will be written like this in the info.plist file:
	<key>NSPrincipalClass</key>
	<string>TmioanScreenSaveLibMac_OsInterface</string>
 */
 
 
cpccApp	app;	


@interface TmioanScreenSaveLibMac_OsInterface : ScreenSaverView 
{	// ScreenSaverView class is a subclass of NSView 
}

/*

 
 Implementing preservesContentDuringLiveResize to return YES gives you some 
 extra responsibilities, but can improve performance during window resizing.
 
 10.6 added another two new APIs of this sort, layerContentsRedrawPolicy and layerContentsPlacement.
 
 */


@end

// this selects if drawing will be done in the function drawInDrawRect 
// otherwise it is done in the function animateOneFrame
#define     drawInDrawRect


@implementation TmioanScreenSaveLibMac_OsInterface


BOOL runningInPreview;
cpccScreenSaverInterface_PerMonitor *ssPtr=NULL;
bool    animationStarted=false,
        fadeoutEffectCompleted=false;



// flip the coordinates of the NSView, so they are the same as in the case of windows screensavers
- (BOOL)isFlipped  { return YES; }

/*  If your subclass of NSView implements the isOpaque method to return YES,
    then the framework will never clear anything behind your view or draw any of the views under it.
    YES = this keeps Cocoa from unneccessarily redrawing our superview */
- (BOOL)isOpaque    { return YES; }


/*  This class method allows to select how the desktop visibly transitions to the screen saver view.
    If this method returns YES (default), the screen will gradually darken before the animation begins.
    If it returns NO, the transition will be immediate.
    'No' is more appropriate if the screen saver animates a screen shot of the desktop, as is the case
    for optical lens effects.  */
+ (BOOL)performGammaFade { return NO; }


- (cpccNativeWindowHandle)getNativeWindowHandle
{
	return self; // this returns an NSView
	// return [self window]; // this returns an NSWindow
}


- (void)createScreensaver
{
	assert(ssPtr==NULL && "#4813: createScreensaver already called?");
	//TmioanUtils::DebugLogLn("TmioanScreenSaveLibMac_OsInterface BEFORE new ClassOfScreensaver");
	ssPtr = createScreenSaver();
	assert(ssPtr && "Error 4567: ssPtr = nil");
}


- (void)initScreensaverWithWindowHandle
{
    cpccNativeWindowHandle windowHandle = [self getNativeWindowHandle];
    assert(windowHandle && "Error 2354b: could not get native window handle");
    ssPtr->initWithWindowHandle( windowHandle, runningInPreview);
}


- (void)doOnceTheFadeoutEffect
{
    //[self lockFocus];
    if (!fadeoutEffectCompleted)
        if (ssPtr)
        {
            ssPtr->fadeoutUsersDesktop();
            fadeoutEffectCompleted = true;
        }
    //[self unlockFocus];
}


- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
	infoLog().add( "TmioanScreenSaveLibMac_OsInterface initWithFrame");
    //cpccOS::sleep(2000);
    
	runningInPreview = isPreview;
	
    self = [super initWithFrame:frame isPreview:isPreview];
	assert(self && "#9572: 'super initWithFrame:frame isPreview:isPreview' has FAILED");
	
    return self;
}


- (void)startAnimation
{
	// This method should not do any drawing.
	// You must at least call [super stopAnimation] as shown in the standard template.
    
	infoLog().add( "TmioanScreenSaveLibMac_OsInterface startAnimation");
    //cpccOS::sleep(2000);
    
    // semi transparent window
    //if (![self isPreview])
    //    [[self window] setAlphaValue:0.7];
    
    /*
    // demo
    NSRect tmpRect = NSMakeRect(30, 40, 100, 150);
    [[NSColor redColor] set];
    NSRectFill(tmpRect);
    cpccOS::sleep(2000);
     */
    
    
    [[self window] setBackgroundColor:[NSColor clearColor]];
    
	// Activates the periodic timer that animates the screen saver.
    // A zero value polls as fast as possible while a negative number turns animation off.
	[self setAnimationTimeInterval:1/25.0]; // 25 frames/sec
    [self createScreensaver];
    [self initScreensaverWithWindowHandle];
	[self doOnceTheFadeoutEffect];
    
    [super startAnimation];
    animationStarted = true;
    
    infoLog().add( "TmioanScreenSaveLibMac_OsInterface startAnimation exiting");
    //cpccOS::sleep(2000);
}


- (void)stopAnimation
{
    // You must at least call [super stopAnimation] as shown in the standard template.
    // stopAnimation, it will get called if the display goes to sleep.
	infoLog().add( "TmioanScreenSaveLibMac_OsInterface stopAnimation");
	
	/*
	 Deactivates the timer that advances the animation.
	When Mac OS X wants your screen saver to stop doing its thing, it calls stopAnimation. 
	 You can override stopAnimation to release resources or 
	 do any other cleanup you want before your screen saver goes away. 
	 */
	
    [super stopAnimation];
}


- (void)drawRect:(NSRect)rect
{
    // infoLog().add("drawRect()");
	/*
     ScreenSaverView implements drawRect: to draw a black background. 
     Subclasses can do their drawing here or in animateOneFrame.
     
	There are two main ways to do drawing in a screen saver. 
	 You can either do your drawing in the normal NSView drawRect: method,
	 or you can do your drawing in ScreenSaverView’s animateOneFrame method.
	 If you do drawing in drawRect: , you should call setNeedsDisplay: with
	 an argument of YES in animateOneFrame.
	 
     By keeping all the drawing code in drawRect: you ensure it is always drawn when the Cocoa system needs it drawn.
     By keeping all of your computational logic out of the draw path, you ensure multiple-redraws don't cause unwanted side-effects in your animation.
     */
    
    
    if (!animationStarted)
        return;
    
    //infoLog().add( "TmioanScreenSaveLibMac_OsInterface drawRect");
    //cpccOS::sleep(1000);
    
    [super drawRect:rect];      // this call also draws a black background
    
    // std::cout << "drawRect. w:" << rect.size.width << " h:" << rect.size.height << "\n";
#ifdef drawInDrawRect
	if (ssPtr)
		{
		//TmioanUtils::DebugLogLn("before ssPtr->drawOneFrame()");
		ssPtr->drawOneFrame();
		ssPtr->flushOneFrame();
		}
#endif
}



- (void) animateOneFrame {
	/*
	 This method is called each time the timer animating the screen saver fires. 
	 The time between calls to this method is always at least animationTimeInterval.
	 It is guaranteed that the focus is locked when this method is called, 
	 so subclasses may do drawing in this method. 
	 The subclass can also let drawRect: perform the drawing, in which 
	 case animateOneFrame needs to call setNeedsDisplay: with an argument of YES. 
	 The default implementation does nothing.
	 */
    //infoLog().add( "TmioanScreenSaveLibMac_OsInterface animateOneFrame");
    //cpccOS::sleep(1000);
    

		
	if (ssPtr)
		{
		//NSDisableScreenUpdates();
		
		ssPtr->animateOneFrame();
			
#ifndef drawInDrawRect
		ssPtr->drawOneFrame();
		ssPtr->flushOneFrame();
#else
        [self setNeedsDisplay:YES];
#endif
            
		//NSEnableScreenUpdates();
		}
	
	
}


-(void)dealloc 
{
	/* 
	 -dealloc is called when the screen saver was instantiated by SaverLab or System Preferences; 
	 -dealloc is not called when ScreenSaverEngine runs the saver.
	 */
	if (ssPtr)
		{
		infoLog().add( "TmioanScreenSaveLibMac_OsInterface dealloc");
		ssPtr->shutDown();
		delete ssPtr;
		ssPtr = NULL;
		}
	
	[super dealloc];
}


- (BOOL)hasConfigureSheet
{	
	infoLog().add(  "TmioanScreenSaveLibMac_OsInterface hasConfigureSheet()");
	return false;
	/*
	bool result;
	TmioanScreenSaverAbstract *tmpSsPtr;
	tmpSsPtr = new ClassOfScreensaver;
	
	result = tmpSsPtr->hasConfigureSheet();
	delete tmpSsPtr;
    return result;
	 */
}


- (NSWindow*)configureSheet
{
	infoLog().add( "TmioanScreenSaveLibMac_OsInterface configureSheet()");
    // ToDo: a code like the following example must be put here
	/*
	TmioanScreenSaverAbstract *tmpSsPtr;
	tmpSsPtr = new ClassOfScreensaver;
	
	if (tmpSsPtr->hasConfigureSheet())
		tmpSsPtr->showConfigureSheet();
	
	delete tmpSsPtr;
    */
	 return nil;
}



@end
