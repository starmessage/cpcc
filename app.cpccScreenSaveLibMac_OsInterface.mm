
/*  *****************************************
 *  File:		app.cpccScreenSaveLibMac_OsInterface.mm
 *  Version:	see function getClassVersion()
 *	Purpose:    Portable (cross-platform), light-weight, library
 *				screensaver interface with windows and OSX
 *	*****************************************
 *  Library:    Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include    "io.cpccLog.h"
#include    "app.cpccAppInfo.h"
#include    "core.cpccTryAndCatch.h"
#include    "app.cpccScreenSaverInterface.h"
#import     <ScreenSaver/ScreenSaver.h>


////////////////////////////////
// configuration parameters
////////////////////////////////
// this selects if drawing will be done inside the function drawInDrawRect.
// If not defined it is done inside the function animateOneFrame
//  Drawing in animateOneFrame doesn't seem to work under OS 10.14 Mojave
// but drawing in drawRect: seems slow under erarlier versions of OS X

#define     drawInDrawRect
#define     config_FramesPerSec 25.0  // 25 frames/sec




/*
 MacOS screensavers need to be compiled 32/64-bit with garbage collection supported or required 
 in the 64-bit architecture if they are to work on 64-bit systems. 
 
 apple documentation
 https://developer.apple.com/library/mac/documentation/UserExperience/Reference/ScreenSaver/Classes/ScreenSaverView_Class/
 
 */

/* multiple displays
 http://www.cocoabuilder.com/archive/cocoa/124621-screensaverview-with-multiple-displays.html
 
 initWithFrame() gets called for each display.
 
 animateOneFrame() is called in order of display:
 animateOneFrame() for display 1,
 animateOneFrame() for display 2, and then back to 1 again, etc.

*/

// example
// http://cocoadevcentral.com/articles/000088.php
// OpenGL example
// http://cocoadevcentral.com/articles/000089.php


/*
 Running a screensaver as your desktop background 
 In a terminal, run
 
 % /System/Library/Frameworks/ScreenSaver.framework/Resources/ScreenSaverEngine.app/Contents/MacOS/ScreenSaverEngine -background &
 */


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
 
 
 11.
    When a user clicks Preview in Screen Saver System Preferences, your ScreenSaverView subclass is instantiated.
 
 
 */

 
 /* cpcc instructions:
    ------------------
	Creation of the screensaver object in MACOSX
	You have to declare your screensaver class as NSPrincipalClass in the project
	so that OSX knows which class to instanciate when it starts the screensaver
	The setting will be written like this in the info.plist file:
	<key>NSPrincipalClass</key>
	<string>cpccScreenSaveLibMac_OsInterface</string>
	
	If editing the info.plist file from xcode, find the key "Principal class" and put a value of cpccScreenSaveLibMac_OsInterface

 */
 
 




@interface cpccScreenSaveLibMac_OsInterface : ScreenSaverView 
{	// ScreenSaverView class is a subclass of NSView
    
}




@end



@implementation cpccScreenSaveLibMac_OsInterface {

    // instance variables
    cpccScreenSaverInterface *ssPtr,
                             *ssConfigurePtr;
    
}

/*
- (void)applicationWillTerminate:(NSNotification *)notification
{
    NSLog(@"the screensaver received applicationWillTerminate");
    infoLog().add("the screensaver received applicationWillTerminate");
}
*/


// flip the coordinates of the NSView, so they are the same as in the case of windows screensavers
- (BOOL)isFlipped  { return YES; }



- (BOOL)isOpaque
/*  If your subclass of NSView implements the isOpaque method to return YES,
 then the framework will never clear anything behind your view or draw any of the views under it.
 YES = this keeps Cocoa from unneccessarily redrawing our superview
 
 The drawing system needs to know whether it should bother updating views that lie behind yours.
 In non-transparent windows you use [self setOpaque:YES]; to speed up drawing.
 
 View Opacity
 
 The display... methods must find an opaque background behind the view that requires displaying and begin drawing from there forward. The display... methods search up the view hierarchy to locate the first view that responds YES to an isOpaque message, bringing the invalidated rectangles along.
 
 If a view instance can guarantee that it will fill all the pixels within its bounds using opaque colors, it should implement the method isOpaque, returning YES. The NSView implementation of isOpaque returns NO. Subclasses should override this method to return YES if all pixels within the view's content will be drawn opaquely.
 
 The isOpaque method is called during drawing, and may be called several times for a given view in a drawing pass. Subclasses should avoid computationally intensive calculations in their implementation of the isOpaque method. Simple tests–for example determining if the background color is opaque as the DraggableItemView does–are acceptable.
 
    // If the background color is opaque, return YES
    // otherwise, return NO
    return [[self backgroundColor] alphaComponent] >= 1.0 ? YES : NO;
 */
{ 	// infoLog().add( "cpccScreenSaveLibMac_OsInterface.isOpaque()");
    return true;
}


