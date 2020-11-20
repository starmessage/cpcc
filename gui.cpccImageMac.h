/*  *****************************************
 *  File:		cpccImageMac.h
 *	Purpose:	    Portable (cross-platform), light-weight image class
 *	*****************************************
 *  Library:    Cross Platform C++ Classes (cpcc)
 *  Copyright:  2014 StarMessage software.
 *  License:    Free for opensource projects.
 *  	            Commercial license for closed source projects.
 *	Web:	        http://www.StarMessageSoftware.com/cpcclibrary
 *	            http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:   https://github.com/starmessage/cpcc
 *	email:      sales -at- starmessage.info
 *	*****************************************
 */


#if defined __OBJC__

#else
#error Must be compiled  as obj-c++
#endif

#pragma once

#include <string>
#include "io.cpccLog.h"
#include "io.cpccFileSystemMiniOSX.h"

#include "gui.cpccImageBase.h"
#include "gui.cpccDrawingToolsMacOnFocused.h"
#include "gui.cpccWindowMac.h"
#include "core.cpccTryAndCatch.h"

#include <AppKit/NSGraphics.h>

// //////////////////////////////////////////////////////
//
//          cpccImageMacNSImage
//      (to replace the older class cpccImageMacBmpRep)
//
// //////////////////////////////////////////////////////


class cpccImageMacNSImage: public cpccImageBase
{

private:  // data
    NSImage     *m_imagePtr;
    int         m_widthCached,
                m_heightCached;
    
public:
    cpccImageMacNSImage(void): m_imagePtr(NULL), m_widthCached(0), m_heightCached(0)    {    }
    
    virtual int     getWidth(void) const    override {  return m_imagePtr ? m_widthCached : 0;  }
    virtual int     getHeight(void) const   override {  return m_imagePtr ? m_heightCached : 0;  }
    virtual void    drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const override;
    virtual void    resizeTo(const int newWidth, const int newHeight) override;
    virtual bool    initWithFile(const cpcc_char* aFullPathFilename, const bool transparentCorner) override;
    
    // not implemented:
    virtual void    initWithSizeAndColor(const int aWidth, const int aHeight, const cpccColor &aColor) override { };
    virtual void    drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) override  { };
    virtual void    cropTo(const int newTop, int newLeft, int newWidth, int newHeight) override { };
    inline cpccColor    getPixel(const int x, const int y) const override { return cpccYellow; };
    inline virtual void setPixel(const int x, const int y, const cpccColor &aColor) override { };
    inline virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB ) override { };
    
};


// //////////////////////////////////////////////////////
//      cpccImageMacNSImage
//          implementation
// //////////////////////////////////////////////////////

inline bool    cpccImageMacNSImage::initWithFile(const cpcc_char* aFullPathFilename, const bool transparentCorner)
{
    infoLog().addf("cpccImageMacBmpRep.initWithFile_impl(%s)", aFullPathFilename);
    if (!aFullPathFilename)
        return false;
    
    if (m_imagePtr)
        [m_imagePtr release];
    
    m_widthCached = m_heightCached = 0;
    
    NSString * tmpFilename = [[[NSString alloc] initWithUTF8String:aFullPathFilename] autorelease];
    if    (! tmpFilename)
    {
        errorLog().addf("#2834q: initWithFile_impl() failed:%s", aFullPathFilename);
        return NULL;
    }
    
    m_imagePtr = [[NSImage alloc]initWithContentsOfFile:tmpFilename];
    if (m_imagePtr == nil)
    {
        errorLog().addf("#2834q: initWithFile_impl() failed:%s", aFullPathFilename);
        return false;
    }

    m_widthCached  = (int) m_imagePtr.size.width;
    m_heightCached = (int) m_imagePtr.size.height;
    infoLog().addf("resizeTo_impl_newUntested, has resized. width:%i height:%i", m_widthCached, m_heightCached);
    return true;
}


inline void    cpccImageMacNSImage::drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const
{
    // destWindow is ignored probably because it already has the focus
    if (!destWindow)
        return;
    
    if (!m_imagePtr)
        return;
    
    [m_imagePtr drawInRect:NSMakeRect(x, y, getWidth(), getHeight())
              fromRect:NSZeroRect
              operation:NSCompositeSourceOver  // respect transparency.
              fraction:1.0
        respectFlipped:YES
                 hints:nil];
}


