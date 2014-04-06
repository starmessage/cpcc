/*  *****************************************
 *  File:		cpccNativeWindowHandler.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, library
 *				native window handle in windows and OSX
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#if defined(__APPLE__)
	#include <Cocoa/Cocoa.h>
	// Window handle is NSWindow (void*) on Mac OS X - Cocoa
	//typedef NSWindow* NativeWindowHandle;
	typedef NSView* NativeWindowHandle;
	//typedef void* NativeWindowHandle;
	
	// CGRect
	//NSRect structure, defined in NSGeometry.h. 
	// typedef NSRect  NativeRect;

#elif defined(_WIN32)
	#include <Windows.h>
	// Window handle is HWND (HWND__*) on Windows
	typedef HWND NativeWindowHandle;

	// typedef RECT NativeRect;
#else
	#error Unknown platform for NativeWindowHandle
#endif

