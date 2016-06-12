/*  *****************************************
 *  File:		cpccImageMac.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

// Currently, this class is not used

#if defined __OBJC__

#else
#error Must be compiled  as obj-c++
#endif

#pragma once

#include <string>
#include "io.cpccLog.h"
#include "io.cpccFileSystemMiniOSX.h"

#include "gui.cpccImageBase.h"
#include "gui.cpccImageUtilsOSX.h"
#include "gui.cpccDrawingToolsNSBitmapImageRep.h"
#include "gui.cpccWindowMac.h"
#include "core.cpccTryAndCatch.h"

#include <AppKit/NSGraphics.h>


/*
 notes:
    what is "planar":
    https://www.mikeash.com/pyblog/friday-qa-2012-08-31-obtaining-and-interpreting-image-data.html
    ... It's also possible to store each color in a separate chunk of memory. 
    Each chunk is called a plane, and this format is called "planar". 
    In this case, you essentially have three or four (depending on whether alpha is present) 
    regions of memory, each of which is laid out exactly like the pixels from the grayscale. 
    The pixel's color is a combination of the values from all of the planes. 
    This can sometimes be more convenient to work with, but is often slower,
    and often more complex to work with, so it's a much less common format.
 
    --------------------------
 
    The anchor point (origin) of NSImage is top-left, and
    NSBitmapImageRep is bottom-left. 
    When you convert from one to the other, the image coordinates are flipped vertically.
 
    --------------------------
 
    CGImageRef works under both OSX and IOS
 
    --------------------------
 
 	The key to getting the raw pixel data on an image seems to be
 	-TIFFRepresentation (which, despite its name, returns an NSData object,
 	not an NSImageRep object). I think the most direct way to create a
 	bitmap from an image is probably:
 
 	NSData* tiffData = [theImage TIFFRepresentation];
 	NSBitmapImageRep* bitMapRep = [NSBitmapImageRep imageRepWithData:tiffData];
 
 
 --------------------------
 
 
    cocoa drawing guide
    https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/CocoaDrawingGuide/Images/Images.html
 
    example of accessing the pixels in a representation
    - (BOOL) manipulateImageRep:(NSBitmapImageRep*)rep 
    {
        int Bpr, spp;
        unsigned byte *data;
        int x, y, w, h;
 
        if ([rep bitsPerSample] != 8) {
            // usually each color component is stored
            // using 8 bits, but recently 16 bits is occasionally
            // also used ... you might want to handle that here
            return NO;
            }
 
        Bpr = [rep bytesPerRow];
        spp = [rep samplesPerPixel];
        data = [rep bitmapData];
        w = [rep pixelsWide];
        h = [rep pixelsHigh];
 
        for (y=0; y<h; y++) {
            unsigned byte *p = data + Bpr*y;
            for (x=0; x<w; x++) {
                // do your thing
                // p[0] is red, p[1] is green, p[2] is blue, p[3] can be alpha if spp==4
                p += spp;
                }
            }
        return YES;
        }
 
    -------------------------------
    A texture is a rectangular area of RGBA values, i.e. 
    for each pixel we have red, green, and blue values, and an alpha value. 
    In the Core Animation world this is basically what a CALayer is.
    
    ---------------------------------
 
 
    If you create an NSImage by reading a JPEG file from disk, the object returned to you 
    has a single representation: an NSBitmapImageRep. 
    But, as soon as you call -lockFocus on the image and draw to it, NSImage discards the 
    NSBitmapImageRep and creates an NSCachedImageRep from the data. 
    This is one of NSImage’s optimizations to achieve its goal of getting image data to the 
    screen quickly - even though it interferes with getting the data you want back out to disk. 
    The solution is to create your own NSBitmapImage rep instead of relying on NSImage to 
    maintain the original one for you: ....
    http://blakeseely.tumblr.com/post/556241874/nsimage-deceivingly-simple-or-just-deceiving
    Some developers prefer to bypass NSImage altogether and instead use Quartz CGImage objects instead.
    CGImage bypasses all the funky caching and uncertainty of what NSImage is doing with 
    NSImageRep objects, but also loses the simplicity of reading a file and drawing it to 
    the screen with just a couple method calls.
 
    ---------------------------------
 
    example of creating an image and a rep and draw pixels in the rep
    http://svn.gna.org/svn/gnustep/tests/examples/trunk/gui/GSTest/PixelExactDrawing-test/PixelExactDrawing-test.m
 
 
    ---------------------------------
 
 
    example: create an image and then a bitmapRep
    http://www.cocoabuilder.com/archive/cocoa/180232-getting-colour-data-at-specified-point-in-an-nsimage.html
 
    ---------------------------------
 
    As matter of fact, you can't really lock focus on bitmap image representation.
    Lock focus on image will create a NSCachedImageRep which use offscreen window as drawing buffer.
 
    If you would like to draw to the bitmap, you might have to use quartz's bitmap context or something like this:
    image = [[NSImage alloc] initWithSize:imageSize];
    [image lockFocus];
    [[NSColor colorWithCalibratedWhite:1.0 alpha:1.0] set];
    NSRectFillUsingOperation(NSMakeRect(0, 0, imageSize.width, imageSize.height), NSCompositeCopy);
    [[NSBitmapImageRep alloc] initWithFocusedViewRect: NSMakeRect(0, 0, imageSize.width, imageSize.height)];
    [image unlockFocus];
    [release];
 
    But the second approach is much more in-efficient.(draw to offscreen window and then copy it to bitmap buffer)
 
    What I've ended up doing is using an NSBitmapImageRep from the start,
    instead of an NSImage, and then adding it as a representation to an
    NSImage for drawing purposes later. This way, I can still access the
    RGBA bytes directly via NSBitmapImageRep for my performance-sensitive
    needs. Seems to work as I need.
 
 */



