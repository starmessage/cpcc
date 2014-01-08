/*  *****************************************
 *  File:		cpccNumberWithBounds.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
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


#include <assert.h>

#include "cpccNumberWithBounds.h"

#if defined(cpccNumberWithBounds_DoSelfTest)
	#include "cpcc_SelfTest.h"
#endif


#if defined(cpccNumberWithBounds_DoSelfTest)


template<typename T, const T &m_min, const T &m_max>
void cpccNumberWithBounds<T, m_min, m_max>::selfTest(void)
{
	std::cout << "cpccNumberWithBounds::SelfTest starting\n";
	cpccNumberWithBounds<float, fzero, fone>		f;
	f = 2.0f;
	// std::cout << "f:" << f();
			
	assert( (f == 1.0f) && "#9621a: cpccNumberWithBounds");
	f = 0.5f;
	assert( f == 0.5f && "#9621b: cpccNumberWithBounds");
	f -= 1.0f;
	assert( f == 0.0f && "#9621c: cpccNumberWithBounds");
			
	f=0.1f;
	f*=2.0f;

	float newf = f();
	assert( newf == 0.2f && "#9621d: cpccNumberWithBounds");

	std::cout << "cpccNumberWithBounds::SelfTest ended\n";
}


#endif



#if defined(cpccNumberWithBounds_DoSelfTest)


SELFTEST_BEGIN(cpccNumberWithBounds_SelfTest)
	cpccFloat0_1::selfTest();
SELFTEST_END


#endif // cpccFileSystemMini_DoSelfTest
