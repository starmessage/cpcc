
/*  *****************************************
 *  File:		gui.cpccText.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, c++ library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once


#include "cpccUnicodeSupport.h"
#include "cpccColor.h"

enum eFontQuality { fqAntiAliased=0, fqNonAntiAliased, fqUndefined };
enum eFontWeight  { fwNormal=0, fwThin=1, fwUndefined };
enum eTextAlign	  { taLeft=-1, taCenter=0, taRight=1 };


struct cpccTextParams
{
	cpccColor		*color;
	const cpcc_char	*fontName;
	int				*fontSize;
	eTextAlign      *textAlign;
	float			*kerning;
	eFontQuality	fontQuality;
	eFontWeight		fontWeight;

	cpccTextParams():
		color(NULL), fontName(NULL), fontSize(NULL), 
		textAlign(NULL), fontQuality(fqUndefined), fontWeight(fwUndefined), kerning(NULL) 
	{ }
};


