﻿/*  *****************************************
 *  File:		cpccTypes.h
  *	Purpose:	Portable (cross-platform), light-weight, library
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

#ifndef cpccTypes_h
#define cpccTypes_h

#include <stdint.h>

//  the Intel x86 processor represents a common little-endian architecture
//  Little-endian systems store the least significant byte in the smallest address
#define cpccLITTLEENDIAN
#if powerpc
	// big endian
#elif intel
	// little endian
#endif

typedef unsigned char		cpccUint8_t;
typedef unsigned char		cpccBYTE;


#if defined(_MSC_VER) 
	#include <stdio.h>
	typedef unsigned __int16	uint16_t;
	typedef unsigned __int32	uint32_t;
#endif

typedef uint16_t            cpccWORD;
typedef uint32_t            cpccDWORD;

// these types are part of C99
static union
{
	char   cpccBYTE_incorrect[sizeof(cpccBYTE)  == 1 ? 1 : -1];
	char	   WORD_incorrect[sizeof(cpccWORD)  == 2 ? 1 : -1];
	char    int32_t_incorrect[sizeof(cpccDWORD) == 4 ? 1 : -1];
    char   uint32_t_incorrect[sizeof(uint32_t)  == 4 ? 1 : -1];
};


/*
#ifdef LINUX
    #define INT64 long 
#elif WIN32
    #define INT64 long long
#endif

*/

/*
#ifndef __INT_TYPES_H__
#define __INT_TYPES_H__
// @brief Microsoft Visual C++ 6.0 and later (Microsoft specific)

#if ( _MSC_VER >= 1200 )
typedef __int8				INT_8;
typedef unsigned__int8		UINT_8;
typedef __int16				INT_16;
typedef unsigned __int16	UINT_16;
typedef __int32				INT_32;
typedef unsigned __int32	UINT_32;
typedef __int64				INT_64;
typedef unsigned __int64	UINT_64;

// @brief HP-UX

#elif defined( hpux ) || defined( _hpux ) || defined( __hpux )
#include <sys/_inttypes.h>
typedef int8_t INT_8;
typedef uint8_t UINT_8;
typedef int16_t INT_16;
typedef uint16_t UINT_16;
typedef int32_t INT_32;
typedef uint32_t UINT_32;
typedef int64_t INT_64;
typedef uint64_t UINT_64;

// @brief ANSI C Standard

#else
#include <stdint.h>
typedef int8_t INT_8;
typedef uint8_t UINT_8;
typedef int16_t INT_16;
typedef uint16_t UINT_16;
typedef int32_t INT_32;
typedef uint32_t UINT_32;
typedef int64_t INT_64;
typedef uint64_t UINT_64;
#endif
#endif // __INT_TYPES_H__ 

*/


#endif
