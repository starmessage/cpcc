
/*  *****************************************
 *  File:		app.cpccAppTelemetryNalpeiron.h
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

#include "core.cpccLinkLibrary.h"



// the following typedefs are taken from NalpeironNSA.h

#if defined(__APPLE__)
// Mac OS X Specific header stuff here
	#include <TargetConditionals.h>
	#define WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif


	typedef int(*NalpLibOpen_t)(const char *xmlParams);
	typedef int(*NalpLibClose_t)(void);
	typedef int(*NalpGetErrorMsg_t)(int nalpErrno, char **errMsg);
	
	//Returns true if DLL matches customer number and last 5
	// digits of product ID
	typedef	int(*NSAValidateLibrary_t)(uint32_t custNum, uint32_t prodNum);


	//Routine to inform Nalpeiron server that application has started
	typedef int(*NSAApStart_t)
		(const char *nsaClientData, uint32_t *transID);

	//Routine to inform Nalpeiron server that application has ended
	typedef int(*NSAApStop_t)
		(const char *nsaClientData, uint32_t *transID);
	
	//Checks state of end user privacy setting
	typedef int(*NSAGetPrivacy_t)();

	//Set end user privacy (on - 1, off - 0, unset - 2)
	typedef int(*NSASetPrivacy_t)(unsigned short nsaPriv);

	//Free memory allocated via NSA
	typedef int(*NSAFree_t)(void *memptr);

	// Checks server connectivity
	typedef int(*NSATestConnection_t)
		(const char *nsaClientData, uint32_t *transID);


    //Send system information from end user's computer to Nalpeiron
	typedef int(*NSASysInfo_t)
		(const char *username, const char *applang, const char *version,
			const char *edition, const char *build, const char *licenseStat,
			const char *nsaClientData, uint32_t *transID);


	//Log in user to Nalpeiron server analytics collection
	typedef int(*NSALogin_t)
		(char *username, const char *nsaClientData, uint32_t *transID);

	//Log out user from Nalpeiron server analytics collection
	typedef int(*NSALogout_t)
		(char *username, const char *nsaClientData, uint32_t *transID);



	//Inform Nalpeiron server that use of a specific feature has begun
	typedef int(*NSAFeatureStart_t)
		(char *username, char *featureCode,
			const char *nsaClientData, uint32_t *transID);

	//Inform Nalpeiron server that use of a specific feature has ended 
	typedef int(*NSAFeatureStop_t)
		(char *username, char *featureCode,
			const char *nsaClientData, uint32_t *transID);


	//Inform Nalpeiron server that a local error has occurred
	typedef int(*NSAException_t)
		(char *username, char *exceptionCode, char *description,
			const char *nsaClientData, uint32_t *transID);

	//If any analytics transactions are in queue send them to Nalpeiron
	typedef int(*NSASendCache_t)(char *username, uint32_t *transID);

	// returns the version number as a 3 digit number i.e. 2.7.0 returns 270
	typedef	int(*NSAGetVersion_t)(char **version);

	// returns computer identification string
	typedef int(*NSAGetComputerID_t)(char **computerID);

	//Return the domain name of the activation server
	typedef int(*NSAGetHostName_t)(char **hostName);



#ifdef __cplusplus
}
#endif


// loads/unloads the .dylib or the dll and create linked functions to the library functions
class cpccAppTelemetryNalpeiron: public cpccLinkedLibrary
{
private:

	NalpGetErrorMsg_t		NalpGetErrorMsg_ptr = NULL;
	NSAValidateLibrary_t	NSAValidateLibrary_ptr = NULL;
	NalpLibOpen_t			NalpLibOpen_ptr = NULL;
	NalpLibClose_t			NalpLibClose_ptr = NULL;
	NSAApStart_t			NSAAppStart_ptr = NULL;
	NSAApStop_t				NSAAppStop_ptr = NULL;
	NSASetPrivacy_t			NSASetPrivacy_ptr = NULL;
	NSAFree_t				NSAFree_ptr = NULL;
	NSATestConnection_t		NSATestConnection_ptr = NULL;
	NSASysInfo_t            NSASysInfo_ptr = NULL;
	NSAFeatureStart_t		NSAFeatureStart_ptr = NULL;
	NSAFeatureStop_t		NSAFeatureStop_ptr = NULL;
	std::stringstream		*m_infoDumpPtr = NULL;

public:
    
    static const char *getDefaultLibFilename(void)
    {
    #ifdef _WIN32
        return  "StarMessage-ShaferFilechck.DLL";
    #elif defined(__APPLE__)
        return  "ShaferFilechck.dylib";
    #endif
    }
    
   
    
public:
    
	cpccAppTelemetryNalpeiron(const char *aLibraryfilename, std::stringstream &anErrorStream, std::stringstream *anInfoErrorStream) : 
			cpccLinkedLibrary(aLibraryfilename, anErrorStream ),
			m_infoDumpPtr(anInfoErrorStream) // for debug
	{
		if (m_infoDumpPtr)
			(*m_infoDumpPtr) << "\nInfo: cpccAppTelemetryNalpeiron constructor\n";
        if (!isLoaded())
            errorStream << "nalpeiron library not loaded\n";
	}
    	

public: 

	bool NSAFeatureStart(char *username, char *featureCode, const char *nsaClientData, uint32_t *transID)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSAFeatureStart()\n";

		if (!NSAFeatureStart_ptr)
			if (!(NSAFeatureStart_ptr = (NSAFeatureStart_t)getFunction("NSAFeatureStart")))
			{
				errorStream << "error linking to NSAFeatureStart()\n";
				return false;
			}
		
		int ret = NSAFeatureStart_ptr(username,featureCode,nsaClientData, transID);
		if (ret == 0)
			return true;
		reportErrorMsg("#6822m: error calling NSAFeatureStart()", ret);
		return false;
	}
	
	
	bool NSAFeatureStop(char *username, char *featureCode, const char *nsaClientData, uint32_t *transID)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSAFeatureStop()\n";

		if (!NSAFeatureStop_ptr)
			if (!(NSAFeatureStop_ptr = (NSAFeatureStop_t)getFunction("NSAFeatureStop")))
			{
				errorStream << "error linking to NSAFeatureStop()\n";
				return false;
			}
		
		int ret = NSAFeatureStop_ptr(username, featureCode, nsaClientData, transID);
		if (ret == 0)
			return true;
		reportErrorMsg("#6822n: error calling NSAFeatureStop()", ret);
		return false;
	}
	
	bool	NSA_free(void *memptr)
	{
		/*
		The NSAFree functions is available to free any memory allocated by the NSA library. 
		This function is mandatory on windows as depending on how your binary is built, your 
		final binary may have a different heap than the NSA library. 
		This function is provided to ensure that memory allocated from the NSA heap is freed there. 
		NSAFree should be used on the memory returned from NSAGetStats, ReturnVersion, GetComputerID, 
		InternetHelloWorld, GetHostName, etc..

		*/
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSA_free()\n";

		if (!NSAFree_ptr)
			if (!(NSAFree_ptr = (NSAFree_t)getFunction("NSA_free")))
			{
				errorStream << "error linking to NSA_free()\n";
				return false;
			}

		NSAFree_ptr(memptr);
		return true;
	}


	void	reportErrorMsg(const char * myText, const int errorCode)
	{
		errorStream << myText << "\nNalp ErrorCode:" << errorCode ;

		if (!NalpGetErrorMsg_ptr)
			if (!(NalpGetErrorMsg_ptr = (NalpGetErrorMsg_t)getFunction("NalpGetErrorMsg")))
			{
			errorStream << "error linking to NalpGetErrorMsg()\n";
			return;
			}

		/*
		int NalpGetErrorMsg(int nalpErrorNo, char **errorMsg)
		Get a descriptive string for Nalpeiron error codes.
		nalpErrorno is a negative return value from one of the Nalpeiron functions and errorMsg is a descriptive string explaining that error.
		errorMsg should be freed with NSAFree by the caller.

		error codes:
		http://docs.nalpeiron.com/display/DOCS/NSA+Error+Code+List
		*/
			
		char *msg;
		NalpGetErrorMsg_ptr(errorCode, &msg);
		errorStream << ":" << msg << std::endl;
		NSA_free(msg);
	}


	bool NalpLibOpen(const char * xmlConfiguration)
	{
		//You must always call NalpLibOpen before using the NSA library.  
		// NalpLibOpen initializes necessary structures and kicks off the
		// threads.  To guard against future changes, NalpLibOpen takes
		// a xml string as its parameter.

		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NalpLibOpen()\n";

		// logTimeCountrer _tmpLog("NalpLibOpen()");
		if (!NalpLibOpen_ptr)
			if (!(NalpLibOpen_ptr = (NalpLibOpen_t)getFunction("NalpLibOpen")))
			{
				errorStream << "error linking to NalpLibOpen()\n";
				return false;
			}
		
		int ret = NalpLibOpen_ptr(xmlConfiguration); // Returns 0 for success and a negative number for an error.

		if (ret == 0)
			return true;
		reportErrorMsg("#6822a: error NalpLibOpen_ptr()", ret);
		return false;
	}


	bool NalpLibClose(void)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NalpLibClose()\n";
		/*
		During NSAClose, all data remaining in the transaction pool is moved into the cache thread and
		written to disk as soon as possible.
		NSAClose does not block. That is, when called, it returns immediately.
		The decision was made that all NSA library functions should return immediately while necessary
		processing takes place in thread in the background.
		This means that under certain circumstances, such as a short lived application,
		there is the possibilty of data loss.
		This may occur if the library itself is closed (with a dlclose or similar command)
		before NSAClose actually completes.
         
        Shuts down the library. Returns 0 for success and a negative number for an error.
		*/
		if (!NalpLibClose_ptr)
			if (!(NalpLibClose_ptr = (NalpLibClose_t)getFunction("NalpLibClose")))
			{
				errorStream << "error linking to NalpLibClose()\n";
				return false;
			}

		int ret = NalpLibClose_ptr();	// Returns 0 for success and a negative number for an error
		if (ret == 0)
			return true;
		reportErrorMsg("#6822e: error calling NalpLibClose_ptr()", ret);
		return false;
	}
	

	bool NSAAppStart(const char *nsaClientData, uint32_t *transID)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSAAppStart()\n";

		if (!NSAAppStart_ptr)
			if (!(NSAAppStart_ptr = (NSAApStart_t)getFunction("NSAAppStart")))
			{
				errorStream << "error linking to NSAAppStart()\n";
				return false;
			}
		
		// Returns 0 on success and a negative number for an error.
		// if I do not call NSASetPrivacy_ptr(0) before, I get the error: Can't complete do to privacy set
		int ret = NSAAppStart_ptr(nsaClientData, transID);
		if (ret == 0)
			return true;
		reportErrorMsg("#6822c: error calling NSAAppStart_ptr()", ret);
		return false;
	}


	bool NSAAppStop(const char *nsaClientData, uint32_t *transID)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSAAppStop()\n";

		if (!NSAAppStop_ptr)
			if (!(NSAAppStop_ptr = (NSAApStop_t)getFunction("NSAAppStop")))
			{
				errorStream << "error linking to NSAAppStop()\n";
				return false;
			}
		
		// Records shutdown of your application. Returns 0 on success and a negative number for an error.
		int ret = NSAAppStop_ptr(nsaClientData, transID);
		if (ret == 0)
			return true;
		reportErrorMsg("#6822d: error calling NalpLibClose_ptr()", ret);
		return false;
	}


	
	
	bool NSA_validateLibrary(const char* customerID, const char* productID)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSA_validateLibrary()\n";

		// logTimeCountrer _tmpLog("NSAValidateLibrary()");
		if (!NSAValidateLibrary_ptr)
			if (! (NSAValidateLibrary_ptr = (NSAValidateLibrary_t)getFunction("NSAValidateLibrary")))
			{
				errorStream << "error linking to NSAValidateLibrary()\n";
				return false;
			}


		int cid = (uint32_t)strtoul(customerID, NULL, 10);
		int pid = (long)strtoul(productID, NULL, 10);
        /*
        Must be called after NalpLibOpen()
        int NSAValidateLibrary(uint32_t customerID, uint32_t productID)
        Verifies that the shared library you are accessing is the library you stamped.
        It does this by checking the customerID and productID sent in against the stamped values.
        If they match, 0 is returned
        */
        int ret = NSAValidateLibrary_ptr(cid, pid);
        if (ret == 0)
            return true;
        reportErrorMsg("#6822b: error validating nap library", ret);
		return false;
	}


	bool NSAdisablePrivacy(void)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSAdisablePrivacy()\n";

		if (!NSASetPrivacy_ptr)
			if (! (NSASetPrivacy_ptr = (NSASetPrivacy_t)getFunction("NSASetPrivacy")))
			{
				errorStream << "error linking to NSASetPrivacy()\n";
				return false;
			}

		int ret = NSASetPrivacy_ptr(0); // return values are 0 "no privacy", 1 "privacy enabled" or a negative number for an error.
		if (ret<0)
		{
			reportErrorMsg("#6822c: error calling NSAAppStart_ptr()", ret);
			return false;
		}
		return true;
	}


	bool NSATestConnection(const char *param, uint32_t *transID)
	{

		if (!NSATestConnection_ptr)
			if (!(NSATestConnection_ptr = (NSATestConnection_t)getFunction("NSATestConnection")))
			{
				errorStream << "error linking to NSATestConnection()\n";
				return false;
			}
		
		// Connects to Nalpeiron and retrieves a "Hello World". Used only for testing the library and connectivity.
		// Returns 0 on success and a negative number for an error.
		// It seems that NSATestConnection() may fail if it is called before disabling privacy: NSASetPrivacy(0)
		int ret = NSATestConnection_ptr(NULL, transID);
		if (ret == 0)
			return true;
		reportErrorMsg("#6822r: NSATestConnection() failed", ret);
		return false;
	}


	bool NSASysInfo(const char *username, const char *applang, const char *version,
					const char *edition, const char *build, const char *licenseStat,
					const char *nsaClientData, uint32_t *transID)
	{
		if (m_infoDumpPtr)
			*m_infoDumpPtr << "\nInfo: NSASysInfo()\n";

		if (!NSASysInfo_ptr)
			if (!(NSASysInfo_ptr = (NSASysInfo_t)getFunction("NSASysInfo")))
			{
				errorStream << "error linking to NSASysInfo()\n";
				return false;
			}

		int ret = NSASysInfo_ptr(username, applang, version, edition, build, licenseStat, nsaClientData, transID);
		if (ret == 0)
			return true;
		reportErrorMsg("#6822k: error calling NSASysInfo()", ret);
		return false;
	}

};