////////////////////////////////////////////////////////
//      cpccImageMacBmpRep
////////////////////////////////////////////////////////



class cpccImageMacBmpRep: public cpccImageBase
{
protected:  // data
    NSBitmapImageRep *                  bmpPtr;
    cpccDrawingToolsNSBitmapImageRep	m_dtool;
	
public: /// ctors
    cpccImageMacBmpRep(): m_dtool(bmpPtr), bmpPtr(NULL)	{	}
	
    
public: /// functions
	virtual const int getWidth(void) const	override {   return bmpPtr ? (int) bmpPtr.pixelsWide : 0;  }
	virtual const int getHeight(void) const	override {	return bmpPtr ? (int) bmpPtr.pixelsHigh : 0;  }
    
    virtual void initWithSizeAndColor(const int aWidth, const int aHeight, const cpccColor &aColor) override
    {
        const bool hasAlpha = true;
        
        //infoLog().addf("cpccImageMacBmpRep.initWithSizeAndColor(%i,%i)", aWidth, aHeight);
        
        if(aWidth < 1 || aHeight < 1)
            return;
        
        /*
         If planes is NULL or an array of NULL pointers, this method allocates enough memory
         to hold the image described by the other arguments. You can then obtain pointers to
         this memory (with the getPixel:atX:y: or bitmapData method) and fill in the image data.
         In this case, the allocated memory will belong to the object and will be freed when it’s freed.
         */
        bmpPtr = [[NSBitmapImageRep alloc]
                                 initWithBitmapDataPlanes: NULL
                                 pixelsWide: aWidth
                                 pixelsHigh: aHeight
                                 bitsPerSample: 8
                                 samplesPerPixel: hasAlpha ? 4 : 3
                                 hasAlpha: hasAlpha
                                 isPlanar: NO
                                 colorSpaceName: NSCalibratedRGBColorSpace
                                 bitmapFormat: 0 // RGBA
                                 bytesPerRow: 0 // 0 == autodetect
                                 bitsPerPixel: 0 // 0 == autodetect
                                 ];
        
        if (!bmpPtr)
            warningLog().add("cpccImageMacBmpRep.createBmpRepresentation() gave null rep");
        
    }
    
public:
    
