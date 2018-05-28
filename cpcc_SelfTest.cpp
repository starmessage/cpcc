/*  *****************************************
 *  File:		cpcc_SelfTest.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Automatic tests of the library's classes
 *				Include this cpp in your program to enable automatic tests when the program starts
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#include "cpcc_SelfTest.h"

#if (ENABLE_SELF_TESTS==1)


// some selftests of other classes

#include "math.cpccVector.h"
SELFTEST_BEGIN(aTest_NumberXd)
	// MessageBox(NULL, "selftest"	, NULL, NULL);
	cpccVector<int,3>::selfTest();
SELFTEST_END


#include "math.cpccNumberWithBounds.h"

#if defined(cpccNumberWithBounds_DoSelfTest)
	SELFTEST_BEGIN(cpccNumberWithBounds_SelfTest)
		cpccFloat0_1::selfTest();
	SELFTEST_END
#endif

/*
#include "math.cpccFastFloat.h"
SELFTEST_BEGIN(cpccFixedPointNumber_SelfTest)
    cpccFastFloat13::selfTest();
SELFTEST_END
*/

#include "core.cpccStringUtil.h"
SELFTEST_BEGIN(cpccStringUtil_SelfTest)
{
	time_t aTime = time(NULL);
	cpcc_string str(stringConversions::toStr(aTime));
	time_t bTime = stringConversions::fromStr(str.c_str(), (time_t)1000);
	assert((aTime == bTime) && "#9687: stringConversions problem with time_t");
}
SELFTEST_END



#endif