/*  *****************************************
 *  File:		cpccWindowBase.h
 *  Version:	
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to draw into native windows (HWND on Windows, NSView on MAC)
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include "cpccColor.h"
#include "cpccUnicodeSupport.h"
#include "cpccStackWithDefault.h"

enum eFontQuality { fqAntiAliased=0, fqNonAntiAliased };
enum eFontWeight  { fwNormal=0, fwThin=1 };

class cpccWindowBase
{
private:
	
public:		// data

	// drawing parameters
	cpccStackWithDefault<cpccColor> 	bgColor,
										drawColor;
	
	cpccStackWithDefault<cpcc_string>	fontName;

	cpccStackWithDefault<int>			fontSize,
										textAlign,
										textSpacing;

	cpccStackWithDefault<eFontQuality>	fontQuality;
	cpccStackWithDefault<eFontWeight>	fontWeight;

	
public:  // constructor
	
	cpccWindowBase(): 
		bgColor(cpccGray), 
		drawColor(cpccYellow), 
		fontName(_T("Arial")), 
		fontSize(12),
		textAlign(-1),	// left
		textSpacing(0),
		fontQuality(fqAntiAliased),
		fontWeight(fwNormal)
	{
		
	}

	

};

