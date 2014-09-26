/*  *****************************************
 *  File:		cpccImage.h
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
 
 
#pragma once

#include <string>
#include "io.cpccFileSystemMini.h"

#if defined(__APPLE__)  
	#include "gui.cpccImageMac.h"
	typedef cpccImageMacBmpRep	ImageImpl;

#elif defined(_WIN32)
	#include "gui.cpccImageWin.h"
	typedef cpccImageWin	ImageImpl;

#else
	#error #4367: Unknown platform for TmioanImage
#endif


class cpccImage: public ImageImpl
{


public:	// ctors

	
public: // functions


	virtual void setPixel(const int x, const int y, const cpccColor &aColor)
	{
		if (x<0 || x>=getWidth() || y<0 || y>=getHeight()) 
			return;
			
		ImageImpl::setPixel(x,  y, aColor);
	}
	
	
	virtual cpccColor	getPixel(const int x, const int y)
	{
		if (x<0 || x>=getWidth() || y<0 || y>=getHeight()) 
			return cpccMoccasin;
	
		return ImageImpl::getPixel( x, y);
	}
	

	virtual void 		resizeTo(const int newWidth, const int newHeight)
	{ 
		if (newWidth==getWidth() && newHeight==getHeight())
			return;
		ImageImpl::resizeTo_impl(newWidth, newHeight);
	}
	
	
	virtual void 		cropTo(const int newTop, int newLeft, int newWidth, int newHeight)
	{
        if ( newTop<0 || newTop >= getHeight() ||  newLeft<0 || newLeft >= getWidth() ||
			newWidth>=getWidth() || newHeight>=getHeight() ||  newWidth<0 || newHeight<0)
            return;

        // ensure that the xy origin is smaller than the width or height of the original image
        
		ImageImpl::cropTo_impl(newTop, newLeft, newWidth, newHeight);
	}
	

	virtual bool		initWithFile(const cpcc_char* afullpathfilename, const bool hasTransparentCorner=false)
	{
		if (!afullpathfilename)
		{
			warningLog().add( _T("cpccImage.initWithFile(): empty filename") );
			return false;
		}
		cpccFileSystemMini fs;
        
		if (!fs.fileExists(afullpathfilename))
		{
			warningLog().addf( _T("cpccImage.initWithFile: file does not exist:[%s]") , afullpathfilename);
			return false;
		}
		
		if (!ImageImpl::initWithFile_impl(afullpathfilename, hasTransparentCorner))
        {
            warningLog().addf( _T("cpccImage.initWithFile() failed to load:[%s]") , afullpathfilename);
			return false;
		}
	
		m_hasTrasparentColor=hasTransparentCorner;
		if (m_hasTrasparentColor)
			m_transparentColor = getPixel(0,0);
	
		infoLog().addf("cpccImage.initWithFile(%s) OK. W:%i H:%i", afullpathfilename, getWidth(), getHeight());
		return true;
	}
};

