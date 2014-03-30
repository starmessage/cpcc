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
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once


#include "../../../cpcc/cpccColor.h"
#include "../../../cpcc/cpccUnicodeSupport.h"



template <typename T> class cpccProperty
{
private: 
	bool	m_isNull;
	T		m_value;
	void	set(const T &a) { m_value = a; m_isNull=false; }
	T *		get(void) { return m_isNull ? NULL : &m_value; }

public:
	cpccProperty(): m_isNull(true) { }
	
	T* operator()(const T a)	{ m_value = a; return get(); }
	T* operator()(void) 		{ return get(); }

	void operator=(const T &a)	{ set(a); }
	void	clear(void)	{ m_isNull=true; }
	bool	isNull(void) { return  m_isNull; }
};





class cpccCSS
{
public: // enums
	enum	TcssTextAlignValue { taNone, taLeft, taRight, taCenter } ;

public: // constructors
		// cpccCss(cpccCSS *aParentCSS=NULL): parentCSS(aParentCSS) { }

public: // data
		// cpccCss *						parentCSS;
		cpccProperty<cpccColor>			color, bgColor;
		cpccProperty<cpcc_string>		fontName;
		cpccProperty<int>				fontSize; 
		cpccProperty<TcssTextAlignValue> textAlign;
		
	public: // functions
};

