
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


// API
// http://docs.nalpeiron.com/pages/viewpage.action?pageId=524314

class dynLib_nalpeiron: public cpccLinkedLibrary
{
private:
	unsigned int	transIDA = 0;		//application

public:	// functions
	
	typedef int(*NalpGetErrorMsg_t)(int nalpErrno, char **errMsg);
	typedef	int(*NSAValidateLibrary_t)(uint32_t custNum, uint32_t prodNum);

	//Routine to inform Nalpeiron server that application has started
	typedef int(*NSAApStart_t)
		(const char *nsaClientData, uint32_t *transID);

	//Routine to inform Nalpeiron server that application has ended
	typedef int(*NSAApStop_t)
		(const char *nsaClientData, uint32_t *transID);
	
	typedef int(*NalpLibOpen_t)(const char *xmlParams);
	typedef int(*NalpLibClose_t)(void);
	typedef int(*NSASetPrivacy_t)(unsigned short nsaPriv);
	typedef int(*NSAFree_t)(void *memptr);

	NalpGetErrorMsg_t		NalpGetErrorMsg_ptr = NULL;
	NSAValidateLibrary_t	NSAValidateLibrary_ptr = NULL;
	NalpLibOpen_t			NalpLibOpen_ptr = NULL; 
	NalpLibClose_t			NalpLibClose_ptr = NULL;	// Shuts down the library.  Returns 0 for success and a negative number for an error.
	NSAApStart_t			NSAApStart_ptr= NULL;
	NSAApStop_t				NSAApStop_ptr = NULL;
	NSASetPrivacy_t			NSASetPrivacy_ptr = NULL;
	NSAFree_t				NSAFree_ptr = NULL;

public:
    
	dynLib_nalpeiron(const char *aLibraryfilename, const char *aCustomerID, const char *aProductID, const char *xmlConfiguration) : cpccLinkedLibrary(aLibraryfilename)
	{
		if (!isLoaded())
			return;
		
		if (!linkFunctions())
			return;

		int ret;
		

		// Returns 0 for success and a negative number for an error.  
		if (!NalpLibOpen_ptr) return;
		ret = NalpLibOpen_ptr(xmlConfiguration);
		if (ret!=0)
		{
			errorDump << "#6822: error NalpLibOpen_ptr()" << std::endl;
			reportErrorMsg(ret);
			return;
		}

		int cid = (uint32_t)strtoul(aCustomerID, NULL, 10);
		int pid = (long)strtoul(aProductID, NULL, 10);
		if (NSAValidateLibrary_ptr)
			if ((ret = NSAValidateLibrary_ptr(cid, pid)) != 0 )
				/*
				Must be called after NalpLibOpen()
				int NSAValidateLibrary(uint32_t customerID, uint32_t productID)
				Verifies that the shared library you are accessing is the library you stamped.
				It does this by checking the customerID and productID sent in against the stamped values.
				If they match, 0 is returned
				*/
			{
				errorDump << "#5934: error validating nap library" << std::endl;
				reportErrorMsg(ret);
				return;
			}
		
		// http://docs.nalpeiron.com/pages/viewpage.action?pageId=524341
		if (!NSASetPrivacy_ptr) return;
		NSASetPrivacy_ptr(1);


		if (!NSAApStart_ptr) return;
		ret = NSAApStart_ptr((char *)"my custom data", &transIDA);
		// Returns 0 on success and a negative number for an error.
		if (ret != 0)
		{	// I get the error: Can't complete do to privacy set
			errorDump << "#6592: error calling NSAApStart_ptr()" << std::endl;
			reportErrorMsg(ret);
			return;
		}
	}

	~dynLib_nalpeiron(void) override
	{
		if (NSAApStop_ptr)
			NSAApStop_ptr((const char *)getLibHandle(), &transIDA);

		if (NalpLibClose_ptr)
			NalpLibClose_ptr();
	}


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

		if (!(NSAApStart_ptr = (NSAApStart_t)getFunction("NSAAppStart")))
		{
			errorDump << "error linking to NSAAppStart()\n";
			errorsFound = true;
		}

		if (!(NSAApStop_ptr = (NSAApStop_t)getFunction("NSAAppStop")))
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

        return errorsFound;
	}

private:

	void	reportErrorMsg(const int errorCode)
	{
		/*
		int NalpGetErrorMsg(int nalpErrorNo, char **errorMsg)
		Get a descriptive string for Nalpeiron error codes.
		nalpErrorno is a negative return value from one of the Nalpeiron functions and errorMsg is a descriptive string explaining that error.
		errorMsg should be freed with NSAFree by the caller.
		*/
		if (!NalpGetErrorMsg_ptr)
		{
			errorDump << "NalpGetErrorMsg_ptr is NULL\n";
			return;
		}
			
		char *msg;
		NalpGetErrorMsg_ptr(errorCode, &msg);
		errorDump << msg << std::endl;

		if (NSAFree_ptr)
			NSAFree_ptr(msg);
	}
	
};


#ifdef _WIN32
    #define napLibFilename  "StarMessage-ShaferFilechck.DLL"
#elif defined(__APPLE__)
    #define napLibFilename  "ShaferFilechck.dylib"
#endif


class cpccAppTelemetryNalpeiron
{
private:
    dynLib_nalpeiron    *m_dynLibPtr;

public:
	cpccAppTelemetryNalpeiron(const char*  customerID,
								const char* productID,
								const char* productVersion,
								const char* productBuildNumber,
								const char* productEdition) 
    {
        // 	http://docs.nalpeiron.com/pages/viewpage.action?pageId=524341
		const char * parameterTemplate = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<SHAFERXMLParams>"
			"<NSAEnabled>1</NSAEnabled>"
			"<NSLEnabled>0</NSLEnabled>"
			"<SecurityValue>0</SecurityValue>"
			"</SHAFERXMLParams>";
		/*
		std::string tmpString = customerID;
		long int cid = atoi(tmpString.c_str());
		tmpString = productID;
		long int pid = atoi(tmpString.c_str());
		*/
        m_dynLibPtr = new dynLib_nalpeiron(napLibFilename, customerID, productID, parameterTemplate);
    }
	
	
	~cpccAppTelemetryNalpeiron(void)
	{
		if (m_dynLibPtr)
            delete m_dynLibPtr;
        m_dynLibPtr = NULL;
	}

	bool		 hasErrors(void) const		{ return (m_dynLibPtr) ? m_dynLibPtr->hasErrors() : false; };
	const char * getErrorText(void) const	{ return  (m_dynLibPtr) ? m_dynLibPtr->getErrorText() : ""; }
	void		 clearErrors(void)		
	{ 
		if (m_dynLibPtr)
			m_dynLibPtr->clearErrors();
	}
};
