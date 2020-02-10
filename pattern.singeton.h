/*  *****************************************
 *  File:		pattern.singeton.h
 *	Purpose:	Portable (cross-platform), light-weight functions
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once


// template <typename T, T aDefault>
template <typename T>
class cSingleton
{

public:

    static T& getInstance()
    {
        static T theinstance{} /* = aDefault */;
        return theinstance;
    }

protected:
    cSingleton() {}
    ~cSingleton() {}

private:
    cSingleton(cSingleton const&);
    void operator=(cSingleton const&);
};
