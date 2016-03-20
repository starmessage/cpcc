
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



// loads/unloads the .dylib or the dll and and links the functions to functionPointers
class LinkedLibrary_nalpeiron: public cpccLinkedLibrary
{
public:
    
    static const char *getDefaultLibFilename(void)
    {
    #ifdef _WIN32
        return  "StarMessage-ShaferFilechck.DLL";
    #elif defined(__APPLE__)
        return  "ShaferFilechck.dylib";
    #endif
    }
    
    
public:	// functions
	
	typedef int(*NalpGetErrorMsg_t)(int nalpErrno, char **errMsg);
	typedef	int(*NSAValidateLibrary_t)(uint32_t custNum, uint32_t prodNum);

	//Routine to inform Nalpeiron server that application has started
	typedef int(*NSAAppStart_t)
		(const char *nsaClientData, uint32_t *transID);

	//Routine to inform Nalpeiron server that application has ended
	typedef int(*NSAApStop_t)
		(const char *nsaClientData, uint32_t *transID);
	
	typedef int(*NalpLibOpen_t)(const char *xmlParams);
	typedef int(*NalpLibClose_t)(void);
	typedef int(*NSASetPrivacy_t)(unsigned short nsaPriv);
	typedef int(*NSAFree_t)(void *memptr);
	
	typedef int(*NSATestConnection_t) (const char *nsaClientData, uint32_t *transID); //Checks server connectivity
    
    //Send system information from end user's computer to Nalpeiron
    typedef int (*NSASysInfo_t)
            (   const char *username, const char *applang, const char *version,
                const char *edition, const char *build, const char *licenseStat,
                const char *nsaClientData, uint32_t *transID);
    

	NalpGetErrorMsg_t		NalpGetErrorMsg_ptr = NULL;
	NSAValidateLibrary_t	NSAValidateLibrary_ptr = NULL;
	NalpLibOpen_t			NalpLibOpen_ptr = NULL; 
	NalpLibClose_t			NalpLibClose_ptr = NULL;	// Shuts down the library. Returns 0 for success and a negative number for an error.
	NSAAppStart_t			NSAAppStart_ptr= NULL;
	NSAApStop_t				NSAAppStop_ptr = NULL;
	NSASetPrivacy_t			NSASetPrivacy_ptr = NULL;
	NSAFree_t				NSAFree_ptr = NULL;
	NSATestConnection_t		NSATestConnection_ptr = NULL;
    NSASysInfo_t            NSASysInfo_ptr = NULL;
    
public:
    
	LinkedLibrary_nalpeiron(const char *aLibraryfilename) : cpccLinkedLibrary(aLibraryfilename)
	{
		if (isLoaded())
			linkFunctions();
	}
    

private:
    
