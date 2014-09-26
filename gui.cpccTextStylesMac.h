
/*  *****************************************
 *  File:		gui.cpccTextStylesMac.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, c++ library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once

#include "cpccUnicodeSupport.h"
#include "cpccColor.h"


class cpccTextStylesMac
{
public:
    
	void setFont(NSMutableDictionary *aTextAttr, const cpcc_char *aFontName, const int aFontSize)
    {
        assert( aTextAttr && "#9525: cpccTextStylesMac.setFont() NULL aTextAttr");
        if (aTextAttr && aFontName)
        {
            /*
             // convert font
             NSFont* font = [NSFont fontWithName:@"Arial" size:30];
             font = [[NSFontManager sharedFontManager] convertFont:font toHaveTrait:NSFontItalicTrait];
             
             // create with traits and weight
             NSFont* font = [[NSFontManager sharedFontManager]  fontWithFamily:@"Arial" traits:NSFontItalicTrait weight:2 size:30];
             */
            
            NSString *fontName = [[[NSString alloc] initWithUTF8String:aFontName] autorelease];
            
            // works ok
            // [aTextAttr setObject:[NSFont fontWithName:fontName size:aFontSize ]  forKey:NSFontAttributeName];
            
            // https://developer.apple.com/library/mac/documentation/cocoa/reference/applicationkit/classes/NSFontManager_Class/Reference/Reference.html#//apple_ref/doc/uid/20000383-16096
            [aTextAttr setObject: [[NSFontManager sharedFontManager] fontWithFamily:fontName traits:0 weight:1 size:aFontSize] forKey:NSFontAttributeName] ;
        }
    }
    
  
    void setColor(NSMutableDictionary *aTextAttr, cpccColor *aColor)
    {
        if (aTextAttr && aColor)
            [aTextAttr setObject:aColor->asNSColor() forKey:NSForegroundColorAttributeName];
    }
    
    void setKerning(NSMutableDictionary *aTextAttr, const float aKerning)
    {
        if (!aTextAttr)
            return;

        // NSKernAttributeName toValue:nil
        [aTextAttr setObject:[NSNumber numberWithDouble:aKerning] forKey:NSKernAttributeName];
    }
    
    
    void setParagraph(NSMutableDictionary *aTextAttr, const int align)
    {
        
        if (aTextAttr)
        {
            NSMutableParagraphStyle *paragraphStyle = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy]  autorelease];
            
            if (align==0)
                [paragraphStyle setAlignment:NSCenterTextAlignment];
            if (align==1)
                [paragraphStyle setAlignment:NSRightTextAlignment];
            if (align==-1)
                [paragraphStyle setAlignment:NSLeftTextAlignment];
            
            [aTextAttr setObject:paragraphStyle forKey:NSParagraphStyleAttributeName];
        }
    }
};