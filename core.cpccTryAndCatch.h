
/*  *****************************************
 *  File:		core.cpccTryAndCatch.h
 *	Purpose:	Portable (cross-platform), light-weight, functions
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include "io.cpccLog.h"

#define CPCC_TRY_AND_CATCH(expr, catchMsg)          \
try { expr;  }                                      \
catch(const std::exception& e)  { errorLog().add(catchMsg);  errorLog().add(e.what()); }    \
catch( ... ) { errorLog().add(catchMsg); }