	bool	linkFunctions(void)
	{
		bool errorsFound = false;

		if (!(NSAValidateLibrary_ptr = (NSAValidateLibrary_t)getFunction("NSAValidateLibrary")))
		{
			errorDump << "error linking to NSAValidateLibrary()\n";
			errorsFound = true;
		}

		if (!(NalpGetErrorMsg_ptr = (NalpGetErrorMsg_t)getFunction("NalpGetErrorMsg")))
		{
			errorDump << "error linking to NalpGetErrorMsg()\n";
			errorsFound = true;
		}

		if (!(NalpLibOpen_ptr = (NalpLibOpen_t)getFunction("NalpLibOpen")))
		{
			errorDump << "error linking to NalpLibOpen()\n";
			errorsFound = true;
		}

		if (!(NSAAppStart_ptr = (NSAAppStart_t)getFunction("NSAAppStart")))
		{
			errorDump << "error linking to NSAAppStart()\n";
			errorsFound = true;
		}

		if (!(NSAAppStop_ptr = (NSAApStop_t)getFunction("NSAAppStop")))
		{
			errorDump << "error linking to NSAAppStop()\n";
			errorsFound = true;
		}

		if (!(NalpLibClose_ptr = (NalpLibClose_t)getFunction("NalpLibClose")))
		{
			errorDump << "error linking to NalpLibClose()\n";
			errorsFound = true;
		}

		if (!(NSASetPrivacy_ptr = (NSASetPrivacy_t)getFunction("NSASetPrivacy")))
		{
			errorDump << "error linking to NSASetPrivacy()\n";
			errorsFound = true;
		}

		if (!(NSAFree_ptr = (NSAFree_t)getFunction("NSAFree")))
		{
			errorDump << "error linking to NSAFree()\n";
			errorsFound = true;
		}

        if (!(NSATestConnection_ptr = (NSATestConnection_t)getFunction("NSATestConnection")))
        {
            errorDump << "error linking to NSATestConnection()\n";
            errorsFound = true;
        }
        
        if (!(NSASysInfo_ptr = (NSASysInfo_t)getFunction("NSASysInfo")))
        {
            errorDump << "error linking to NSASysInfo()\n";
            errorsFound = true;
        }
        
        return !errorsFound;
	}

protected: 
	void	reportErrorMsg(const char * myText, const int errorCode)
	{
		errorDump << myText << "\nNalp ErrorCode:" << errorCode ;

		/*
		int NalpGetErrorMsg(int nalpErrorNo, char **errorMsg)
		Get a descriptive string for Nalpeiron error codes.
		nalpErrorno is a negative return value from one of the Nalpeiron functions and errorMsg is a descriptive string explaining that error.
		errorMsg should be freed with NSAFree by the caller.
		*/
		if (!NalpGetErrorMsg_ptr)
		{
			errorDump << "\nNalpGetErrorMsg_ptr is NULL\n";
			return;
		}
			
		char *msg;
		NalpGetErrorMsg_ptr(errorCode, &msg);
		errorDump << ":" << msg << std::endl;

		if (NSAFree_ptr)
			NSAFree_ptr(msg);
	}
	
};



// API old:	http://docs.nalpeiron.com/pages/viewpage.action?pageId=524341
// API:		http://docs.nalpeiron.com/pages/viewpage.action?pageId=524314
/*
	ToDo: include the functions logging the app features
	int NSAFeatureStart( char *username, char *featureCode)	
	Records start of use of featureCode by username. Returns 0 on success and a negative number for an error. 
	Username and featureCode should be a ANSI C strings or a utf8 encoded unicode strings.

	int NSAFeatureStop( char *username, char *featureCode)	
	Records end of use of featureCode by username. Returns 0 on success and a negative number for an error. 
	Username and featureCode should be a ANSI C strings or a utf8 encoded unicode strings.
*/
class cpccAppTelemetryNalpeiron: public LinkedLibrary_nalpeiron
{
private:
    uint32_t		m_transID = 0;	// To retrieve a transaction ID from any function, set transID = 0 and call the function. 
									// Upon return, transID will be set with a random number that will be passed to the Nalpeiron server to identify the transaction.  
									// Send this value into any functions that are to be grouped together.
	
    bool            m_appWasStarted = false;
    bool            m_libWasOpened = false;
	enum			eNalConfig 
						{ 
						config_DoLibValidation=false,
						config_DoConnectionTest=false
						};

	
	bool openLib(void)
	{
		// logTimeCountrer _tmpLog("NalpLibOpen()");
		
		if (NalpLibOpen_ptr)
		{
			const char * xmlConfiguration =
				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
				"<SHAFERXMLParams>"
				"<NSAEnabled>1</NSAEnabled>"
				"<NSLEnabled>0</NSLEnabled>"
				"<LogLevel>1</LogLevel>"
				"<SecurityValue>0</SecurityValue>"
				"</SHAFERXMLParams>";
			
			int ret = NalpLibOpen_ptr(xmlConfiguration); // Returns 0 for success and a negative number for an error.
			if (ret == 0) 
				return true;
			reportErrorMsg("#6822a: error NalpLibOpen_ptr()", ret);
		}
		else
			errorDump << "NULL NalpLibOpen_ptr";

		return false;
	}
	
