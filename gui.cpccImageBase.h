/*  *****************************************
 *  File:		gui.cpccImageBase.h
 *  Version:	
 *	Purpose:	Portable (cross-platform), light-weight library
 *				image class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include "cpccColor.h"
#include "gui.cpccWindowBase.h"

// This class is the abstract image class against which you must do your programming
// Only the class factory should know what is the exact implementation that will be instatiated.
class cpccImageBase
{

protected: // data
	bool				m_hasTrasparentColor;
	cpccColor			m_transparentColor;

public:  // constructor
	
	cpccImageBase(): m_hasTrasparentColor(false) { 	}
    virtual ~cpccImageBase() { }
    
public:		// concrete functions

	const cpccColor*	getTransparentColor(void) const					{ return m_hasTrasparentColor? &m_transparentColor: NULL; }
	void				setTransparentColor(const cpccColor &aColor)	{ m_transparentColor=aColor; m_hasTrasparentColor=true; }
	void				removeTransparency(void)						{ m_hasTrasparentColor=false; }

	
public:  // abstract functions
	virtual void			initWithSizeAndColor(const int aWidth, const int aHeight, const cpccColor &aColor)=0;
    virtual bool			initWithFile(const cpcc_char* afullpathfilename, const bool hasTransparentCorner=false)=0;
	virtual cpccColor		getPixel(const int x, const int y) const =0;
    virtual void 			setPixel(const int x, const int y, const cpccColor &aColor)=0;
    const virtual int       getWidth(void) const =0;
    const virtual int       getHeight(void) const =0;
	const virtual void 		drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const =0;
	virtual void 			resizeBy(const float aFactor) { resizeTo((int) (getWidth()*aFactor), (int) (getHeight()*aFactor)); }
	virtual void 			resizeTo(const int newWidth, const int newHeight) =0;
    virtual void 			cropTo(const int newTop, int newLeft, int newWidth, int newHeight)=0;
	
};

