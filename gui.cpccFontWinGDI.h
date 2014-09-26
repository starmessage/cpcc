/*  *****************************************
 *  File:		cpccFontWinGDI.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, library
 *				windows font class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <Windows.h>




class cpccFontWinGDI
{
private:
	HFONT	m_hFont, m_hFontOld;
	HDC		m_hDC;

public:
	cpccFontWinGDI(HDC ahDC, const cpcc_char *aFontName, const int *aFontSize, const eFontWeight aWeight, const eFontQuality aQuality): m_hDC(ahDC)
	{ 
		if (!m_hDC)
			return;

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

		m_hFont = CreateFont(
			-MulDiv(size, GetDeviceCaps(ahDC, LOGPIXELSY), 72),
			0,0,0,
			fWeight,
			0,0,0, DEFAULT_CHARSET, 0,0,
			fQuality,
			FF_DONTCARE,
			fontname);

		m_hFontOld=(HFONT)SelectObject(m_hDC, m_hFont);
	}

	~cpccFontWinGDI() 
	{ 
		if (m_hDC)
			DeleteObject(SelectObject(m_hDC, m_hFontOld));
	}
};



