
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
#include    "core.cpccTryAndCatch.h"
#include    "app.cpccScreenSaverInterface.h"
#import     <ScreenSaver/ScreenSaver.h>


// //////////////////////////////
// configuration parameters
// //////////////////////////////


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
    cpccScreenSaverInterface  *ssPtr,
                            *ssConfigurePtr;
    bool                    m_isPreview;
    
}


- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
    ssConfigurePtr = NULL;
    
    // Under MacOS Catalina, isPreview is always TRUE. Therefore we ignore it.
    m_isPreview = (frame.size.width <= 400) && (frame.size.height <= 300);
    // Under a retina screen, frame size is not equal to the pixels of the screen. They might be half of it.

    [[NSProcessInfo processInfo] disableAutomaticTermination:@"The screen saver prefers to be closed gracefully."];
    [[NSProcessInfo processInfo] disableSuddenTermination];
    
    // before this, the NSView is not ready to be used
    self = [super initWithFrame:frame isPreview:m_isPreview];
    
    if (self==NULL)
        errorLog().add("#9572: 'super initWithFrame:frame isPreview:isPreview' has FAILED");

    /* make the window smaller for debug purposes.
        NSRect windowframe = frame;
        windowframe.origin.x += 50;
        windowframe.size.height -= 200;
        windowframe.size.width -= 300;
        [self setFrame:windowframe];
    */
    
    // [self.window setPreferredBackingLocation:NSWindowBackingLocationVideoMemory];
    
    return self;
}



- (void)startAnimation
{
    // This method should not do any drawing.
    // You must at least call [super stopAnimation] as shown in the standard template.
    
    logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
    
    // [self.window setOpaque:YES];
    // [self.window setPreferredBackingLocation:NSWindowBackingLocationVideoMemory];
    
    float backingScaleFactor = 1.0f;
    if ([self.window respondsToSelector:@selector(backingScaleFactor)])
    {
        backingScaleFactor = (float) [self.window backingScaleFactor];
        debugLog().addf("startAnimation backingScaleFactor: %g", backingScaleFactor);
    }
    debugLog().addf("startAnimation NSView frame: %g X %g", self.frame.size.width, self.frame.size.height);
    
    if (!ssPtr) // if it was deleted from a previous stopAnimation, then re-create the screensaver
        [self util_createAndInitScreensaverWithWindowHandle: self backingScaleFactor:backingScaleFactor];

    //[[self window] setAlphaValue:0.3];
    //[[self window] setOpaque:NO];
    //[[self window] setAlphaValue:1.0];
    //[[NSColor clearColor] setFill];
    //NSRectFill([self frame]);
    
    
    // Activates the periodic timer that animates the screen saver.
    // A zero value polls as fast as possible while a negative number turns animation off.
    [self setAnimationTimeInterval:1.0f/config_FramesPerSec]; // 25 frames/sec
    [super startAnimation];
}


// +initialize is a class method (static method) that's called at least once when your application starts.
// You can use this method to initialize static variables that are useful across all instances of the class.
// + (void) initialize
// {   }


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
 
 If a view instance can guarantee that it will fill all the pixels within its bounds using opaque colors,
 it should implement the method isOpaque, returning YES.
 The NSView implementation of isOpaque returns NO.
 Subclasses should override this method to return YES if all pixels within the view's content will be drawn opaquely.
 
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


- (void)util_createAndInitScreensaverWithWindowHandle:(NSView *) aViewHandle backingScaleFactor:(int) aBackingScaleFactor
{
    logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );

    if (ssPtr!=NULL)
        errorLog().add("#4813: createScreensaver already called?");
	
    ssPtr = cpccScreenSaverFactory::createScreenSaver();
    if (ssPtr==NULL)
        errorLog().add("Error 4567: ssPtr = nil");
	
    NSBundle *saverBundle = [NSBundle bundleForClass:[self class]];
    
    // a good opportunity to log the resolved bundleID
    NSString *nsBundleID = [saverBundle bundleIdentifier];
    cpcc_string bundleID([nsBundleID UTF8String]);
    infoLog().addf("Resolved BundleID:%s", bundleID.c_str());
    // if (bundleID.compare(cpccAppInfo::MacBundleId) != 0)
    //    errorLog().addf("Resolved bundleID different from expected (%s)", cpccAppInfo::MacBundleId);
    
    // find the actual path of the screensaver
    NSString* pBundlePath = [saverBundle bundlePath];
    if (pBundlePath)
        ssPtr->setContainerFolder([pBundlePath UTF8String]);
    
    // NSView * windowHandle = self;
    // if (windowHandle==NULL)
    //     errorLog().add("Error 2354b: could not get native window handle");
    
    int monitorID = (m_isPreview)? -1 : 0;
    ssPtr->initWithWindowHandle( aViewHandle, monitorID, aBackingScaleFactor);
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

/*
    - (void)viewDidChangeBackingProperties {
        // self.layer.contentsScale = [[self window] backingScaleFactor];
    }
*/





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
    // this is called once after the initWithFrame (before the animation starts)
    // and then called after each animateOneFrame call
    // logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
    
    /*
     ScreenSaverView implements drawRect: to draw a black background.
     
     There are two main ways to do drawing in a screen saver.
     You can either do your drawing in the normal NSView drawRect: method,
     or you can do your drawing in ScreenSaverView’s animateOneFrame method.
     If you do drawing in drawRect: , you should call setNeedsDisplay: with
     an argument of YES in animateOneFrame.
     
     By keeping all the drawing code in drawRect: you ensure it is always drawn when the Cocoa system needs it drawn.
     By keeping all of your computational logic out of the draw path, you ensure multiple-redraws don't cause unwanted side-effects in your animation.
     */
    
	// [super drawRect:rect];      // this call draws a black background. Without it, a gray background is shown as initial background
    /*
    if (false) // make it true for debug
    {
        [[NSColor blackColor] set];
        NSRectFill(rect);
    }
    */

	if (ssPtr)  // after stopAnimation is called, we might receive here a call with null ssPtr
		{
        ssPtr->drawOneFrame();
		ssPtr->flushOneFrame();
		}
    
}



- (void) animateOneFrame {
    
    // logFunctionLife   m_objLife( __PRETTY_FUNCTION__ );
	/*
	 This method is called each time the timer animating the screen saver fires. 
	 The time between calls to this method is always at least animationTimeInterval.
	 It is guaranteed that the focus is locked when this method is called, 
	 so subclasses may do drawing in this method. 
	 The subclass can also let drawRect: perform the drawing, in which 
	 case animateOneFrame needs to call setNeedsDisplay: with an argument of YES. 
	 The default implementation does nothing.
     
     // Under Mojave, the [NSGraphicsContext currentContext] is nil.
     // Drawing must be moved under drawFrame
     // if ([NSGraphicsContext currentContext] == nil) .....
     
	 */
    
	if (ssPtr)
		{
		//NSDisableScreenUpdates();
		
        ssPtr->animateOneFrame(1.0f / config_FramesPerSec);
        [self setNeedsDisplay:YES];

        //NSEnableScreenUpdates();
		}
    else
        errorLog().add("screensaverview animateOneFrame with nil ssPtr");
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
    
    if (ssConfigurePtr==NULL)
    {
        errorLog().add("Error 7692: ssConfigurePtr = nil");
        return false;
    }
    
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