    virtual void drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) override
    {
        NSImage *tmpImageWithText = [[[NSImage alloc] initWithSize: NSMakeSize(getWidth(), getHeight())] autorelease];
        //[tmpImageWithText setFlipped:YES];
        
        
        [tmpImageWithText lockFocus]; //  lockFocusFlipped:YES];
        [bmpPtr drawInRect:NSMakeRect(0, 0, getWidth(), getHeight())];
        // to adjust the flipped copy
        // [bmpPtr drawInRect:NSMakeRect(0, getHeight(), getWidth(), -getHeight())];
        
        int tmpHeight;
        m_dtool.getTextSize(text, params, NULL, &tmpHeight);
        m_dtool.drawText(x, getHeight() - y - tmpHeight  , text, params);
        [tmpImageWithText unlockFocus];
        
        // tmpImageWithText has the text now. Copy it back to a new bmpPtr.
        
        NSData *imageNewData = [tmpImageWithText  TIFFRepresentation]; // converting img into data
        
        if (bmpPtr)
            [bmpPtr release];
        bmpPtr = [[NSBitmapImageRep alloc] initWithData:imageNewData];
    }
    

    const virtual void 	drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const override
    {
        // todo: lockFocus and unlockFocus seems to take too much CPU.
        // Can it remain locked with some smart monitoring if it has not changed?
        assert(bmpPtr && "#7628: cpccImageMacBmpRep.draw() called with null bmpPtr");
        
        //cpccWindowMac *tmpWindow = (cpccWindowMac *)destWindow;
        //NSView * tmpView = tmpWindow->getNativeWindowHandle();
        //[tmpView lockFocus];
        
        [bmpPtr drawInRect:NSMakeRect(x, y, getWidth(), getHeight())
                  fromRect:NSZeroRect
                 operation:NSCompositeSourceOver
                  fraction:1.0
            respectFlipped:YES
                     hints:nil];
        
        //[tmpView unlockFocus];
    }
    
    