inline void    cpccImageMacNSImage::resizeTo(const int newWidth, const int newHeight)
{
    // https://stackoverflow.com/questions/11949250/how-to-resize-nsimage/38442746#38442746
    if (! m_imagePtr.isValid)
        return;

    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
              initWithBitmapDataPlanes:NULL
                            pixelsWide:newWidth
                            pixelsHigh:newHeight
                         bitsPerSample:8
                       samplesPerPixel:4
                              hasAlpha:YES
                              isPlanar:NO
                        colorSpaceName:NSCalibratedRGBColorSpace
                           bytesPerRow:0
                          bitsPerPixel:0];
    
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithBitmapImageRep:rep]];
    [m_imagePtr drawInRect:NSMakeRect(0, 0, newWidth, newHeight) fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0];
    [NSGraphicsContext restoreGraphicsState];

    [m_imagePtr release]; // drop old image
    m_imagePtr = [[NSImage alloc] initWithSize:NSMakeSize(newWidth, newHeight)];
    [m_imagePtr addRepresentation:rep];
    m_widthCached = newWidth;
    m_heightCached = newHeight;
}


// //////////////////////////////////////////////////////
//      cpccImageMacBmpRep
// //////////////////////////////////////////////////////



class cpccImageMacBmpRep: public cpccImageBase
{
private:  // data
    NSBitmapImageRep *           m_bmpPtr;
    // NSImage                     *tmpImageTest=NULL;
    
public: /// ctors
    cpccImageMacBmpRep(): m_bmpPtr(NULL)	{	}
	
public: /// functions
	virtual int     getWidth(void) const	override {   return m_bmpPtr ? (int) m_bmpPtr.pixelsWide : 0;  }
	virtual int     getHeight(void) const	override {	return m_bmpPtr ? (int) m_bmpPtr.pixelsHigh : 0;  }
    virtual void    initWithSizeAndColor(const int aWidth, const int aHeight, const cpccColor &aColor) override;
    virtual void    drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const override;
    virtual void    drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) override;
    
protected: // functions

    virtual bool    initWithFile_impl(const cpcc_char* aFullPathFilename, const bool transparentCorner);
    virtual void    setTransparentColor(const cpccColor &aColor) override;
    virtual void    resizeTo_impl_newUntested(const int newWidth, const int newHeight);
    virtual void    resizeTo_impl(const int newWidth, const int newHeight);
    virtual void    cropTo_impl(const int newTop, int newLeft, int newWidth, int newHeight);
    inline cpccColor    getPixel(const int x, const int y) const override;
    inline virtual void setPixel(const int x, const int y, const cpccColor &aColor) override;
    inline virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB ) override;
    virtual void     setAlpha_impl(int x, int y, const CGFloat a);
    void            makeTransparentPixelsOfColor(const cpccColor &aColor);
    
};



// //////////////////////////////////////////////////////
//      cpccImageMacBmpRep
//          implementation
// //////////////////////////////////////////////////////

inline void  cpccImageMacBmpRep::amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB )
{
    if (!m_bmpPtr)
        return;
    NSColor* color = [m_bmpPtr colorAtX:x y:y];
    NSColor* newColor = [NSColor colorWithDeviceRed : ([color redComponent] * xR)  green : ([color greenComponent] * xG) blue : ([color blueComponent] * xB) alpha : [color alphaComponent]];

    [m_bmpPtr setColor: newColor atX:x y:y];
}


inline cpccColor cpccImageMacBmpRep::getPixel(const int x, const int y) const
{
    if (!m_bmpPtr)
        return cpccGreen;
    NSColor* color = [m_bmpPtr colorAtX:x y:y];
    cpccColor result;
    result.fromNSColor(color);
    return result;
}


inline void cpccImageMacBmpRep::setPixel(const int x, const int y, const cpccColor &aColor)
{
    if (!m_bmpPtr)
        return;
    [m_bmpPtr setColor: aColor.asNSColor() atX:x y:y];
}


inline void cpccImageMacBmpRep::setTransparentColor(const cpccColor &aColor)
{
    cpccImageBase::setTransparentColor(aColor);
    [m_bmpPtr setAlpha:true];
    makeTransparentPixelsOfColor(aColor);
}


inline void cpccImageMacBmpRep::initWithSizeAndColor(const int aWidth, const int aHeight, const cpccColor &aColor)
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
    m_bmpPtr = [[NSBitmapImageRep alloc]
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
                             // bitsPerPixel: 32 // debug retina
                             ];
    // todo for the debug retina: switch off antialias
    if (!m_bmpPtr)
        warningLog().add("cpccImageMacBmpRep.createBmpRepresentation() gave null rep");

}


