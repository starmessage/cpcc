/*  *****************************************
 *  File:		math.cpccDerivedOperators.h
 *	Purpose:	Portable (cross-platform), light-weight, help class with templated operators
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */




/**
	Helper class with derived operators: + , -, >= , >, <=
	Can be used to complete the operators of classes that only have 
	the following operators: ==, <, +=, -=, *=, /=
 */
template <class T> 
class cpccDerivedOperators
{
    friend T operator+(T const& lhs, T const& rhs)
    { T tmp = lhs; tmp += rhs; return tmp; }
    
    
    friend T operator-(T const& lhs, T const& rhs)
    { T tmp = lhs; tmp -= rhs; return tmp; }
    
    
    friend inline bool operator>=(T const& lhs, T const& rhs)
    { return !(lhs < rhs); }
    
    
    friend inline bool operator<=(T const& lhs, T const& rhs)
    { return !(lhs > rhs); }
    
    
    friend inline bool operator>(T const& lhs, T const& rhs)
    { return (rhs < lhs); }
    
    
};