	bool libClose(void)
	{
		if (!NalpLibClose_ptr)
		{
			errorDump << "NULL NalpLibClose_ptr";
			return false;
		}

		int ret = NalpLibClose_ptr();	// Returns 0 for success and a negative number for an error
        if (ret == 0)
			return true;
        reportErrorMsg("#6822e: error calling NalpLibClose_ptr()", ret);
		return false;
	}
	
	
	bool appStart(const char *nsaClientData, uint32_t *transID)
	{
		if (NSAAppStart_ptr)
        {
			// Returns 0 on success and a negative number for an error.
            // if I do not call NSASetPrivacy_ptr(0) before, I get the error: Can't complete do to privacy set
            int ret = NSAAppStart_ptr((char *)"my custom data", &m_transID);
            if (ret == 0)
				return true;
            reportErrorMsg("#6822c: error calling NSAAppStart_ptr()",ret);
        }
		else
			errorDump << "NULL NSAAppStart_ptr";

		return false;
	}
	
	bool appStop(const char *nsaClientData, uint32_t *transID)
	{
		if (NSAAppStop_ptr)
        {
			// Records shutdown of your application. Returns 0 on success and a negative number for an error.
            int ret = NSAAppStop_ptr(NULL, &m_transID); 
            if (ret == 0) 
				return true;
            reportErrorMsg("#6822d: error calling NalpLibClose_ptr()", ret);
        }
		else
			errorDump << "NULL NSAAppStop_ptr";

		return false;
	}
	
	
	bool validateLibrary(const char* customerID, const char* productID)
	{
		// logTimeCountrer _tmpLog("NSAValidateLibrary()");
		int cid = (uint32_t)strtoul(customerID, NULL, 10);
        int pid = (long)strtoul(productID, NULL, 10);
		if (NSAValidateLibrary_ptr)
		{
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
		}
		else
			errorDump << "NULL NSAValidateLibrary_ptr";
		return false;
	}
	
	bool disablePrivacy(void)
	{
		if (!NSASetPrivacy_ptr)
		{
			errorDump << "NULL NSASetPrivacy_ptr";
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
	
	bool connectionTest(uint32_t *param)
	{
	    if (NSATestConnection_ptr)
        {
            // Connects to Nalpeiron and retrieves a "Hello World". Used only for testing the library and connectivity.
            // Returns 0 on success and a negative number for an error.
			// It seems that NSATestConnection() may fail if it is called before disabling privacy: NSASetPrivacy(0)
            int ret = NSATestConnection_ptr(NULL, &m_transID);
			if (ret==0)
				return true;
            reportErrorMsg("#6822r: NSATestConnection() failed", ret);
        }
		else 
			errorDump << "NULL NSATestConnection_ptr";

		return false;
	}
	
public:
	cpccAppTelemetryNalpeiron(const char* nalDynLibraryFilename,
				const char* customerID,
				const char* productID,
				const char* productVersion,
				const char* productBuildNumber,
				const char* productEdition,
				const char* productLicense) :
		LinkedLibrary_nalpeiron(nalDynLibraryFilename)
	{
		if (!isLoaded())
		{
			errorDump << "nalpeiron not loaded\n";
			return;
		}
			

		m_libWasOpened = openLib();
        
		if (config_DoLibValidation)
			if (!validateLibrary(customerID, productID))
				return;

		disablePrivacy();

		if (config_DoConnectionTest)
			connectionTest(&m_transID);

		m_appWasStarted = appStart((char *)"my custom data", &m_transID);
		        		                
        if (NSASysInfo_ptr)
        {
            int ret = NSASysInfo_ptr( "anonymous", "EN", productVersion, productEdition, productBuildNumber, productLicense, "nsaClientData", &m_transID);
            if (ret<0)
                reportErrorMsg("#6822k: error calling NSASysInfo()", ret);
        }

        
    }
	
	
	~cpccAppTelemetryNalpeiron(void) override
	{
		if (m_appWasStarted)
			appStop((char *)"my custom data", &m_transID);
                
		if (m_libWasOpened)
			libClose();
	}


};
