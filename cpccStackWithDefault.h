/*  *****************************************
 *  File:		cpccStackWithDefault.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				a stack that has also a default value to pop when the stack is empty
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include <stack>


template<typename T>
class cpccStackWithDefault 
	{
	protected: // data
		T				m_defaultValue;
		std::stack<T>	m_stack;

	public: // constructors
		cpccStackWithDefault(const T& a):
			m_defaultValue(a)
		{ }

		cpccStackWithDefault(void) { }
		

	public: // functions
		void	operator=(const T& a)	{ m_defaultValue = a; }

		int		getCount(void)			{ return m_stack.size(); }
		T		pop(void)				
		{	T val= getCount()>0? m_stack.top() : m_defaultValue; 
			if (getCount()>0)
				m_stack.pop();
			return val;
		}

		void	push(const T& a)		{ m_stack.push(a); }
		T&		getCurrent(void)		{ return getCount()>0? m_stack.top() : m_defaultValue; }
		void	setDefault(const T& a)	{ m_defaultValue = a; }

	public:		// selftest ----------------------------------------
		
		static const char * getClassVersion(void) { return "0.50"; };
		static void selfTest() { };

	};



