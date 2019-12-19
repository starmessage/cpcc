/*  *****************************************
 *  File:		core.cpccStringUtil-selfTest.cpp
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include "core.cpccStringUtil.h"
#include "cpcc_SelfTest.h"
#include "io.cpccLog.h"


SELFTEST_BEGIN(cpccStringUtil_SelfTest)

    logFunctionLife lf_(_T("cpccStringUtil_SelfTest"));
	stringUtils::selfTest();

    time_t aTime = time(NULL);
    cpcc_string str(strConvertionsV3::toString(aTime));
    time_t bTime = strConvertionsV3::fromString(str.c_str(), (time_t)1000);
    assert((aTime == bTime) && "#9687: stringConversions problem with time_t");

SELFTEST_END
