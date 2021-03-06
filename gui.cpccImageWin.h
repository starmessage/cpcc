﻿/*  *****************************************
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
#include "gui.cpccImageBase.h"
#include "gui.cpccDrawingToolsWinDC.h"
#include "gui.cpccWindowWin.h"


class cpccImageWin: public cpccImageBase
{
private:
	cpccWinGDIBitmap		*m_GDIHBitmap;
	HDC						m_hDC; 
	cpccDrawingToolsWinDC	m_dtool;

	
protected:	// ctors

	cpccImageWin(): 
			// hBitmap(NULL), hOldBitmap(NULL), 
			m_hDC(NULL), m_dtool(m_hDC),
			m_GDIHBitmap(NULL)
	{ 
		/*	CreateCompatibleDC (); 
			http://msdn.microsoft.com/en-us/library/windows/desktop/dd183489%28v=vs.85%29.aspx
			If hdc is NULL, then a memory DC will be created that is compatible to the application's 
			current display screen. One caveat to be aware of when this type of memory DC is created 
			is that the memory DC is attached to the thread which created the memory DC. 
			When the thread is destroyed, the memory DC will be destroyed as well.
		*/
		m_hDC = ::CreateCompatibleDC (NULL);
		SetBkMode(m_hDC, TRANSPARENT);
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
		//if (hBitmap)
		if (m_GDIHBitmap)
			errorLog().add(_T("cpccImageWin.initWithSizeAndColor() called but m_GDIHBitmap not null"));
		if (!m_hDC)
			errorLog().add(_T("cpccImageWin.initWithSizeAndColor() called but m_hDC is null"));

		HDC hdcScreen = GetDC(NULL);
		// http://msdn.microsoft.com/en-us/library/windows/desktop/dd183488%28v=vs.85%29.aspx
		//hBitmap = CreateCompatibleBitmap(hdcScreen, aWidth, aHeight);
		m_GDIHBitmap = new cpccWinGDIBitmap(m_hDC, hdcScreen, aWidth, aHeight);
		ReleaseDC(NULL,hdcScreen);

		//hOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (m_hDC, hBitmap));

		cpccRecti r;
		r.top = r.left =0;  r.width	= aWidth; r.height = aHeight;

		m_dtool.fillRectWithColor(r , aColor);

		//BITMAP bm;
        //::GetObject (hBitmap, sizeof (bm), &bm);
		//infoLog().addf("cpccImageWin.initialize() with w=%i, h=%i", bm.bmWidth, bm.bmHeight);
	}
    

	virtual void drawInWindow(cpccWindowBase *destWindow, const int x, const int y) const
	{
		if (!destWindow)
			return;
		destWindow->bitBlitFrom(x, y, m_hDC, getWidth(), getHeight(), getTransparentColor());
		/*
		cpccWindowWin *tmpWindow = (cpccWindowWin *) destWindow;
		HDC tmpDC = tmpWindow->getDrawDC();
		cpccDrawingToolsWinDC tmpTool(tmpDC);
		tmpTool.bitBlitFrom(x, y, m_hDC, getWidth(), getHeight(), getTransparentColor());
		*/
	}
		
	virtual void	  drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) {  	m_dtool.drawText(x, y, text, params); }

	virtual int getWidth(void) const { return m_GDIHBitmap ? m_GDIHBitmap->getWidth() : 0; }
	virtual int getHeight(void) const { return m_GDIHBitmap ? m_GDIHBitmap->getHeight() : 0; 	}