/*  This class method allows to select how the desktop visibly transitions to the screen saver view.
    If this method returns YES (default), the screen will gradually darken before the animation begins.
    If it returns NO, the transition will be immediate.
    'No' is more appropriate if the screen saver animates a screen shot of the desktop, as is the case
    for optical lens effects.  */

+ (BOOL)performGammaFade { return NO; }




/*
-(void) util_setTransparency: (bool) isTransparent
{
    infoLog().addf( "cpccScreenSaveLibMac_OsInterface.util_setTransparency(%s)", isTransparent?"Yes":"No");
    
    if (isTransparent)
    {
        //self.alphaValue = 0.0;
        [[self window] setBackgroundColor:[NSColor clearColor]];
        [[self window] setOpaque:NO];
    }
    else
    {
        //self.alphaValue = 0.3;
        [[self window] setOpaque:YES];
    }
    
    m_isOpaque = !isTransparent;
}
*/




- (void)util_createAndInitScreensaverWithWindowHandle
{
    logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
	assert(ssPtr==NULL && "#4813: createScreensaver already called?");
	
    ssPtr = cpccScreenSaverFactory::createScreenSaver();
	assert(ssPtr && "Error 4567: ssPtr = nil");
	
    NSBundle *saverBundle = [NSBundle bundleForClass:[self class]];
    
    // a good opportunity to log the resolved bundleID
    NSString *nsBundleID = [saverBundle bundleIdentifier];
    cpcc_string bundleID([nsBundleID UTF8String]);
    infoLog().addf("Resolved BundleID:%s", bundleID.c_str());
    if (bundleID.compare(cpccAppInfo::MacBundleId) != 0)
        errorLog().addf("Resolved bundleID different from expected (%s)", cpccAppInfo::MacBundleId);
    
    // find the actual path of the screensaver
    NSString* pBundlePath = [saverBundle bundlePath];
    ssPtr->setContainerFolder([pBundlePath UTF8String]);
    ssPtr->m_framesPerSec = config_FramesPerSec;
	NSView * windowHandle = self;
    assert(windowHandle && "Error 2354b: could not get native window handle");
    int monitorID = [self isPreview]? -1 : 0;
    ssPtr->initWithWindowHandle( windowHandle, monitorID);
    
}


- (void)util_destroyScreensaver
{
    infoLog().add( __PRETTY_FUNCTION__ );
    if (ssPtr)
    {
        CPCC_TRY_AND_CATCH(ssPtr->shutDown(), "#7525: ssPtr->shutDown()");
        delete(ssPtr);
        ssPtr = NULL;
    }
	
}


- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    // see displayRectIgnoringOpacity
    // at
    // https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/CocoaViewsGuide/SubclassingNSView/SubclassingNSView.html
    
	logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
    ssConfigurePtr = NULL;
    
    //[[self window] setBackgroundColor:[NSColor clearColor]];
    //[[self window] setBackgroundColor:[NSColor orangeColor]];

    [[NSProcessInfo processInfo] disableAutomaticTermination:@"The screen saver prefers to be closed gracefully."];
    
    // http://stackoverflow.com/questions/14798056/how-to-know-when-a-cocoa-app-is-about-to-quit
    // How to know when a Cocoa app is about to quit?
    //  https://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Classes/NSProcessInfo_Class/#//apple_ref/doc/uid/20000316-SW3
    [[NSProcessInfo processInfo] disableSuddenTermination];
    
    self = [super initWithFrame:frame isPreview:isPreview];
    assert(self && "#9572: 'super initWithFrame:frame isPreview:isPreview' has FAILED");
    
		
    /*
    NSRect windowframe = frame;
    
    windowframe.origin.x += 50;
    windowframe.size.height -= 200;
    windowframe.size.width -= 300;
    
    [self setFrame:windowframe];
    */

    
	
    /*
     https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSWindow_Class/#//apple_ref/c/tdef/NSBackingStoreType
     enum { NSBackingStoreRetained = 0, NSBackingStoreNonretained = 1, NSBackingStoreBuffered = 2 };
     typedef NSUInteger NSBackingStoreType;
     */
    
    // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSWindow_Class/#//apple_ref/occ/instm/NSWindow/setBackingType:
    //[[self window] setBackingType:NSBackingStoreRetained];
    
    
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame() point 1");
    
    /*
    if (ssPtr)
        [self util_setTransparency: ssPtr->getPreserveDeskopContents()];
    */
    
    // this reports an error in cerr:
    //[[self window] setBackgroundColor:[NSColor clearColor]];
    //[[NSColor orangeColor] set];
    // [NSBezierPath fillRect:frame];
    
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame() point 3");
    
    
    // [NSBezierPath fillRect:frame];
    
    // cpccOS::sleep(3000);
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame() exiting");
    
    // 05-01-2016: moved into startAnimation()
    // [self util_createAndInitScreensaverWithWindowHandle];
    [self util_createAndInitScreensaverWithWindowHandle];

    
    return self;
}


