/*  *****************************************
 *  File:		core.cpccUncopyable.h
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


// inharit from this class so that your class becomes Uncopyable
class cpccUncopyable {
public:
  cpccUncopyable(void) {}
  virtual ~cpccUncopyable(void) {}

private:
  cpccUncopyable(const cpccUncopyable&);
  cpccUncopyable& operator=(const cpccUncopyable&);
  
};