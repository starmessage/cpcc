
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

////////////////////////////////
// configuration parameters
////////////////////////////////
// this selects if drawing will be done inside the function drawInDrawRect
// if not defined it is done inside the function animateOneFrame
#define     drawInDrawRect
#define     config_FramesPerSec 25.0  // 25 frames/sec

/*
 Screensavers need to be compiled 32/64-bit with garbage collection supported or required 
 in the 64-bit architecture if they are to work on 64-bit systems. 
 
 apple documentation
 https://developer.apple.com/library/mac/documentation/UserExperience/Reference/ScreenSaver/Classes/ScreenSaverView_Class/
 
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
 Running a screensaver as your desktop background [permalink]
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
	creation of the screensaver object in MACOSX
	You have to declare your screensaver class as NSPrincipalClass in the project
	so that when OSX knows which class to instanciate when it starts the screensaver
	The setting will be written like this in the info.plist file:
	<key>NSPrincipalClass</key>
	<string>cpccScreenSaveLibMac_OsInterface</string>
	
	If editing the info.plist file from xcode, find the key "Principal class" and put a value of cpccScreenSaveLibMac_OsInterface
	Todo: rename cpccScreenSaveLibMac_OsInterface to cpccScreenSaveLibMac_OsInterface
 */
 
 
cpccApp	app;	


@interface cpccScreenSaveLibMac_OsInterface : ScreenSaverView 
{	// ScreenSaverView class is a subclass of NSView
    // IBOutlet NSWindow* configSheet; // this name should exist in the nib file of the screensaver configuration screen
    
    
}

/*
 Implementing preservesContentDuringLiveResize to return YES gives you some 
 extra responsibilities, but can improve performance during window resizing.
 
 10.6 added another two new APIs of this sort, layerContentsRedrawPolicy and layerContentsPlacement.
 
 */


@end



@implementation cpccScreenSaveLibMac_OsInterface {

    // instance variables
    cpccScreenSaverInterface *ssPtr;
}

// these are static variables. Move them to instance variables
bool            m_previousPreserveDeskopContents = false;
bool            m_isOpaque = true;
//logObjectLife   m_objLife("cpccScreenSaveLibMac_OsInterface");
// cpccScreenSaverInterface *ssPtr=NULL;


/*
 -(BOOL)isAnimating
    Check if your screen saver is currently between startAnimation and stopAnimation.
    Useful for pausing calculations you don't want to perform while the screen saver is idle.
 
 + (NSBackingStoreType) backingStoreType; { return NSBackingStoreBuffered; }
*/





// flip the coordinates of the NSView, so they are the same as in the case of windows screensavers
- (BOOL)isFlipped  { return YES; }


/*  If your subclass of NSView implements the isOpaque method to return YES,
    then the framework will never clear anything behind your view or draw any of the views under it.
    YES = this keeps Cocoa from unneccessarily redrawing our superview 
    
    The drawing system needs to know whether it should bother updating views that lie behind yours.
    In non-transparent windows you use [self setOpaque:YES]; to speed up drawing.
 */
- (BOOL)isOpaque
/*
 View Opacity
 
 The display... methods must find an opaque background behind the view that requires displaying and begin drawing from there forward. The display... methods search up the view hierarchy to locate the first view that responds YES to an isOpaque message, bringing the invalidated rectangles along.
 
 If a view instance can guarantee that it will fill all the pixels within its bounds using opaque colors, it should implement the method isOpaque, returning YES. The NSView implementation of isOpaque returns NO. Subclasses should override this method to return YES if all pixels within the view's content will be drawn opaquely.
 
 The isOpaque method is called during drawing, and may be called several times for a given view in a drawing pass. Subclasses should avoid computationally intensive calculations in their implementation of the isOpaque method. Simple tests–for example determining if the background color is opaque as the DraggableItemView does–are acceptable.
 
    // If the background color is opaque, return YES
    // otherwise, return NO
    return [[self backgroundColor] alphaComponent] >= 1.0 ? YES : NO;
 */
{ 	// infoLog().add( "cpccScreenSaveLibMac_OsInterface.isOpaque()");
    return m_isOpaque;
}


/*  This class method allows to select how the desktop visibly transitions to the screen saver view.
    If this method returns YES (default), the screen will gradually darken before the animation begins.
    If it returns NO, the transition will be immediate.
    'No' is more appropriate if the screen saver animates a screen shot of the desktop, as is the case
    for optical lens effects.  */

+ (BOOL)performGammaFade { return NO; }