protected: // functions
    

	
    
    virtual bool initWithFile_impl(const cpcc_char* aFullPathFilename, const bool transparentCorner)
	{
		/* not needed if the previous creation was done with autoRelease
        if (bmpPtr)
			[bmpPtr release];
		*/
        infoLog().addf("cpccImageMacBmpRep.initWithFile_impl(%s)", aFullPathFilename);
        
        cpcc_string finalFilename = aFullPathFilename;
        #ifdef __APPLE__
        if (fileSystemOSX_helper::startsWithTilde_OSX(aFullPathFilename))
            finalFilename = fileSystemOSX_helper::expandTilde_OSX(aFullPathFilename);
        #endif
        
		bmpPtr = NULL;
		NSString * tmpFilename = [[[NSString alloc] initWithUTF8String:finalFilename.c_str()] autorelease];
        
        CPCC_TRY_AND_CATCH(bmpPtr = [[NSBitmapImageRep alloc] initWithData:[NSData dataWithContentsOfFile:tmpFilename]]
                           , "#8672: Loading image")
        
		
        if	(! bmpPtr)
		{
			errorLog().addf("#2834: initWithFile_impl() failed:%s", finalFilename.c_str());
			return NULL;
		}
        
        //[bmpPtr autorelease];
        
        // now make the color transparent
        if (transparentCorner)
		    setTransparentColor(getPixel(0,0));
		
        return true;
    }
    
    
    virtual void		setTransparentColor(const cpccColor &aColor) override
    {
        cpccImageBase::setTransparentColor(aColor);
        [bmpPtr setAlpha:true];
        makeTransparentPixelsOfColor(aColor);
    }

    
         
    virtual void 		resizeTo_impl(const int newWidth, const int newHeight)
    {
        // logFunctionLife _tmpLog((char *)"cpccImageMacBmpRep.resizeTo_impl()");
        
        NSImage *imageOfNewSize = [[[NSImage alloc] initWithSize: NSMakeSize(newWidth, newHeight)] autorelease];
        
        [imageOfNewSize lockFocus];
        [bmpPtr drawInRect:NSMakeRect(0, 0, newWidth, newHeight)];
        // to adjust the flipped copy
        //[bmpPtr drawInRect:NSMakeRect(0, newHeight, newWidth, -newHeight)];
        [imageOfNewSize unlockFocus];
        // imageOfNewSize has the resized image now
    
        NSData *imageNewData = [imageOfNewSize  TIFFRepresentation]; // converting img into data
        
        if (bmpPtr)
			[bmpPtr release];
        bmpPtr = [[NSBitmapImageRep alloc] initWithData:imageNewData];
        
        infoLog().addf("resized. width:%i height:%i", getWidth(), getHeight());
        if (getWidth()!=newWidth)
            warningLog().addf("resizeTo_impl failed. Desired width: %i. Actual width: %i", newWidth, getWidth());
        
        if (getHeight()!=newHeight)
            warningLog().addf("resizeTo_impl failed. Desired height: %i. Actual width: %i", newHeight, getHeight());
        // assert((getWidth()==newWidth) && (getHeight()==newHeight) && "#8465: cpccImageMacBmpRep.resizeTo_impl()");
    }
    
    
    virtual void 		cropTo_impl(const int newTop, int newLeft, int newWidth, int newHeight)
	{
        // todo: later....
		
		/*
		    NSInteger pixelsWide = [ciImage extent].size.width;
    NSInteger pixelsHigh = [ciImage extent].size.height;
    NSInteger samplesPerPixel = 4;
    NSBitmapImageRep *bmpImgRep = [[[NSBitmapImageRep alloc] 
                                  initWithBitmapDataPlanes:NULL
                                  pixelsWide:pixelsWide pixelsHigh:pixelsHigh bitsPerSample:8
                                  samplesPerPixel:samplesPerPixel hasAlpha:YES isPlanar:NO
                                  colorSpaceName:NSCalibratedRGBColorSpace bitmapFormat:0
                                  bytesPerRow:0 bitsPerPixel:0] autorelease];
    [NSGraphicsContext saveGraphicsState];
    NSGraphicsContext *ctx = [NSGraphicsContext graphicsContextWithBitmapImageRep:bmpImgRep];
    [NSGraphicsContext setCurrentContext:ctx];
    [[ctx CIContext] drawImage:ciImage atPoint:NSMakePoint(0,0) fromRect:NSMakeRect(0,0,pixelsWide,pixelsHigh)];
    [NSGraphicsContext restoreGraphicsState];
		*/
    }
    
    
    inline cpccColor getPixel(const int x, const int y) const override
    {
        return m_dtool.getPixel(x,y);
    }
    
         
	inline virtual void setPixel(const int x, const int y, const cpccColor &aColor) override
    {
        m_dtool.setPixel(x,y,aColor);
    }

    inline virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB ) override
    {
        m_dtool.amplifyPixel(x, y, xR, xG, xB);
    }
    
    virtual void setAlpha_impl(int x, int y, const CGFloat a)
	{
		/*
         http://stackoverflow.com/questions/1562959/clearing-the-alpha-channel-of-an-nsimage
         All of the pixel formats supported in graphics contexts have premultiplied alpha.
         If the alpha channel is zero, the other channels have to be zero too.
         http://www.gimp.org/docs/plug-in/appendix-alpha.html
         
         Make sure you're using the NSCalibratedRGBColorSpace - if you're not,
         changing the rgb & alpha levels will be strange.
         You can change a color to this space with:
         [color colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
         */
        
        if (!bmpPtr)   return;
        
        NSColor* color = [bmpPtr colorAtX:x y:y];
        
        CGFloat r,g,b;
        [color getRed:&r green:&g blue:&b alpha:NULL ];
        
        [bmpPtr setColor: [NSColor colorWithDeviceRed:r*a green:g*a blue:b*a alpha:a] atX:x y:y];
	}

    
    void makeTransparentPixelsOfColor(const cpccColor &aColor)
    {
        //std::cout << "setTransparencyOfAllPixels\n";
        for (int x=0; x<getWidth(); x++)
            for (int y=0; y<getWidth(); y++)
                if (getPixel(x, y) == aColor)
                    setAlpha_impl(x, y, 0.0f);
    }
    
};



