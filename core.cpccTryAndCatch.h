
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
catch(const std::runtime_error& re) { errorLog().addf("Runtime error caught:\n%s\n%s", re.what(), catchMsg);  }    \
catch(const std::exception& ex)		{ errorLog().addf("Exception caught:\n%s\n%s", ex.what(), catchMsg);  }    \
catch(const std::exception *ex)		{ if (ex) errorLog().addf("Exception caught:\n%s\n%s", ex->what(), catchMsg);  }    \
catch( ... )						{ errorLog().addf("Exception caught:%s", catchMsg); }


 // catch(std::exception *ex)			{ if (ex) errorLog().addf("Exception caught:\n%s\n%s", ex->what(), catchMsg);  }  