protected:

	virtual void 		cropTo_impl(const int newTop, const int newLeft, const int newWidth, const int newHeight)
	{
		if (!m_GDIHBitmap)
			return;

		cpccWinGDIMemoryDC croppedBitmapBuffer(m_hDC, newWidth, newHeight);
		BitBlt(croppedBitmapBuffer.dc(), 0,0, newWidth, newHeight, m_hDC, newTop, newLeft, SRCCOPY);
		
		if (m_GDIHBitmap)
			m_GDIHBitmap->deleteCurrentAndSelectAnother(CreateCompatibleBitmap(m_hDC, newWidth, newHeight));

		// copy back
		BitBlt(m_hDC, 0, 0,  newWidth, newHeight, croppedBitmapBuffer.dc(), 0,0, SRCCOPY);
	}


	virtual void 		resizeTo_impl(const int newWidth, const int newHeight)
	{ 
		if (!m_GDIHBitmap)
			return;

		cpccWinGDIMemoryDC resizedBitmapBuffer(m_hDC, newWidth, newHeight);
		SetStretchBltMode(resizedBitmapBuffer.dc(), COLORONCOLOR);
		StretchBlt(resizedBitmapBuffer.dc(), 0,0, newWidth, newHeight, m_hDC, 0, 0, getWidth(), getHeight(), SRCCOPY);
		
		m_GDIHBitmap->deleteCurrentAndSelectAnother(CreateCompatibleBitmap(m_hDC, newWidth, newHeight));

		// copy back
		BitBlt(m_hDC, 0, 0,  newWidth, newHeight, resizedBitmapBuffer.dc(), 0,0, SRCCOPY);
		
		
		infoLog().addf(_T("cpccImageWin.resizeTo_impl() exiting with new w=%i, new h=%i"), getWidth(), getHeight());
	}


public:

	virtual bool initWithResource(const int resourceID, const bool transparentCorner) override
	{

		if (m_GDIHBitmap)
		{ 
			errorLog().addf(_T("cpccImageWin.initWithResource_impl(%i) called but hBitmap not null"), resourceID);
			return false;
		}
		HINSTANCE hInst = GetModuleHandle(NULL);
		HBITMAP hBitmapTemp = ::LoadBitmap(hInst, MAKEINTRESOURCE(resourceID));
		// if LoadImage fails, it returns a NULL handle
		if (!hBitmapTemp)
		{
			// LoadImage faled so get extended error information.
			errorLog().addf(_T("#4653b: image.initWithResource_impl failed:%i with windows error code:%u"), ::GetLastError());
			return false;
		}

		if (!m_hDC)
		{ 
			errorLog().addf(_T("cpccImageWin.initWithResource_impl(%i) called but m_hDC is null"), resourceID);
			return false;
		}

		// hOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (m_hDC, hBitmap));
		m_GDIHBitmap = new cpccWinGDIBitmap(m_hDC, hBitmapTemp);
		return m_GDIHBitmap != NULL;
	}


protected: // functions

	// ToDo: transparentCorner is ignored here but is treated by the encapsulating class
	virtual bool initWithFile_impl(const cpcc_char* aFullPathFilename, const bool transparentCorner)
	{
		// if (hBitmap)
		if (m_GDIHBitmap)
		{ 
			errorLog().addf(_T("cpccImageWin.initWithFile(%s) called but hBitmap not null"), aFullPathFilename);
			return false;
		}

		HBITMAP hBitmapFromFile = (HBITMAP) ::LoadImage(NULL, aFullPathFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE /* |LR_CREATEDIBSECTION */);
		// if LoadImage fails, it returns a NULL handle
		if(!hBitmapFromFile)
		{
			// LoadImage faled so get extended error information.
			errorLog().addf( _T("#4653: image.loadFromFile failed:%s") , aFullPathFilename);
			errorLog().addf(_T("        with windows error code:%u") , ::GetLastError());
			return false;
		}
	
		if (!m_hDC)
		{
			errorLog().addf(_T("cpccImageWin.initWithFile(%s) called but m_hDC is null"), aFullPathFilename);
			return false;
		}

		// hOldBitmap = reinterpret_cast<HBITMAP> (::SelectObject (m_hDC, hBitmap));
		m_GDIHBitmap = new cpccWinGDIBitmap(m_hDC, hBitmapFromFile);

		return m_GDIHBitmap != NULL;
	}


	inline cpccColor getPixel(const int x, const int y) const
	{	return m_dtool.getPixel(x,y);
	}
	

	inline virtual void setPixel(const int x, const int y, const cpccColor &aColor)
	{	m_dtool.setPixel(x,y,aColor);
	}

	inline virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB)
	{	m_dtool.amplifyPixel(x, y, xR, xG, xB);
	}
};


