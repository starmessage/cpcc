/*  *****************************************
 *  File:		core.cpccLinkLibrary.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight class to help with the 
 *				dynamic library loading and linking of functions (.DLL, .dylib)
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.starmessagesoftware.com/cpcclibrary/
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

//	https://en.wikipedia.org/wiki/Dynamic_loading


#include <string>
#include <cstdio>
#include <iostream>
#include "cpccUnicodeSupport.h"

#ifdef _WIN32
	#include <windows.h>
	#pragma comment(lib, "Kernel32.lib")
	typedef HMODULE 	tLibHandle;
#elif defined(__APPLE__)
    #include <dlfcn.h>
	typedef	void *		tLibHandle;
#endif

#ifdef _WIN32
	class cpccLinkedLibraryPortable_Win
	{
	public:
		static 	tLibHandle	loadLibrary(const cpcc_char *aFilename)
		{
			/*
			 http://msdn.microsoft.com/en-us/library/ms684175.aspx
			 If the string specifies a full path, the function searches only that path for the module.
			 If the string specifies a relative path or a module name without a path, the function uses 
			 a standard search strategy to find the module.
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
			return LoadLibrary(aFilename);
		}

		static void unloadLibrary(const tLibHandle	aHandle) 	{	FreeLibrary(aHandle);	}
	
		static void *getFunctionAddress(const tLibHandle	aHandle, const char * aFunctionName)
		{
			void *ptr = (void *)GetProcAddress(aHandle, aFunctionName);
			if (ptr==NULL)
				cpcc_cerr << _T("#7541: failed to load function:[") << aFunctionName << _T("]\n");
			return ptr;
		}
	};

	typedef cpccLinkedLibraryPortable_Win PORTABLE_LINKLIBRARYFUNCTIONS;
#endif

#ifdef __APPLE__
	class cpccLinkedLibraryPortable_OSX 
	{
	public:
		static 	tLibHandle	loadLibrary(const char *aFilename)
		{
			return dlopen(aFilename, RTLD_LAZY);
			// add error checking
		}

		static void unloadLibrary(const tLibHandle	aHandle) 	{	dlclose(aHandle);	}
	
		static void *getFunctionAddress(const tLibHandle	aHandle, const char * aFunctionName)
		{
			void *ptr = dlsym(aHandle, aFunctionName);
			if (ptr==NULL)
				std::cerr << "#7541: failed to load function:[" << aFunctionName << "]\n";
			return ptr;
		}
	};

	typedef cpccLinkedLibraryPortable_OSX PORTABLE_LINKLIBRARYFUNCTIONS;
#endif


class cpccLinkedLibrary: protected PORTABLE_LINKLIBRARYFUNCTIONS 
{

private:
	tLibHandle 			m_libHandle;
	
protected:
	tLibHandle			getLibHandle(void) const { return m_libHandle;  }
	cpcc_stringstream	&errorStream;

public:	// members
	


public: // ctor, dtor
	cpccLinkedLibrary(const cpcc_char *aLibraryfilename, cpcc_stringstream &anErrorStream):
				errorStream(anErrorStream)
	{
		m_libHandle = loadLibrary(aLibraryfilename);
		if (!m_libHandle)
		{ 
			cpcc_cerr << _T("#7524: failed to load DLL: ") << aLibraryfilename;
			errorStream << _T("#7524: failed to load DLL: ") << aLibraryfilename;
			if (cpccFileSystemMini::fileExists(aLibraryfilename))
				errorStream << _T(". The file exists\n");
			else
				errorStream << _T(". The file does not exist\n");
		}
    };


	virtual ~cpccLinkedLibrary()
	{
		if (m_libHandle)
			unloadLibrary(m_libHandle);
		m_libHandle = NULL;
	};
	

	void *	getFunction(const char * aFunctionName)
	{
        if (!isLoaded())
            errorStream << _T("#9771e: Cannot load function ") << aFunctionName << _T(" because the dynamically was not loaded\n");

		void *ptr = PORTABLE_LINKLIBRARYFUNCTIONS::getFunctionAddress(m_libHandle, aFunctionName);
		if (!ptr)
			errorStream << _T("#9771a: did not find function ") << aFunctionName << _T(" in the dynamically loaded library\n");
		return  ptr;
	}

public:		// functions
	
	bool	isLoaded(void) const { return (m_libHandle != NULL); };

};