inline void cpccImageMacBmpRep::drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params)
{
    if (!m_bmpPtr)
        return;
    
    NSImage *tmpImageWithText = [[[NSImage alloc] initWithSize: NSMakeSize(getWidth(), getHeight())] autorelease];
    //[tmpImageWithText setFlipped:YES];
    
    [tmpImageWithText lockFocus]; //  lockFocusFlipped:YES];
    [m_bmpPtr drawInRect:NSMakeRect(0, 0, getWidth(), getHeight())];
    // to adjust the flipped copy
    // [bmpPtr drawInRect:NSMakeRect(0, getHeight(), getWidth(), -getHeight())];
    
    int tmpHeight;
    cpccDrawingToolsMacOnFocused    drawingTool;
    drawingTool.getTextSize(text, params, NULL, &tmpHeight);
    drawingTool.drawText(x, getHeight() - y - tmpHeight  , text, params);
    [tmpImageWithText unlockFocus];
    
    // tmpImageWithText has the text now. Copy it back to a new bmpPtr.
    
    NSData *imageNewData = [tmpImageWithText  TIFFRepresentation]; // converting img into data
    
    if (m_bmpPtr)
        [m_bmpPtr release];
    m_bmpPtr = [[NSBitmapImageRep alloc] initWithData:imageNewData];
}


inline void cpccImageMacBmpRep::drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const
{
    // destWindow is ignored probably because it already has the focus
    
    // this takes too much time in Retina screens
    // return; // debug retina

    // todo: lockFocus and unlockFocus seems to take too much CPU.
    // Can it remain locked with some smart monitoring if it has not changed?
    if (m_bmpPtr==NULL)
    {
        errorLog().add("#7628: cpccImageMacBmpRep.draw() called with null bmpPtr");
        return;
    }
    
    
    
    // const bool useLockFocus = false;
    // NSView * tmpView = NULL;
    // if (useLockFocus)
    // {
    //     cpccWindowMac *tmpWindow = (cpccWindowMac *)destWindow;
    //     tmpView = tmpWindow->getNativeWindowHandle();
    //     [tmpView lockFocus];
    // }
    
    // under retina, this flips vertically the image and takes 1/4 of the screen
    // reset the affine transformations
    // NSAffineTransform *trans = [[[NSAffineTransform alloc] init] autorelease];
    // [trans scaleBy:2.0];
    // [trans set];
    
    /*
    const bool useNSImage=false;
    if (useNSImage)
    {
        NSImage *tmpImageTest = NULL;
        if (!tmpImageTest)
        {
            tmpImageTest = [[NSImage alloc] initWithSize:[m_bmpPtr size]];
            [tmpImageTest addRepresentation: m_bmpPtr];
        }
        [tmpImageTest drawAtPoint:NSMakePoint(x, y) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1];
        return;
    }
    */

    // https://stackoverflow.com/questions/24442017/fast-alternative-to-drawinrect
    // you should remove saving and restoring the NSGraphicsContext, because the -[NSImageRep drawInRect:] method does this itself

    const bool useDrawInRect=true;

    if (useDrawInRect)
        [m_bmpPtr drawInRect:NSMakeRect(x, y, getWidth(), getHeight())
                  fromRect:NSZeroRect
                  operation:NSCompositeSourceOver  // respect transparency.
                  fraction:1.0
            respectFlipped:YES
                     hints:nil];
    else
        [m_bmpPtr     drawAtPoint:NSMakePoint(x, y)];  // faster but no transparency

    // if (tmpView)
    //     [tmpView unlockFocus];
}


inline bool cpccImageMacBmpRep::initWithFile_impl(const cpcc_char* aFullPathFilename, const bool transparentCorner)
{
    infoLog().addf("cpccImageMacBmpRep.initWithFile_impl(%s)", aFullPathFilename);
    
    // not needed if the previous creation was done with autoRelease
    if (m_bmpPtr)
        [m_bmpPtr release];
    m_bmpPtr = NULL;
    
    cpcc_string finalFilename = aFullPathFilename;
    #ifdef __APPLE__
    if (fileSystemOSX_helper::startsWithTilde_OSX(aFullPathFilename))
        finalFilename = fileSystemOSX_helper::expandTilde_OSX(aFullPathFilename);
    #endif
    
    NSString * tmpFilename = [[[NSString alloc] initWithUTF8String:finalFilename.c_str()] autorelease];
    if    (! tmpFilename)
    {
        errorLog().addf("#2834q: initWithFile_impl() failed:%s", finalFilename.c_str());
        return NULL;
    }
    
    NSData *fileContents = [NSData dataWithContentsOfFile:tmpFilename ];
    if    (! fileContents)
    {
        errorLog().addf("#2834r: initWithFile_impl() failed:%s", finalFilename.c_str());
        return NULL;
    }
    
    CPCC_TRY_AND_CATCH(m_bmpPtr = [[NSBitmapImageRep alloc] initWithData:fileContents]
                       , "#8672: Loading image");
    [fileContents release];
    #if !(__has_feature(objc_arc))
    //     [fileContents release];
    #endif
    
    if    (! m_bmpPtr)
    {
        errorLog().addf("#2834: initWithFile_impl() failed:%s", finalFilename.c_str());
        return NULL;
    }
    
    //[bmpPtr autorelease];
    
    // [bmpPtr setOpaque:true]; // debug retina
    
    // now make the color transparent
    if (transparentCorner)
        setTransparentColor(getPixel(0,0));
    
    return true;
}


