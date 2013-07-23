/*  *****************************************
 *  File:		cpccDynamicLinkSDL.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
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

#pragma once


#if defined(_MSC_VER)
	#include "cpccLinkDLL.h"
	typedef unsigned __int32	Uint32;
#elif defined(__APPLE__)
	//typedef uint32_t			Uint32;
	#include "/LIBRARY/FRAMEWORKS/SDL.framework/headers/sdl.h" // or include in the search paths of xcode the folder 
#endif


class SDLfunctions 
{
public:
	int		(*pSDL_Init)		(Uint32 flags);
	void	(*pSDL_Quit)		(void);
	char	*(*pSDL_GetError)	(void);

public:
	SDLfunctions(): 
		pSDL_Init(NULL),
		pSDL_Quit(NULL),
		pSDL_GetError(NULL)
	{
	};
	
	
	virtual ~SDLfunctions()
	{
		initializePointers();
	};
	
	
	void initializePointers(void)
	{
		pSDL_Init = NULL;
		pSDL_Quit = NULL;
		pSDL_GetError = NULL;
	};
	
};



class cpccDynamicLinkSDL: 
#if defined(_WIN32) 
	public cpccLinkDLL, 
#endif
	public SDLfunctions
{	
private:
	
public: // ctor, dtor
	cpccDynamicLinkSDL(const char *aDllFilename="sdl.dll")
#if defined(_WIN32) 	
		: 
		cpccLinkDLL(aDllFilename)
#endif
	{
		
	};
	
	
	virtual ~cpccDynamicLinkSDL()
	{
		initializePointers();
	};
	
	
	virtual void linkAllFunctions()
	{	
	#if defined(_WIN32)	
		functionsLinkedOk = true;
		pSDL_Init		= (int ( *)(Uint32)) linkDllFunction("SDL_Init");
		pSDL_Quit		= (void ( *)(void)) linkDllFunction("SDL_Quit");
		pSDL_GetError	= (char *( *)(void)) linkDllFunction("SDL_GetError");
	#elif defined(__APPLE__)
		pSDL_Init		= SDL_Init;
		pSDL_Quit		= SDL_Quit;
		pSDL_GetError	= SDL_GetError;
	#endif		
	};
	
	
};
