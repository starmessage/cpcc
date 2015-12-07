/*  *****************************************
 *  File:		cpccCss.h
 *  Version:	1.0
 *	Purpose:	Portable (cross-platform), light-weight library
 *				CSS like display properties
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once


#include "cpccColor.h"
#include "cpccUnicodeSupport.h"


template <typename T> class cpccProperty
{
private: 
	bool	m_isNull;
	T		m_value;
	void	set(const T &a) { m_value = a; m_isNull=false; }

public:
	T 		get(void) const { return m_value; }

public:
	cpccProperty(): m_isNull(true) { }
	cpccProperty(const T& aDefault) : m_isNull(true), m_value(aDefault) { }
		
    // convert to T
    inline operator T(void)     { return get(); }
    
    
	void operator=(const T &a)	{ set(a); }
	void	clear(void)         { m_isNull=true; }
	bool	isNull(void) const  { return  m_isNull; }
};





class cpccCSS
{
public: // enums
	enum	TcssTextAlignValue { taNone, taLeft, taRight, taCenter } ;

public: // constructors
		cpccCSS(void) : fontSize(10)
		{ 

		}

public: // data
		// cpccCss *						parentCSS;
		cpccProperty<cpccColor>			color, bgColor;
		cpccProperty<cpcc_string>		fontName;
		cpccProperty<int>				fontSize; 
		cpccProperty<TcssTextAlignValue> textAlign;
		
public: // functions
    
    
};