inline void  cpccImageMacBmpRep::resizeTo_impl(const int newWidth, const int newHeight)
{
    
    // logFunctionLife _tmpLog((char *)"cpccImageMacBmpRep.resizeTo_impl()");
    infoLog().addf("resizeTo_impl, from %iw X %ih to %iw X %ih", getWidth(), getHeight(), newWidth, newHeight);
    
    if (!m_bmpPtr)
        return;
    
    NSImage *tmpImageOfNewSize = [[NSImage alloc] initWithSize: NSMakeSize(CGFloat(newWidth), CGFloat(newHeight))];
    if (!tmpImageOfNewSize)
        return;
    
    if (![tmpImageOfNewSize isValid])
        return;
    
    [tmpImageOfNewSize lockFocus];
    [m_bmpPtr drawInRect:NSMakeRect(CGFloat(0), CGFloat(0), CGFloat(newWidth), CGFloat(newHeight))];
    // to adjust the flipped copy
    //[bmpPtr drawInRect:NSMakeRect(0, newHeight, newWidth, -newHeight)];
    [tmpImageOfNewSize unlockFocus];
    // tmpImageOfNewSize has the resized image now

    NSData *imageNewData = [tmpImageOfNewSize  TIFFRepresentation]; // converting img into data
    if (imageNewData)
    {
        if (m_bmpPtr)
            [m_bmpPtr release]; // does not work properly under Catalina
        m_bmpPtr = NULL;
        
        m_bmpPtr = [[NSBitmapImageRep alloc] initWithData:imageNewData];
        [imageNewData release];
    }
    
    [tmpImageOfNewSize release];
    
    infoLog().addf("resizeTo_impl, has resized. width:%i height:%i", getWidth(), getHeight());
    if (getWidth()!=newWidth)
        warningLog().addf("resizeTo_impl failed. Desired width: %i. Actual width: %i", newWidth, getWidth());
    
    if (getHeight()!=newHeight)
        warningLog().addf("resizeTo_impl failed. Desired height: %i. Actual width: %i", newHeight, getHeight());
    
}


inline void cpccImageMacBmpRep::resizeTo_impl_newUntested(const int newWidth, const int newHeight)
{
    // https://stackoverflow.com/questions/11949250/how-to-resize-nsimage
    NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc]
          initWithBitmapDataPlanes:NULL
                        pixelsWide:newWidth
                        pixelsHigh:newHeight
                     bitsPerSample:8
                   samplesPerPixel:4
                          hasAlpha:YES
                          isPlanar:NO
                    colorSpaceName:NSCalibratedRGBColorSpace
                       bytesPerRow:0
                      bitsPerPixel:0];
    
    NSSize *newSize = new NSSize();
    newSize->width = newWidth;
    newSize->height = newHeight;
    imageRep.size = *newSize;

    
    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithBitmapImageRep:imageRep]];
    [m_bmpPtr drawInRect:NSMakeRect(0, 0, newWidth, newHeight)];
    [NSGraphicsContext restoreGraphicsState];

    //if (bmpPtr)
    //    [bmpPtr release];  // does not work properly under Catalina
    m_bmpPtr = imageRep;
    
    infoLog().addf("resizeTo_impl_newUntested, has resized. width:%i height:%i", getWidth(), getHeight());
    if (getWidth()!=newWidth)
        warningLog().addf("resizeTo_impl_newUntested failed. Desired width: %i. Actual width: %i", newWidth, getWidth());
    
    if (getHeight()!=newHeight)
        warningLog().addf("resizeTo_impl_newUntested failed. Desired height: %i. Actual width: %i", newHeight, getHeight());
    
}


inline void cpccImageMacBmpRep::makeTransparentPixelsOfColor(const cpccColor &aColor)
{
    //std::cout << "setTransparencyOfAllPixels\n";
    for (int x=0; x<getWidth(); x++)
        for (int y=0; y<getWidth(); y++)
            if (getPixel(x, y) == aColor)
                setAlpha_impl(x, y, 0.0f);
}


inline void cpccImageMacBmpRep::setAlpha_impl(int x, int y, const CGFloat a)
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
    
    if (!m_bmpPtr)   return;
    
    NSColor* color = [m_bmpPtr colorAtX:x y:y];
    
    CGFloat r,g,b;
    [color getRed:&r green:&g blue:&b alpha:NULL ];
    
    [m_bmpPtr setColor: [NSColor colorWithDeviceRed:r*a green:g*a blue:b*a alpha:a] atX:x y:y];
}


inline void  cpccImageMacBmpRep::cropTo_impl(const int newTop, int newLeft, int newWidth, int newHeight)
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
