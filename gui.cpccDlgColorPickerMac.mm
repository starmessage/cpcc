//


#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "gui.cpccDlgColorPicker.h"
#include "io.cpccLog.h"


@interface colorPanelWrapper : NSObject <NSWindowDelegate>
{
    
}

// - (void) colorUpdate:(NSColorPanel*)cp;
- (NSColor *) show:(NSColor *)color;

@end


@implementation colorPanelWrapper




- (BOOL)windowShouldClose:(NSWindow *)sender
{
    // return: YES to allow sender to be closed; otherwise, NO.
    // NSLog(@"colorPanelWrapper: windowShouldClose");
    infoLog().add("colorPanelWrapper: windowShouldClose");
    [NSApp stopModalWithCode:NSOKButton];
    return YES;
}

/*
-(void) colorUpdate:(NSColorPanel*)cp
{
    //NSLog(@"colorPanelWrapper: colorClick");
    infoLog().add("colorPanelWrapper: colorUpdate");
    m_color = cp.color;
    //m_color.fromNSColor([cp color]);
}
*/

- (NSColor *)show:(NSColor *)color;
{
    logFunctionLife _fnShow("colorPanelWrapper: show");
    
    /*
     NSColorPanel is a singleton, that is loaded lazily.
     Only one instance of NSColorPanel may be created.
     Color panel is loaded to memory on first sharedColorPanel call.
     If you will release this panel, application will crash next time you will
     access it, because NSColorPanel class keeps reference to released instance.
     Apple doesn't provide a way to clear resources after sharedColorPanel usage.
     */
    NSColorPanel *panel; // weak ref
    panel = [NSColorPanel sharedColorPanel];
    [panel setShowsAlpha:NO];
    [panel setColor: color ];
    [panel setDelegate:self];
    
    // about the disabled close button under mojave:
    
    // override func viewDidAppear() {
    //var button = view.window?.standardWindowButton(NSWindowButton.CloseButton)
    //button?.enabled = true
    //}
    
    // [[window standardWindowButton:NSWindowCloseButton] setHidden:YES];
    
    
    //[panel setTarget:self]; // Sets the target of the receiver.
    //[panel setAction:@selector(colorUpdate:)]; // Sets the color panel's action message.
    
    // na dokimaso: [window makeKeyAndOrderFront:self];
    // [[panel standardWindowButton:NSWindowCloseButton] setEnabled:YES];
    // [[panel standardWindowButton:NSWindowCloseButton] setHidden:NO];
    
    // test gia na katalabo poio ta dexetai ayta ta events
    // [[panel standardWindowButton:NSWindowZoomButton] setHidden:YES];
    // [[panel standardWindowButton:NSWindowZoomButton] setEnabled:NO];
    // [panel setTitle:@"test title"];
    
    // NSColorWell example / test
    // https://github.com/gnustep/tests-examples/blob/master/gui/GSTest/NSColorWell-test/NSColorWell-test.m
    
    // show panel
    // [panel makeKeyAndOrderFront:self];
    // [panel orderFront: self];
    [NSApp runModalForWindow:panel]; // resets panel position
    [panel setDelegate:nil];
    NSColor *result=panel.color;
    [panel close];
    return result;
}


@end


bool showColorPicker(cpccColor &aColor)
{
    // https://books.google.gr/books?id=a4WTYqg3S1UC&pg=PT266&lpg=PT266&dq=nscolorpanel+modal&source=bl&ots=cDVnXrrPLg&sig=mIZoT6UmWl0JG6iZnng9c2esDU0&hl=en&sa=X&ved=0ahUKEwif-_nO2Y_aAhXPa5oKHVJVClAQ6AEIXjAI#v=onepage&q=nscolorpanel%20modal&f=false
    // http://www.ccp4.ac.uk/dist/checkout/wxPython-src-3.0.2.0/src/osx/carbon/colordlgosx.mm
    infoLog().add("showColorPicker");
    
    colorPanelWrapper *cpw = [[[colorPanelWrapper alloc] init] autorelease];
    NSColor *macColor = aColor.asNSColor();
    macColor = [cpw show:macColor];
    aColor.fromNSColor(macColor);
    //[cp orderOut:cp];
    //[cp close];
    return true;
}