- (NSView *)util_getNativeWindowHandle
{
	return self; // this returns an NSView
}


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


- (void) viewDidLoad
{
    infoLog().add( "cpccScreenSaveLibMac_OsInterface.viewDidLoad()");
    
}


- (void)util_createScreensaver
{
	assert(ssPtr==NULL && "#4813: createScreensaver already called?");
	
    ssPtr = cpccScreenSaverFactory::createScreenSaver();
	assert(ssPtr && "Error 4567: ssPtr = nil");
}


- (void)util_initScreensaverWithWindowHandle
{
    NSView * windowHandle = [self util_getNativeWindowHandle];
    assert(windowHandle && "Error 2354b: could not get native window handle");
    int monitorID = [self isPreview]? -1 : 0;
    ssPtr->initWithWindowHandle( windowHandle, monitorID);
	

}



- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    // see displayRectIgnoringOpacity
    // at
    // https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/CocoaViewsGuide/SubclassingNSView/SubclassingNSView.html
    
	infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame()");
    
    {	// report on monitors found
        cpccMonitorList mList;
        int n = cpccOS::getListOfMonitors(mList);
        infoLog().addf("Number of monitors:%i", n);
        for (int i = 0; i < n; ++i)
            infoLog().addf("Monitor %i: Left %i, top %i, right %i, bottom %i", i, mList[i].left, mList[i].top, mList[i].right, mList[i].bottom );
    }
    
    //cpccOS::sleep(2000);
    
    //[[self window] setBackgroundColor:[NSColor clearColor]];
    //[[self window] setBackgroundColor:[NSColor orangeColor]];

    
    self = [super initWithFrame:frame isPreview:isPreview];
    
    /*
    NSRect windowframe = frame;
    
    windowframe.origin.x += 50;
    windowframe.size.height -= 200;
    windowframe.size.width -= 300;
    
    [self setFrame:windowframe];
    */

     assert(self && "#9572: 'super initWithFrame:frame isPreview:isPreview' has FAILED");
	
    /*
     https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSWindow_Class/#//apple_ref/c/tdef/NSBackingStoreType
     enum { NSBackingStoreRetained = 0, NSBackingStoreNonretained = 1, NSBackingStoreBuffered = 2 };
     typedef NSUInteger NSBackingStoreType;
     */
    
    // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSWindow_Class/#//apple_ref/occ/instm/NSWindow/setBackingType:
    //[[self window] setBackingType:NSBackingStoreRetained];
    [self util_createScreensaver];
    [self util_initScreensaverWithWindowHandle];
    
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame() point 1");
    
    if (ssPtr)
        [self util_setTransparency: ssPtr->getPreserveDeskopContents()];
    
    // this reports an error in cerr:
    //[[self window] setBackgroundColor:[NSColor clearColor]];
    //[[NSColor orangeColor] set];
    // [NSBezierPath fillRect:frame];
    
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame() point 3");
    
    
    // [NSBezierPath fillRect:frame];
    
    // cpccOS::sleep(3000);
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface.initWithFrame() exiting");
    
    return self;
}


- (void)startAnimation
{
	// This method should not do any drawing.
	// You must at least call [super stopAnimation] as shown in the standard template.
    
	infoLog().add( "cpccScreenSaveLibMac_OsInterface startAnimation");
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
    // stopAnimation, it will get called if the display goes to sleep.
	infoLog().add( "cpccScreenSaveLibMac_OsInterface stopAnimation");
	
	/*
	 Deactivates the timer that advances the animation.
	 When Mac OS X wants your screen saver to stop doing its thing, it calls stopAnimation.
	 You can override stopAnimation to release resources or 
	 do any other cleanup you want before your screen saver goes away. 
	 */
	
    /* 
     when the user is in the screensaver preferences and presses [preview]
     OSX will call stopAnimation() and then initWithFrame().
     It will be a different window as it will cover all the screen.
     */
    
    [super stopAnimation];
}


