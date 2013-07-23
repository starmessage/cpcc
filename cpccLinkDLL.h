/*  *****************************************
 *  File:		cpccLinkDLL.h
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

#ifndef _WIN32
	#error #8531: this file is to be compiled only under windows
#endif

#include <string>
#include <cstdio>
#include <iostream>
#include <windows.h>

#pragma comment(lib,"Kernel32.lib")


class cpccLinkDLL
{
private:
	HMODULE dllHandle;
	std::string DLLfilename;
	
	
protected:
	HMODULE getDllHandle(void) { return dllHandle; };
	bool functionsLinkedOk;
	
	
public: // ctor, dtor
	cpccLinkDLL(const char *aDLLfilename): DLLfilename(""), functionsLinkedOk(false)
	{
		/*
		 http://msdn.microsoft.com/en-us/library/ms684175.aspx
		 If the string specifies a full path, the function 
		 searches only that path for the module.
		 If the string specifies a relative path or a module name 
		 without a path, the function uses a standard search strategy 
		 to find the module.
		 If the string specifies a module name without a path and the 
		 file name extension is omitted, the function appends the default 
		 library extension .dll to the module name. To prevent the function 
		 from appending .dll to the module name, include a trailing point 
		 character (.) in the module name string.
		 When specifying a path, be sure to use backslashes (\), 
		 not forward slashes (/).
		 
		 If the function fails, the return value is NULL. 
		 To get extended error information, call GetLastError.
		 */
		dllHandle = LoadLibrary(aDLLfilename);
		if (dllHandle)
			{
			DLLfilename = aDLLfilename;
			linkDllFunctions();
			}
		else 
			{
			std::cerr << "#7524: failed to load DLL: " << aDLLfilename << std::endl;
			std::cerr << GetLastError() << std::endl;
			};

	};
	
	
	
	virtual ~cpccLinkDLL()
	{
		functionsLinkedOk = false;
		if (dllHandle)
			FreeLibrary(dllHandle);
		dllHandle = NULL;
	};
	
	
public:		// functions

	void *	linkDllFunction(char * functionName)
	{
		void *ptr = (void *)GetProcAddress(getDllHandle(), functionName);
		if (ptr==NULL)
			{
			std::cerr << "#7541: failed to load function:[" << functionName << "] from DLL:[" << DLLfilename << "]\n";
			functionsLinkedOk = false;
			}
		return ptr;
	};
	
	
	bool isLoaded(void)
	{
		return (dllHandle!=NULL);
	};
	
	
	virtual void linkAllFunctions(void) = 0;
};