- (void)startAnimation
{
	// This method should not do any drawing.
	// You must at least call [super stopAnimation] as shown in the standard template.
    
	logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
    if (!ssPtr) // if it was deleted from a previous stopAnimation
        [self util_createAndInitScreensaverWithWindowHandle];

    
    // cpccOS::sleep(3000);
    
    //[[self window] setAlphaValue:0.3];
    
    //cpccOS::sleep(2000);
    //[[self window] setBackgroundColor:[NSColor clearColor]];
    
    //[[self window] setBackgroundColor:[NSColor orangeColor]];
    //[[self window] setOpaque:NO];
    //[[self window] setAlphaValue:1.0];
    //[[NSColor clearColor] setFill];
    //NSRectFill([self frame]);
    
    // demo
    //NSRect tmpRect = NSMakeRect(30, 40, 100, 150);
    //[[NSColor redColor] set];
    //NSRectFill(tmpRect);
    // cpccOS::sleep(2000);
    
	// Activates the periodic timer that animates the screen saver.
    // A zero value polls as fast as possible while a negative number turns animation off.
	[self setAnimationTimeInterval:1/config_FramesPerSec]; // 25 frames/sec

	
    //infoLog().add( "before [super startAnimation];");
    [super startAnimation];

    //infoLog().add( "cpccScreenSaveLibMac_OsInterface startAnimation exiting");
}


- (void)stopAnimation
{
    // You must at least call [super stopAnimation] as shown in the standard template.
    // StopAnimation, will also get called if the display goes to sleep.
    // When the system will awake from sleep, WILL it call InitWithFrame + StartAnimation ?
    // If YES, then allocation of the screensaver can be done on InitWithFrame() and Deallocation on stopAnimation()
	logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
	
	/*
	 Deactivates the timer that advances the animation.
	 When Mac OS X wants your screen saver to stop doing its thing, it calls stopAnimation.
	 You can override stopAnimation to release resources or 
	 do any other cleanup you want before your screen saver goes away.
     
     when the user is in the screensaver preferences and presses [preview]
     OSX will call stopAnimation() and then initWithFrame() for another instance of the class.
     It will be a different window as it will cover all the screen.
     */
    
    [super stopAnimation];
	usleep(1000* 50);
    
    if (ssPtr)
        [self util_destroyScreensaver];
}



- (void)drawRect:(NSRect)rect
{
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
    
    
	// infoLog().add( __PRETTY_FUNCTION__ );
	// [super drawRect:rect];      // this call draws a black background
    /*          - (void) drawRect:(NSRect) rect
                {
                    [[NSColor blackColor] set];
                    NSRectFill(rect);
                }
      */
    // without it, a gray background is shown as initial background

    
#ifdef drawInDrawRect
	if (ssPtr)  // after stopAnimation is called, we might receive here a call with null ssPtr
		{
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
    
	// infoLog().add( __PRETTY_FUNCTION__ );
		
    
	if (ssPtr)
		{
		//NSDisableScreenUpdates();
		
        ssPtr->animateOneFrame(1.0f / config_FramesPerSec);
			
#ifndef drawInDrawRect
		ssPtr->drawOneFrame();
		ssPtr->flushOneFrame();
#else
        [self setNeedsDisplay:YES];
#endif
		//NSEnableScreenUpdates();
		}
    else
        errorLog().add("screensaverview animateOneFrame with nil ssPtr");
    
    // MacOS Mojave check.
    // Under Mojave, the [NSGraphicsContext currentContext] is nil.
    // Drawing must be moved under drawFrame
    // if ([NSGraphicsContext currentContext] == nil) .....
       
}


-(void)dealloc 
{
    /* ATTENTION:
     -dealloc is called ok when the screen saver gets instantiated by System Preferences;
     -dealloc is not called when ScreenSaverEngine runs the saver. (normal run by timeout or hotcorners)
	 For these reasons the allocation/deallocation of the screensaver was moved under StartAnimation/StopAnimation
     */
	logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
    
    // 05-01-2016: moved into stopAnimation()
    //if (ssPtr)
    //    [self util_destroyScreensaver];
    
    if (ssConfigurePtr)
    {
        delete(ssConfigurePtr);
        ssConfigurePtr = NULL;
    }
    
	[super dealloc];
}



- (BOOL)hasConfigureSheet
{
    logTimeCountrer   m_objLife( __PRETTY_FUNCTION__ );
    
    if (!ssConfigurePtr)
        ssConfigurePtr = cpccScreenSaverFactory::createScreenSaver();
    
    assert(ssConfigurePtr && "Error 7692: ssPtr = nil");
    if (!ssConfigurePtr)
        return false;
    
    return ssConfigurePtr -> hasConfigureSheet();
}


- (NSWindow*)configureSheet
{
    logObjectLife   m_objLife( __PRETTY_FUNCTION__ );
    if (!ssConfigurePtr)
        ssConfigurePtr = cpccScreenSaverFactory::createScreenSaver();
    
    if (!ssConfigurePtr->hasConfigureSheet())
        return nil;
    
    return ssConfigurePtr->showConfigureSheet(self);
}




@end

