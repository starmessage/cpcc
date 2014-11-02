
/*  *****************************************
 *  File:		gui.cpccWinGDIhelper.h
  *	Purpose:	Portable (cross-platform), light-weight, graphic context drawing objects
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	http://www.StarMessageSoftware.com/cpcclibrary
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once

#include <windows.h>


//////////////////////////////////////////////
//		cpccWinGDIObject
//		Windows GDI 32 drawing parameters
//////////////////////////////////////////////
class cpccWinGDIObject
{
protected:
	HDC		m_dc;
	HGDIOBJ m_OldObject, m_object;

public:
	cpccWinGDIObject(const HDC hdc, const HGDIOBJ aNewObj): m_dc(hdc), m_object(aNewObj)
	{
		// change the drawing parameters
		m_OldObject = SelectObject(m_dc, m_object);
	}

	~cpccWinGDIObject()
	{
		// restore the drawing parameters
		SelectObject(m_dc, m_OldObject);
		DeleteObject(m_object);
	}
	
	// rename to replaceSelectedObject
	// and check why it is not equivalent to delete the old cpccWinGDIObject object and create a new
	void deleteCurrentAndSelectAnother(const HGDIOBJ aNewObj)
	{
		DeleteObject( SelectObject(m_dc, aNewObj));
		m_object = aNewObj;
	}

};


//////////////////////////////////////////////
//		cpccWinGDIBrush
//		Windows GDI 32 brush construction, destruction
//////////////////////////////////////////////
class cpccWinGDIBrush: public cpccWinGDIObject
{

public:
	cpccWinGDIBrush(const HDC hdc, const COLORREF c): cpccWinGDIObject(hdc, (HGDIOBJ) CreateSolidBrush(c))
	{ }

	/// convert to HBRUSH
    inline operator HBRUSH(void) const   { return (HBRUSH) m_object; }
};



//////////////////////////////////////////////
//		cpccWinGDIBitmap
//		Windows GDI 32 HBITMAP construction, destruction
//////////////////////////////////////////////
class cpccWinGDIBitmap: public cpccWinGDIObject
{

public:
	// constructor by a ready HBITMAP
	cpccWinGDIBitmap(const HDC hdc, const HBITMAP aHBitmap): 
			cpccWinGDIObject(hdc, (HGDIOBJ) aHBitmap)
	{ }

	// constructor by width and height
	cpccWinGDIBitmap(const HDC hdc, const HDC hCompatDC, const int width, const int height): 
			cpccWinGDIObject(hdc, (HGDIOBJ) CreateCompatibleBitmap(hCompatDC, width, height))
	{ }

	/// convert to HBITMAP
    inline operator HBITMAP(void) const   { return (HBITMAP) m_object; }



	const int getWidth(void) const
	{ 
		BITMAP bm;
        ::GetObject ((HBITMAP) m_object, sizeof (bm), &bm);
        return bm.bmWidth;
	} 


	const int getHeight(void) const
	{ 
		BITMAP bm;
        ::GetObject ((HBITMAP) m_object, sizeof (bm), &bm);
        return bm.bmHeight;
	}

};


//////////////////////////////////////////////
//		cpccWinGDIFont
//		Windows GDI 32 HFONT construction, destruction
//////////////////////////////////////////////
class cpccWinGDIFont
{
private:
	cpccWinGDIObject	*m_GDIObjectPtr;

public:
	cpccWinGDIFont(HDC ahDC, const cpcc_char *aFontName, const int *aFontSize, const eFontWeight aWeight, const eFontQuality aQuality): m_GDIObjectPtr(NULL)
	{ 
		int fWeight = FW_NORMAL;
		switch (aWeight)
			{
				case fwThin: fWeight= FW_THIN; break;
			}
		

		int fQuality = DEFAULT_QUALITY;
		switch (aQuality)
			{
				case fqNonAntiAliased: fQuality=NONANTIALIASED_QUALITY; break;
			}
		
		int size=  aFontSize ? *aFontSize : 10;
		char *fontname = aFontName ? aFontName : "Arial";

		HFONT hFont = CreateFont(
							-MulDiv(size, GetDeviceCaps(ahDC, LOGPIXELSY), 72),
							0,0,0,
							fWeight,
							0,0,0, DEFAULT_CHARSET, 0,0,
							fQuality,
							FF_DONTCARE,
							fontname);

		m_GDIObjectPtr = new cpccWinGDIObject(ahDC, hFont);
	}


	~cpccWinGDIFont() 
	{ 
		if (m_GDIObjectPtr)
			delete m_GDIObjectPtr;
		m_GDIObjectPtr = NULL;
	}

};


//////////////////////////////////////////////
//		cpccWinGDIMemoryDC
//////////////////////////////////////////////

class cpccWinGDIMemoryDC
{
private:
	cpccWinGDIBitmap	*m_bitmapPtr;
	HDC					m_hDCmemoryBuffer;
	int					w,h;
public:
	HDC dc(void) { return m_hDCmemoryBuffer; };


	cpccWinGDIMemoryDC(const HDC hDC, const int aWidth, const int aHeight, const bool copyContents=false): 
			w(aWidth), h(aHeight), 
			m_hDCmemoryBuffer(CreateCompatibleDC(hDC)),
			//m_GDIObjectPtr(NULL),
			m_bitmapPtr(NULL)
	{	
		SetBkMode(m_hDCmemoryBuffer, TRANSPARENT);
		m_bitmapPtr = new cpccWinGDIBitmap(m_hDCmemoryBuffer,hDC , aWidth, aHeight);

		if (copyContents)
			blitFrom(hDC);
	}


	~cpccWinGDIMemoryDC()
	{
		delete m_bitmapPtr;
		DeleteDC(m_hDCmemoryBuffer);
		m_hDCmemoryBuffer=NULL;
	}


	void blitFrom(const HDC sourceDC)
	{
		//infoLog().addf("cpccWinGDIMemoryDC.blitFrom w:%i, h:%i", w, h);
		::BitBlt(m_hDCmemoryBuffer, 0, 0, w, h, sourceDC, 0, 0, SRCCOPY /* |CAPTUREBLT */ );
	}


	void blitTo(const HDC destDC)
	{
		::BitBlt(destDC, 0, 0, w, h, m_hDCmemoryBuffer, 0, 0, SRCCOPY);
	}

};




//////////////////////////////////////////////
//		cpccFontKerningWin
//////////////////////////////////////////////
class cpccFontKerningWin
{
private:
	int m_oldSpacing;
	HDC	m_dc;

public:
	cpccFontKerningWin(HDC hdc, int aKerning): m_dc(hdc), m_oldSpacing(GetTextCharacterExtra(hdc))
	{
		SetTextCharacterExtra(hdc, aKerning); // change the spacing of the characters
	}

	~cpccFontKerningWin()
	{
		SetTextCharacterExtra(m_dc, m_oldSpacing); // restore the drawing tools
	}
	
};

