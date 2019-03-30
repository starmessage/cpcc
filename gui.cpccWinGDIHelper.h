
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
//		cpccWinGDISelectedObject
//		Windows GDI 32 drawing parameters
//////////////////////////////////////////////
class cpccWinGDISelectedObject
{
protected:
	HDC		m_dc;
	HGDIOBJ m_OldObject, m_object;
	int		m_width=0, m_height=0;
	bool	m_sizeNotValid;

public:
	cpccWinGDISelectedObject(const HDC hdc, const HGDIOBJ aNewObj): m_dc(hdc), m_object(aNewObj), m_sizeNotValid(true)
	{
		// change the drawing parameters
		m_OldObject = SelectObject(m_dc, m_object);
	}

	~cpccWinGDISelectedObject()
	{
		// restore the drawing parameters
		SelectObject(m_dc, m_OldObject);
		DeleteObject(m_object);
	}
	
	// rename to replaceSelectedObject
	// and check why it is not equivalent to delete the old cpccWinGDISelectedObject object and create a new
	void deleteCurrentAndSelectAnother(const HGDIOBJ aNewObj)
	{
		DeleteObject( SelectObject(m_dc, aNewObj));
		m_object = aNewObj;
		m_sizeNotValid = true;
	}

};


//////////////////////////////////////////////
//		cpccWinGDIBrush
//		Windows GDI 32 brush construction, destruction
//////////////////////////////////////////////
class cpccWinGDIBrush: public cpccWinGDISelectedObject
{

public:
	cpccWinGDIBrush(const HDC hdc, const COLORREF c): cpccWinGDISelectedObject(hdc, (HGDIOBJ) CreateSolidBrush(c))
	{ }

	/// convert to HBRUSH
    inline operator HBRUSH(void) const   { return (HBRUSH) m_object; }
};


//////////////////////////////////////////////
//		cpccWinGDIPen
//		Windows GDI 32 HPEN construction, destruction
//////////////////////////////////////////////
/*	simple example:
	
	HPEN hPenOld;

	// Draw a red line
	HPEN hLinePen;
	COLORREF qLineColor = RGB(255, 0, 0);
	hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);

	MoveToEx(hdc, 100, 100, NULL);
	LineTo(hdc, 500, 250);

	SelectObject(hdc, hPenOld);
	DeleteObject(hLinePen);
*/
class cpccWinGDIPen : public cpccWinGDISelectedObject
{

public:
	cpccWinGDIPen(const HDC hdc, const int width, const COLORREF c) : cpccWinGDISelectedObject(hdc, CreatePen(PS_SOLID, width, c))
	{ }

	/// convert to HBRUSH
	inline operator HPEN(void) const   { return (HPEN)m_object; }
};



//////////////////////////////////////////////
//		cpccWinGDIBitmap
//		Windows GDI 32 HBITMAP construction, destruction
//////////////////////////////////////////////
class cpccWinGDIBitmap: public cpccWinGDISelectedObject
{
protected:
	void	recalculateSize(void)
	{
		BITMAP bm;
		::GetObject((HBITMAP)m_object, sizeof(bm), &bm);
		m_width = bm.bmWidth;
		m_height = bm.bmHeight;
		m_sizeNotValid = false;
	}

public:
	// constructor by a ready HBITMAP
	cpccWinGDIBitmap(const HDC hdc, const HBITMAP aHBitmap): 
			cpccWinGDISelectedObject(hdc, (HGDIOBJ) aHBitmap)
	{ }

	// constructor by width and height
	cpccWinGDIBitmap(const HDC hdc, const HDC hCompatDC, const int width, const int height): 
			cpccWinGDISelectedObject(hdc, (HGDIOBJ) CreateCompatibleBitmap(hCompatDC, width, height))
	{ }

	/// convert to HBITMAP
    inline operator HBITMAP(void) const   { return (HBITMAP) m_object; }

	const int getWidth(void) 
	{ 
		if (m_sizeNotValid)
			recalculateSize();
		return m_width;
	} 


	const int getHeight(void) 
	{ 
		if (m_sizeNotValid)
			recalculateSize();
		return m_height;
	}

};


//////////////////////////////////////////////
//		cpccWinGDIFont
//		Windows GDI 32 HFONT construction, destruction
//////////////////////////////////////////////
class cpccWinGDIFont
{
private:
	cpccWinGDISelectedObject	*m_GDIObjectPtr;

public:
	// todo: remove depedencies to eFontWeight, eFontQuality
	cpccWinGDIFont(HDC ahDC, const cpcc_char *aFontName, const float *aFontSize, const eFontWeight aWeight, const eFontQuality aQuality): m_GDIObjectPtr(NULL)
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
		
		int size=  (int) (aFontSize ? *aFontSize : 10.0f);
		cpcc_char *fontname = aFontName ? aFontName : _T("Arial");

		HFONT hFont = CreateFont(
							-MulDiv(size, GetDeviceCaps(ahDC, LOGPIXELSY), 72 ),
							0,0,0,
							fWeight,
							0,0,0, DEFAULT_CHARSET, 0,0,
							fQuality,
							FF_DONTCARE,
							fontname);

		m_GDIObjectPtr = new cpccWinGDISelectedObject(ahDC, hFont);
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


private:

	void blitFrom(const HDC sourceDC /* , const int topLeft_x, const int topLeft_y */)
	{
		//infoLog().addf("cpccWinGDIMemoryDC.blitFrom w:%i, h:%i", w, h);
		::BitBlt(m_hDCmemoryBuffer, 0, 0, w, h, sourceDC, 0,0, /* topLeft_x, topLeft_y, */ SRCCOPY /* |CAPTUREBLT */ );
	}


public:

	void blitTo(const HDC destDC /* , const int topLeft_x, const int topLeft_y */ )
	{
		::BitBlt(destDC, /* topLeft_x, topLeft_y, */ 0, 0, w, h, m_hDCmemoryBuffer, 0, 0, SRCCOPY);
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

