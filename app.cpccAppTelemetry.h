
/*  *****************************************
 *  File:		app.cpccAppTelemetry.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales (at) starmessage.info
 *	*****************************************
 */

#pragma once

#define USE_TRACKERBIRD
// #define USE_NALPEIRON


#ifdef USE_NALPEIRON
	#ifdef USE_TRACKERBIRD
		#error USE_TRACKERBIRD and USE_NALPEIRON must not be both defined
	#endif
#endif


#ifdef USE_TRACKERBIRD
	#include "app.cpccAppTelemetryTrackerbird.h"
	typedef cpccAppTelemetryTrackerBird		cpccAppTelemetry;
#endif

#ifdef USE_NALPEIRON
	#include "app.cpccAppTelemetryNalpeiron.h"
	typedef cpccAppTelemetryNalpeiron		cpccAppTelemetry;
#endif



