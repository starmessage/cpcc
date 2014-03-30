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
	cpccFontWinGDI(HDC ahDC, const cpcc_char *aFontName, const int aFontSize, const eFontWeight aWeight, const eFontQuality aQuality): m_hDC(ahDC)
	{ 
		int fWeight=FW_NORMAL, fQuality=DEFAULT_QUALITY;
		if (aWeight==fwThin)
			fWeight=FW_THIN;

		if (aQuality==fqNonAntiAliased)
			fQuality=NONANTIALIASED_QUALITY;

		m_hFont = CreateFont(
			-MulDiv(aFontSize, GetDeviceCaps(ahDC, LOGPIXELSY), 72),
			0,0,0,
			fWeight,
			0,0,0, DEFAULT_CHARSET, 0,0,
			fQuality,
			FF_DONTCARE,
			aFontName);


		m_hFontOld=(HFONT)SelectObject(m_hDC, m_hFont);
	}

	~cpccFontWinGDI() 
	{ 
		DeleteObject(SelectObject(m_hDC, m_hFontOld));
	}
};



