/*  *****************************************
 *  File:		cpccWindowWin.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013,2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <Windows.h>
#include "cpccColor.h"
#include "gui.cpccWindowBase.h"
#include "gui.cpccrenderBufferWinGDI.h"
#include "gui.cpccDrawingToolsWinDC.h"

#pragma comment(lib,"msimg32.lib")	// needed for TransparentBlt()

/**
	windows version of the C++ portable window class
*/
class cpccWindowWin: public cpccWindowBase
{
private:
	RECT					m_rect;
	HDC						m_WindowDC,
							m_DrawDC;
	renderBufferWinGDI*		m_renderBuffer;
	cpccDrawingToolsWinDC	m_dtool;
	enum	{ config_UseDoubleBuffer=true };



protected:		// ctors. 

	/// constructor is protected because this class should not be created alone, but only via its derived class, cpccWindow
	cpccWindowWin(const HWND aWnd): cpccWindowBase(aWnd), m_renderBuffer(NULL), m_dtool(m_DrawDC)
	{
		m_rect.top = m_rect.bottom = m_rect.left = m_rect.right = 0;
		m_DrawDC = m_WindowDC = GetDC(m_windowHandle);
		SetBkMode(m_WindowDC, TRANSPARENT);
		GetWindowRect(m_windowHandle, &m_rect);

		if (config_UseDoubleBuffer)
		{
			// the last paramenter, "true" means take a screenshot here of the actual window contents and put them in the buffer
			m_renderBuffer = new renderBufferWinGDI(m_WindowDC, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, true);
			//m_renderBuffer->blitFrom(m_WindowDC);
			m_DrawDC = m_renderBuffer->dc();
		}
	}


	virtual ~cpccWindowWin()
	{
		if (m_renderBuffer)
			delete (m_renderBuffer);
		if (m_WindowDC)
			ReleaseDC(m_windowHandle, m_WindowDC);
	}


public:
	
	const HDC getDrawDC(void) const { return m_DrawDC; }


protected:		// functions ////////////////////////////////
				// they are protected because no one needs to call the directly. They are called from the base class' methods

	void 		flush(void) 
	{ 
		if (!m_renderBuffer)
			return;	// no need to flush, already drawn on the screen

		m_renderBuffer->blitTo(m_WindowDC);
		// BitBlt(m_WindowDC, m_rect.left, m_rect.top, m_rect.right-m_rect.left, m_rect.bottom-m_rect.top, m_DrawDC, 0, 0, SRCCOPY);
	}

	
	

	void getTextSize(const cpcc_char *txt, int *width, int *height)
	{	cpccTextParams params;
		params.fontName		= fontName.getCurrent().c_str();
		params.fontSize		= &fontSize.getCurrent();
		params.fontWeight	= fontWeight.getCurrent();
		params.fontQuality	= fontQuality.getCurrent();
		params.kerning		= &kerning.getCurrent();
		
		cpccVector2i size = m_dtool.getTextSize(txt, params);
		if (width)
			*width = size.x();
		if (height)
			*height = size.y();
	}

	int 		getHeight(void)												{ return m_rect.bottom - m_rect.top; }
	int 		getWidth(void) 												{ return m_rect.right - m_rect.left; }
	void 		fillWithColor(const cpccColor &c)							{ m_dtool.fillRectWithColor(m_rect, c);  }
	void		fillRectWithColor(const cpccRecti &r, const cpccColor& c)	{ m_dtool.fillRectWithColor(r.asRECT(), c); }
	void		fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c) 
					{ m_dtool.fillEllipseWithColor( left, top, right, bottom, c); } 
	
	
protected:  // the xxxxxx_impl() functions. They should be called only from the anscenstor
    

	cpccColor	getPixel_impl(const int x, const int y)						{ return m_dtool.getPixel(x,y); }
	void		setPixel_impl(const int x, const int y, const cpccColor &c) { m_dtool.setPixel(x,y,c); }
		
	void 		textOut_impl(const int x, const int y, const cpcc_char *txt) 
	{
		cpccTextParams params;
		params.fontName		= fontName.getCurrent().c_str();
		params.fontSize		= &fontSize.getCurrent();
		params.fontWeight	= fontWeight.getCurrent();
		params.fontQuality	= fontQuality.getCurrent();
		params.color		= &drawColor.getCurrent();
		params.kerning		= &kerning.getCurrent();
		params.textAlign	= &textAlign.getCurrent();
		m_dtool.textOut(x, y, txt, params);
	}

	
};

