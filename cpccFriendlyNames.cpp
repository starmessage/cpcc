/*  *****************************************
 *  File:		cpccFriendlyNames.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
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


#include "cpccFriendlyNames.h"
#include "cpcc_SelfTest.h"

#define cpccFriendlyNames_DoSelfTest 1

//////////////////////////////////////////////////////////
///		class cpccFriendlyNames
//////////////////////////////////////////////////////////

/////////////////////////////////////////////
// Selftest cpccFileSystemMini
/////////////////////////////////////////////

#if (cpccFriendlyNames_DoSelfTest==1)


SELFTEST_BEGIN(cpccFriendlyNames_SelfTest)

// MessageBox(NULL, "selftest of cpccFriendlyNames"	, NULL, NULL);
// cpccQmsg::Qmsg("starting selftest","cpccFriendlyNames");

cpccFriendlyNames::selfTest();
SELFTEST_END


#endif 