// API old:	http://docs.nalpeiron.com/pages/viewpage.action?pageId=524341
// API:		http://docs.nalpeiron.com/pages/viewpage.action?pageId=524314
/*
	ToDo: include the functions for the llogging of the app features
	int NSAFeatureStart( char *username, char *featureCode)	
	Records start of use of featureCode by username. Returns 0 on success and a negative number for an error. 
	Username and featureCode should be a ANSI C strings or a utf8 encoded unicode strings.

	int NSAFeatureStop( char *username, char *featureCode)	
	Records end of use of featureCode by username. Returns 0 on success and a negative number for an error. 
	Username and featureCode should be a ANSI C strings or a utf8 encoded unicode strings.
	
	 In the NSA library, if any of the calls that connect to the Nalpeiron server fails for any reason, 
	 the are cached to disk. In your WorkDir you will see a file with the extension .cache. 
	 All failed calls are stored there. These calls will be sent to Nalpeiron automatically by the library 
	 at a later time or with the NSASendCache call.

	If you'd like to immediately test the connection and bypass the cache file, you should use the 
	NSLTestConnection call. It will return an error message immediately if the connection fails.
	
	This is the NSL (not NSA) api
	http://docs.nalpeiron.com/display/NND/NSL+V10+Developers+API
*/


