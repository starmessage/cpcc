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
	cpccWinGDIMemoryDC*		m_renderBuffer;
	cpccDrawingToolsWinDC	m_dtool;
	enum	{ config_HasDoubleBuffer=true };



protected:		// ctors. 

	/// constructor is protected because this class should not be created alone, but only via its derived class, cpccWindow
	cpccWindowWin(const HWND aWnd): cpccWindowBase(aWnd), m_renderBuffer(NULL), m_dtool(m_DrawDC)
	{
		m_rect.top = m_rect.bottom = m_rect.left = m_rect.right = 0;
		m_DrawDC = m_WindowDC = GetDC(m_windowHandle);
		SetBkMode(m_WindowDC, TRANSPARENT);
		GetWindowRect(m_windowHandle, &m_rect);
		infoLog().addf("cpccWindowWin() with hWND:%X, rect top:%lu, left:%lu, bottom:%lu, right:%lu", m_windowHandle, m_rect.top, m_rect.left, m_rect.bottom, m_rect.right );

		if (config_HasDoubleBuffer)
		{
			// the last paramenter, "true" means take a screenshot here of the actual window contents and put them in the buffer
			m_renderBuffer = new cpccWinGDIMemoryDC(m_WindowDC, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, true);
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

	void lockFocus(void) { } // used in OSX. Here, I might add something like BeginPaint()
	void unlockFocus(void) { } // used in OSX. Here, I might add something like EndPaint()


public:
	
	const HDC		getDrawDC(void) const { return m_DrawDC; }

	virtual void	useDblBuffer(const bool a)	
	{ 
		cpccWindowBase::useDblBuffer(a); 
		#pragma warning(suppress  : 6240)
		m_DrawDC = (a && config_HasDoubleBuffer) ? m_renderBuffer->dc() : m_WindowDC;
	}


protected:		// functions ////////////////////////////////
				// they are protected because no one needs to call the directly. They are called from the base class' methods

	void 		flush(void) 
	{ 
		if ((!m_renderBuffer) || (!m_useDblBuffer))
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
		
		m_dtool.getTextSize(txt, params, width, height);
	}

	inline int 		getHeight(void)	const										{ return m_rect.bottom - m_rect.top; }
	inline int 		getWidth(void) 	const										{ return m_rect.right - m_rect.left; }
	inline int      getTop(void)	const										{ return m_rect.top; }
	inline int      getLeft(void)	const										{ return m_rect.left; }

	inline void 	fillWithColor(const cpccColor &c)							{ m_dtool.fillRectWithColor(m_rect, c);  }
	inline void		fillRectWithColor(const cpccRecti &r, const cpccColor& c)	{ m_dtool.fillRectWithColor(r.asRECT(), c); }
	inline void		fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c)
					{ m_dtool.fillEllipseWithColor( left, top, right, bottom, c); } 
	
	
protected:  // the xxxxxx_impl() functions. They should be called only from the anscenstor
	virtual void 			drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &c)
	{
		{ m_dtool.drawLine(x1, y1, x2, y2, width, c); }
	}


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
		m_dtool.drawText(x, y, txt, params);
	}

	
};

