/*  *****************************************
 *  File:		cpccImageWin.h
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
#include <Windows.h>
#include "io.cpccLog.h"
#include "gui.cpccRenderBufferWinGDI.h"
#include "gui.cpccImageBase.h"
#include "gui.cpccDrawingToolsWinDC.h"
#include "gui.cpccWindowWin.h"


class cpccImageWin: public cpccImageBase
{
private:
	HBITMAP	hBitmap, hOldBitmap;
	HDC		m_hDC; 
	cpccDrawingToolsWinDC	m_dtool;

	// todo: why this function is unused ?
	void releaseBitmap(HBITMAP &aBitmap)
	{
		if (aBitmap)
		{
			if (m_hDC)
				::SelectObject (m_hDC, hOldBitmap);
			DeleteObject(aBitmap);
		}
	}

	// todo: why this function is unused ?
	void selectBitmap(HBITMAP &aBitmap)
	{
		hOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (m_hDC, aBitmap));
	}

	
protected:	// ctors

	cpccImageWin(): hBitmap(NULL), hOldBitmap(NULL), m_hDC(NULL), m_dtool(m_hDC)
	{ 
		/*	CreateCompatibleDC (); 
			http://msdn.microsoft.com/en-us/library/windows/desktop/dd183489%28v=vs.85%29.aspx
			If hdc is NULL, then a memory DC will be created that is compatible to the application's 
			current display screen. One caveat to be aware of when this type of memory DC is created 
			is that the memory DC is attached to the thread which created the memory DC. 
			When the thread is destroyed, the memory DC will be destroyed as well.
		*/
		m_hDC = ::CreateCompatibleDC (NULL);
	}
	
	
	~cpccImageWin() 
	{ 
		if (m_hDC)
			::DeleteDC (m_hDC);
	}
	

public:		// functions

	virtual void initWithSizeAndColor(const int aWidth, const int aHeight, const cpccColor &aColor)
	{
		// http://www.cplusplus.com/forum/windows/102452/
		if (hBitmap)
			errorLog().add("cpccImageWin.initWithSizeAndColor() called but hBitmap not null");
		if (!m_hDC)
			errorLog().add("cpccImageWin.initWithSizeAndColor() called but m_hDC is null");

		HDC hdcScreen = GetDC(NULL);
		// http://msdn.microsoft.com/en-us/library/windows/desktop/dd183488%28v=vs.85%29.aspx
		hBitmap = CreateCompatibleBitmap(hdcScreen, aWidth, aHeight);
		ReleaseDC(NULL,hdcScreen);

		hOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (m_hDC, hBitmap));

		RECT r;
		r.top = r.left =0;  r.right	= aWidth; r.bottom = aHeight;

		m_dtool.fillRectWithColor(r , aColor);

		//BITMAP bm;
        //::GetObject (hBitmap, sizeof (bm), &bm);
		//infoLog().addf("cpccImageWin.initialize() with w=%i, h=%i", bm.bmWidth, bm.bmHeight);
	}
    
	// virtual HDC 	getDrawSurfaceHandle_obsolete(void)	{	return m_hDC;	}


	const virtual void 					drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const
	{
		cpccWindowWin *tmpWindow = (cpccWindowWin *) destWindow;
		HDC tmpDC = tmpWindow->getDrawDC();
		cpccDrawingToolsWinDC tmpTool(tmpDC);
		tmpTool.bitBlitFrom(x, y, m_hDC, getWidth(), getHeight(), getTransparentColor());
	}


	const virtual int getWidth(void) const
	{ 
		BITMAP bm;
        ::GetObject (hBitmap, sizeof (bm), &bm);
        return bm.bmWidth;
	} 

	
	const virtual int getHeight(void) const
	{ 
		BITMAP bm;
        ::GetObject (hBitmap, sizeof (bm), &bm);
        return bm.bmHeight;
	} 
	

protected:

	virtual void 		cropTo_impl(const int newTop, const int newLeft, const int newWidth, const int newHeight)
	{
		renderBufferWinGDI croppedBitmapBuffer(m_hDC, newWidth, newHeight);

		BitBlt(croppedBitmapBuffer.dc(), 0,0, newWidth, newHeight, m_hDC, newTop, newLeft, SRCCOPY);

		HBITMAP newhBitmap = CreateCompatibleBitmap(m_hDC, newWidth, newHeight);
		DeleteObject( SelectObject(m_hDC, newhBitmap));
		hBitmap = newhBitmap;

		// copy back
		BitBlt(m_hDC, 0, 0,  newWidth, newHeight, croppedBitmapBuffer.dc(), 0,0, SRCCOPY);
	}


	virtual void 		resizeTo_impl(const int newWidth, const int newHeight)
	{ 
		renderBufferWinGDI resizedBitmapBuffer(m_hDC, newWidth, newHeight);
		SetStretchBltMode(resizedBitmapBuffer.dc(), COLORONCOLOR);
		StretchBlt(resizedBitmapBuffer.dc(), 0,0, newWidth, newHeight, m_hDC, 0, 0, getWidth(), getHeight(), SRCCOPY);
		
		HBITMAP newhBitmap = CreateCompatibleBitmap(m_hDC, newWidth, newHeight);
		DeleteObject( SelectObject(m_hDC, newhBitmap));
		hBitmap = newhBitmap;

		// copy back
		BitBlt(m_hDC, 0, 0,  newWidth, newHeight, resizedBitmapBuffer.dc(), 0,0, SRCCOPY);
		
		// http://www.codeguru.com/cpp/g-m/bitmap/specialeffects/article.php/c4897/Accelerated-Smooth-Bitmap-Resizing.htm
		// http://forums.codeguru.com/showthread.php?128649-using-StretchBlt-with-HBITMAP-how&p=346744#post346744
	}


	



protected: // functions
	// ToDo: transparentCorner is ignored here but is treated by the encapsulating class
	virtual bool initWithFile_impl(const cpcc_char* aFullPathFilename, const bool transparentCorner)
	{
		if (hBitmap)
			errorLog().addf("cpccImageWin.initWithFile(%s) called but hBitmap not null", aFullPathFilename);
		
		hBitmap = (HBITMAP) ::LoadImage(NULL, aFullPathFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE /* |LR_CREATEDIBSECTION */);
		// if LoadImage fails, it returns a NULL handle
		if(!hBitmap)
		{
			// LoadImage faled so get extended error information.
			errorLog().addf( _T("#4653: image.loadFromFile failed:%s") , aFullPathFilename);
			errorLog().addf(_T("       with windows error code:%u") , ::GetLastError());
			return false;
		}
	
		if (!m_hDC)
			errorLog().addf("cpccImageWin.initWithFile(%s) called but m_hDC is null", aFullPathFilename);

		hOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (m_hDC, hBitmap));
		return true;
	}


	inline cpccColor getPixel(const int x, const int y) const
	{
		return m_dtool.getPixel(x,y);
	}
	

	inline virtual void setPixel(const int x, const int y, const cpccColor &aColor)
	{
		m_dtool.setPixel(x,y,aColor);
	}

	
};




