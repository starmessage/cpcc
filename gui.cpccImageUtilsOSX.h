/*  *****************************************
 *  File:		cpccImageUtilsOSX.h
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

#if defined __OBJC__

#else
#error Must be compiled  as obj-c++
#endif

#pragma once





////////////////////////////////////////////////////////
//      cpccImageUtilsOSX
////////////////////////////////////////////////////////
class cpccImageUtilsOSX
{
public:
    static void removeRepresentations(NSImage * aImagePtr)
    {
    /*
		An NSImage returns an NSArray of its representations in response to a representations message. 
		Each representation is a kind of NSImageRep object:
		NSEPSImageRep 		An image that can be recreated from EPS data that's either stored by the object or at a known location in the file system.
		NSBitmapImageRep 	An image that can be recreated from bitmap or TIFF data.
		NSCustomImageRep 	An image that can be redrawn by a method defined in the application.
		NSCachedImageRep 	An image that has been rendered in an off-screen cache from data or instructions that are no longer available. 
							The image in the cache provides the only data from which the image can be reproduced. 
							
		addRepresentation:
		Adds imageRep to the receiver's list of representations. After invoking this method, you may need to explicitly 
		set features of the new representation, such as size, number of colors, and so on. This is true in particular if 
		the NSImage has multiple image representations to choose from. 
		See NSImageRep and its subclasses for the methods you use to complete initialization.
		Any representation that's added by this method is retained by the NSImage. 
		Note that representations can't be shared among NSImages. 
	*/
        while ([[aImagePtr representations] count]>0)
            [aImagePtr removeRepresentation: [[aImagePtr representations] objectAtIndex:0]];
    }
    
    
    static void logRepresentations(NSImage * aImagePtr)
    {
        if (!aImagePtr)
        {
            warningLog().add("logRepresentations() called with null image");
        }
        
        int nRepresentations = (int) [[aImagePtr representations] count];
        NSString *classname;
        for (int i=0; i<nRepresentations; i++)
        {
            classname = [[[aImagePtr representations] objectAtIndex:i] className ];
            infoLog().addf("Representation #%i has class:%s", i,
                           (char *) [classname UTF8String]  );
        }
        
    }
    
    /*
    static NSBitmapImageRep * getRepresentationOfNSImage(NSImage * aImagePtr, const int index=0)
    {
        //infoLog().add("cpccImageMacBase.getBmpRepresentation() entering");
        if (!aImagePtr)
            return NULL;
        
        int nRepresentations = [[aImagePtr representations] count];
        if (!(nRepresentations>0))
        {   warningLog().add("getBmpRepresentation no representations");
            return NULL;
        }
        else
        {
            logRepresentations(aImagePtr);
        }
        // You can use [NSBitmapImageRep imageRepWithData:[myImage TIFFRepresentation]] to get a bitmap rep
        // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSBitmapImageRep_Class/Reference/Reference.html#//apple_ref/occ/instm/NSBitmapImageRep/isPlanar
        // https://discussions.apple.com/thread/1774118
        if (index<nRepresentations)
            return [[aImagePtr representations] objectAtIndex:index];
        
        
        errorLog().addf("getBmpRepresentationOfNSImage() asked for representation %i, but the count is %i", index, nRepresentations);
        
        
        return NULL;
        
        // http://www.cocoabuilder.com/archive/cocoa/185814-nsimage-representations.html
        // return [ NSBitmapImageRep imageRepWithData:[m_imagePtr TIFFRepresentation]];
    }
    */
    
    
    /*
    static NSBitmapImageRep * getBmpRepresentationOfNSImage(NSImage * aImagePtr)
    {
        //infoLog().add("cpccImageMacBase.getBmpRepresentation() entering");
        if (!aImagePtr)
            return NULL;
        
        int nRepresentations = [[aImagePtr representations] count];
        if (!(nRepresentations>0))
        {   warningLog().add("getBmpRepresentationOfNSImage no representations");
            return NULL;
        }
        else
        {
            logRepresentations(aImagePtr);
        }
        
        for (int i=0; i<nRepresentations ; i++)
            if ([[[aImagePtr representations] objectAtIndex:i] class ] == [NSBitmapImageRep class])
                return [[aImagePtr representations] objectAtIndex:i];
        
        warningLog().add("getBmpRepresentationOfNSImage no BmpRepresentation found");
        return NULL;
    }
    */
    
    static NSBitmapImageRep * createBmpScreenshotOfLockedImage(const int aWidth, const int aHeight)
    {
        NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect(0,0, aWidth, aHeight)];
        
        return rep;
    }
    
    
	static NSBitmapImageRep*  createBitmapRepFromNSImage(NSImage *aImagePtr)
	{
		if	(!aImagePtr) 
			return nil;
			
		NSBitmapImageRep *bitmapImageRep = [[NSBitmapImageRep alloc] initWithData: [aImagePtr TIFFRepresentation]];
		return [bitmapImageRep autorelease];
	}
	
    
	static NSBitmapImageRep * createBlankBmpRepresentation(const int aWidth, const int aHeight, const bool aHasAlpha)
    {
        infoLog().add("cpccImageMacBase.createBmpRepresentation()");
        
        if(aWidth < 1 || aHeight < 1)
            return NULL;
        
        /*
		If planes is NULL or an array of NULL pointers, this method allocates enough memory 
		to hold the image described by the other arguments. You can then obtain pointers to 
		this memory (with the getPixel:atX:y: or bitmapData method) and fill in the image data. 
		In this case, the allocated memory will belong to the object and will be freed when it’s freed.
		*/
		
        // see initForXPMWithDepth
        // http://web.mit.edu/Emacs/source/emacs/src/nsimage.m
        NSBitmapImageRep *rep = [[NSBitmapImageRep alloc]
                                  initWithBitmapDataPlanes: NULL
                                  pixelsWide: aWidth
                                  pixelsHigh: aHeight
                                  bitsPerSample: 8
                                  samplesPerPixel: aHasAlpha ? 4 : 3
                                  hasAlpha: aHasAlpha
                                  isPlanar: NO
                                  colorSpaceName: NSCalibratedRGBColorSpace
                                  bitmapFormat: 0 // RGBA
                                  bytesPerRow: 0 // 0 == autodetect
                                  bitsPerPixel: 0 // 0 == autodetect
                                  ];
        
        if (!rep)
            warningLog().add("cpccImageMacBase.createBmpRepresentation() gave null rep");
        
        return rep;
    }
    
    
    static void drawBmpRepresentation(const int x, const int y, const NSBitmapImageRep * aRep)
    {
        if (!aRep)
            return;
        
        [NSGraphicsContext saveGraphicsState];
        [aRep drawInRect:NSMakeRect(x, y, aRep.size.width, aRep.size.height)];
        [NSGraphicsContext restoreGraphicsState];
    }
    
    
    static void drawNSImageInNSView(int x, int y, NSImage *img, NSView *aView)
    {
        if (!img)
            return;
        if (!aView)
            return;
        
        [aView lockFocus];
        
        /*
         If you want to ensure that a specific image representation is used, 
         you can use the drawRepresentation:inRect: method of NSImage
         */
        [img drawAtPoint:NSMakePoint(x, y) fromRect:NSZeroRect operation: NSCompositeSourceOver fraction: 1.0f];
        [aView unlockFocus];
        
	}
    
    
	static NSImage *loadImageToNSImage(const cpcc_char* aFullPathFilename)
	{
		if (!aFullPathFilename)
			return NULL;
		
        NSString * tmpFilename = [[NSString alloc] initWithUTF8String:aFullPathFilename] ;
        
		NSImage * tmpImg = [[NSImage alloc]init];;
		[tmpImg initWithContentsOfFile:tmpFilename];
        
		[tmpFilename release];
		
        if	(! [tmpImg isValid])
		{
			errorLog().addf("loadImageToNSImage() initWithContentsOfFile failed:%s", aFullPathFilename);
			return NULL;
		}
        
        infoLog().addf("loadImageToNSImage(%s)",aFullPathFilename); 	
		return tmpImg;
	}
    
    
    static NSBitmapImageRep *loadImageToNSBitmapImageRep(const cpcc_char *aFullPathFilename)
    {
       
		NSImage * tmpImg = loadImageToNSImage(aFullPathFilename);
        if (tmpImg==NULL)
            return NULL;
        
        // you have to retain tmpImg to retain the result
        return [NSBitmapImageRep imageRepWithData:[tmpImg TIFFRepresentation] ];
    }
    
    
};


