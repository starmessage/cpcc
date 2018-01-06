
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

#include <iostream>
#include "io.cpccLog.h"

#define CPCC_TRY_AND_CATCH(expr, catchMsg)          \
try { expr;  }                                      \
catch(const std::runtime_error& re) { errorLog().addf(_T("Runtime error caught:\n%s\n%s"), re.what(), catchMsg);  }    \
catch(const std::exception& ex)		{ errorLog().addf(_T("Exception caught:\n%s\n%s"), ex.what(), catchMsg);  }    \
catch(const std::exception *ex)		{ if (ex) errorLog().addf(_T("Exception caught:\n%s\n%s"), ex->what(), catchMsg);  }    \
catch (const char* msg)             { errorLog().addf(_T("Exception caught:\n%s\n%s"), msg, catchMsg);  }    \
catch (const wchar_t* msg)          { errorLog().addf(_T("Exception caught:\n%s\n%s"), msg, catchMsg);  }    \
catch(int i)                        { errorLog().addf(_T("integer exception caught:%i\n%s"), i, catchMsg);  }    \
catch( ... )						{ errorLog().addf(_T("Exception caught:%s"), catchMsg); }


#define CPCC_TRY_AND_CATCH_TO_CERR(expr, developersMsg)          \
try { expr;  }                                      \
catch(const std::runtime_error& re) { std::cerr << "Runtime error caught:\n" << re.what() << std::endl << developersMsg<< std::endl;  }    \
catch(const std::exception& ex)		{ std::cerr << "Exception caught:\n" << ex.what() << std::endl <<  developersMsg<< std::endl;  }    \
catch(const std::exception *ex)		{ if (ex) std::cerr << "Exception caught:\n" << ex->what() << std::endl << developersMsg<< std::endl;  }    \
catch (const char* msg)             { std::cerr << "Exception caught:\n%s\n%s" << msg << std::endl << developersMsg<< std::endl;  }    \
catch (const wchar_t* msg)          { std::cerr << "Exception caught:\n%s\n%s" << msg << std::endl << developersMsg<< std::endl;  }    \
catch(int i)                        { std::cerr << "integer exception caught:" << i << std::endl << developersMsg<< std::endl;  }    \
catch( ... )						{ std::cerr << "Exception caught:" << std::endl << developersMsg << std::endl; }

