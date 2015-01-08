/*  *****************************************
 *  File:		math.cpccVectorTypes.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, multidimentional number typedefs
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include <vector>
#include "math.cpccVector.h"

typedef cpccVector<float, 2>		cpccVector2f;
typedef cpccVector<float, 3>		cpccVector3f;
typedef cpccVector<int, 3>			cpccVector3i;
typedef cpccVector<int, 2>			cpccVector2i;

typedef cpccVector<int,2>			cpccPoint2i;

typedef std::vector<cpccPoint2i>	cpccListOfPoints2i;