- (void)drawRect:(NSRect)rect
{
    //static bool firstDrawRectCall = true;
    
    
    //NSBezierPath * path;
    //path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:60 yRadius:60];
    //[[NSColor colorWithCalibratedRed:0 green:0 blue:0.4 alpha:0.3] set];
    //[path fill];
    
    
    //[[NSColor clearColor] set];
    //NSRectFillUsingOperation(rect, NSCompositeSourceOver);
    
    //NSRect bounds = [self bounds];
    //[[NSColor clearColor] setFill];
    //[NSBezierPath fillRect:rect];
    //NSRectFill([self frame]);
    /*
    [[self window] setBackgroundColor:[NSColor clearColor]];
    //[[self window] setBackgroundColor:[NSColor orangeColor]];
    [[self window] setOpaque:NO];
    [[self window] setAlphaValue:1.0];
    [[NSColor clearColor] set];
    NSRectFill([self frame]);
     
     // NSRectFill treats the alpha in some odd way, [NSBezierPath fillRect:tRect]; is the solution..
    */
    //[[NSColor clearColor] setFill];
    //NSRectFill(rect);
    //[[self window] setBackgroundColor:[NSColor clearColor]];
    
    
    //infoLog().add("drawRect()");
    
    
    if (ssPtr)
        if (ssPtr->getPreserveDeskopContents() != m_previousPreserveDeskopContents)
        {
            infoLog().add("ssPtr->getPreserveDeskopContents() != previousPreserveDeskopContents");
            infoLog().addf("ssPtr->getPreserveDeskopContents(): %s", ssPtr->getPreserveDeskopContents()?"YES":"NO");
            
            m_previousPreserveDeskopContents = ssPtr->getPreserveDeskopContents();
            [self util_setTransparency: m_previousPreserveDeskopContents];
            //self.alphaValue = 0.3;
            //[[self window] setBackgroundColor:[NSColor clearColor]];
            //[[self window] setOpaque:NO];
        }
    
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
    
    
    //if (!animationStarted)
    //    return;
    
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface drawRect");
    //cpccOS::sleep(1000);
    
    // [super drawRect:rect];      // this call draws a black background
    /*          - (void) drawRect:(NSRect) rect
                {
                    [[NSColor blackColor] set];
                    NSRectFill(rect);
                }
      */
    // without it, a gray background is shown as initial background
    
    // std::cout << "drawRect. w:" << rect.size.width << " h:" << rect.size.height << "\n";
#ifdef drawInDrawRect
	if (ssPtr)
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
    //infoLog().add( "cpccScreenSaveLibMac_OsInterface animateOneFrame");
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
	 -dealloc is called when the screen saver was instantiated by SSystem Preferences;
	 -dealloc is not called when ScreenSaverEngine runs the saver.
	 */
	if (ssPtr)
		{
		infoLog().add( "cpccScreenSaveLibMac_OsInterface dealloc");
		ssPtr->shutDown();
		delete ssPtr;
		ssPtr = NULL;
		}
	
	[super dealloc];
}


- (BOOL)hasConfigureSheet
{
    if (!ssPtr)
        return false;
    
    return (ssPtr -> hasConfigureSheet());
}


- (NSWindow*)configureSheet
{
    logObjectLife routineMark("inside cpccScreenSaveLibMac_OsInterface configureSheet()");
    
    /*
     http://www.cocoabuilder.com/archive/cocoa/14759-screensaver-configure-sheet.html
     you need to create a nib file, "load" the nib file into your
     screen saver at run time, and return the window pointer in your
     ScreenSaverView subclasses -configureSheet method.
     
     This window will be run as a sheet, so it must include buttons that allow the user to end the modal session 
     in which the sheet runs. When the user dismisses the sheet, the controller in charge of the sheet must end 
     the document modal session by calling the NSApplication method endSheet: with the sheet’s window as the argument.
     */
    
    /*
     1) create an NSWindow in a nib file in Interface Builder
     2) add the file to your project
     3) return an instance of that window from the configureSheet method that you’ll be overriding
     
     NSBundle’s loadNibNamed: works just fine for loading your window and any controller objects you need.
     */
    
    /*
     In My_ScreensaverView.h, declare an outlet for your panel:
     
     IBOutlet id configSheet;
     
     Note that I used id instead of NSWindow * or NSPanel *, simply because I don’t know what class you actually are using for the sheet. (Ideally, a NSPanel should be used for a sheet.)
     
     In your nib file, make sure that the File's Owner is an instance of My_ScreensaverView. You can determine this by selecting the icon for this object and then using the Identity inspector to specify the class.
     
     Make the connection between the configSheet outlet and the panel. One way of doing this is to hold down the Control key while dragging from the File's Owner object to the window or panel icon, then selecting configSheet from the pop-up that appears.
     */
    
    // http://troz.net/making-a-mac-screen-saver/
    
    if (!ssPtr)
        return nil;
    
    if (!ssPtr->hasConfigureSheet())
        return nil;
    
    /*
    if (configSheet == nil)
        configSheet = ssPtr->showConfigureSheet(self);

    return configSheet;
     */
    return ssPtr->showConfigureSheet(self);
}


@end

