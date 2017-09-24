/*  *****************************************
 *  File:		net.cpccWinHttp_helpers.h
 *	Purpose:	Portable (cross-platform), WinHttp wrapper classes
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <windows.h>
#include <WinHttp.h>
#include <assert.h>

#include "../cpccUnicodeSupport.h"
#include "../core.cpccOS.h"
#include "../io.cpccLog.h"


static bool stringStartsWith(const char *aStr, const char *aPrefix)
{
	#pragma warning( suppress : 4996 )
	return (strnicmp(aStr, aPrefix, strlen(aPrefix)) == 0);
}


/*	
debugging with WinHttpTraceCfg.exe, a Trace Configuration Tool
https://msdn.microsoft.com/en-us/library/windows/desktop/aa384119(v=vs.85).aspx

to check this option

WINHTTP_OPTION_SECURE_PROTOCOLS

Sets an unsigned long integer value that specifies which secure protocols are acceptable. 
By default only SSL3 and TLS1 are enabled in Windows 7 and Windows 8. 
By default only SSL3, TLS1.0, TLS1.1, and TLS1.2 are enabled in Windows 8.1 and Windows 10.
The value can be a combination of one or more of the following values.

WINHTTP_FLAG_SECURE_PROTOCOL_ALL

The Secure Sockets Layer (SSL) 2.0, SSL 3.0, and Transport Layer Security (TLS) 1.0 protocols can be used.
WINHTTP_FLAG_SECURE_PROTOCOL_SSL2

The SSL 2.0 protocol can be used.
WINHTTP_FLAG_SECURE_PROTOCOL_SSL3

The SSL 3.0 protocol can be used.
WINHTTP_FLAG_SECURE_PROTOCOL_TLS1

The TLS 1.0 protocol can be used.
WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1

The TLS 1.1 protocol can be used.
WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2

The TLS 1.2 protocol can be used.


*/


 /* WinHttp overview of code

 This is a draft of the code:

 hSession = WinHttpOpen( L"Agent/1.0",..
 hConnect = WinHttpConnect(hSession,..
 // You can keep issuing as many requests as you want against the same connection on the same session.
 for (all_files_to_upload)
 {
	 hRequest = WinHttpOpenRequest( hConnect, L"PUT",..
	 WinHttpSetCredentials(hRequest,..
	 WinHttpAddRequestHeaders( hRequest,..
	 WinHttpSendRequest( hRequest,..
	 WinHttpWriteData(hRequest,..
	 WinHttpReceiveResponse(hRequest,..
	 WinHttpQueryHeaders(hRequest,..
	 WinHttpCloseHandle(hRequest);
 }

 if (hConnect) WinHttpCloseHandle(hConnect);
 if (hSession) WinHttpCloseHandle(hSession);

 */

 
 ///////////////////////////////////////////////////////////
 //
 //
 //		class cWinHttp_handle
 //
 //
 ///////////////////////////////////////////////////////////
 class cWinHttp_handle
{
private:
	HINTERNET m_handle;

protected:
	bool m_isGood;

public:

    cWinHttp_handle() : m_handle(0), m_isGood(true) { }

    virtual ~cWinHttp_handle() { close(); }

	HINTERNET getHandle(void) const { return m_handle; }
	
protected:

    bool attach(HINTERNET handle)
    {
		assert((0 == m_handle) && "#5723: cWinHttp_handle.attach() called to replace a non NULL handle");
        m_handle = handle;
        return 0 != m_handle;
    }

    HINTERNET detach()
    {
		HINTERNET handle = m_handle;
        m_handle = 0;
        return handle;
    }

    void close()
    {
        if (0 != m_handle)
        {   
            ::WinHttpCloseHandle(m_handle);
            m_handle = 0;
        }
    }

	

public:
	
	bool isGood(void) const { return m_isGood && (m_handle !=0); }

	HRESULT setOption(DWORD option, const void* value, DWORD length)
    {
        if (::WinHttpSetOption(m_handle, option, const_cast<void*>(value), length))      
			return S_OK;
		return HRESULT_FROM_WIN32(::GetLastError());
    }

    HRESULT queryOption(DWORD option, void* value, DWORD& length) const
    {
        if (::WinHttpQueryOption(m_handle, option, value, &length))
			return S_OK; 
		return HRESULT_FROM_WIN32(::GetLastError());  
    }

};

 
 ///////////////////////////////////////////////////////////
 //
 //
 //		class  cWinHttp_session
 //
 //
 ///////////////////////////////////////////////////////////


 class cWinHttp_session : public cWinHttp_handle
 {

 public:
	 explicit cWinHttp_session(const LPCWSTR aUserAgent, const bool runAsync = false)
	 {
		 DWORD sessionFlags = 0;
		 if (runAsync)
			 sessionFlags |= WINHTTP_FLAG_ASYNC;

		 attach( WinHttpOpen(aUserAgent, // optional
						WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
						WINHTTP_NO_PROXY_NAME,
						WINHTTP_NO_PROXY_BYPASS,
						sessionFlags)
				);
	 }

 };


 ///////////////////////////////////////////////////////////
 //
 //
 //		class cWinHttp_connection
 //
 //
 ///////////////////////////////////////////////////////////


 class cWinHttp_connection : public cWinHttp_handle
 {

 public:
	 explicit cWinHttp_connection(const HINTERNET aSessionHandler, const LPCWSTR aServerAddress)
	 {
		 /*
		 WinHttpConnect takes only the hostname part.  E.g. www.google.com
		 It must not contain the protocol https:// or http://
		 You should pass the rest of the path to WinHttpOpenRequest
		 
		 INTERNET_DEFAULT_HTTPS_PORT
		 Uses the default port for HTTPS servers (port 443).
		 Selecting this port does not automatically establish a secure connection.
		 You must still specify the use of secure transaction semantics by using the
		 WINHTTP_FLAG_SECURE flag with WinHttpOpenRequest.
		 */
		 attach(WinHttpConnect(aSessionHandler, aServerAddress, INTERNET_DEFAULT_PORT, 0));
	 }

 };




///////////////////////////////////////////////////////////
//
//
//		class cWinHttp_request
//
//
///////////////////////////////////////////////////////////

class cWinHttp_request : public cWinHttp_handle
{

public:
	explicit cWinHttp_request(const HINTERNET aConnectionHandler, const bool isHTTPS, const LPCWSTR aURLpath, const int aTimeout)
	{
		DWORD flags = WINHTTP_FLAG_REFRESH;
		if (isHTTPS)
			flags |= WINHTTP_FLAG_SECURE;
			
		// you need to do WinHttpOpenRequest for every resource request.
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384099(v=vs.85).aspx
		attach(  WinHttpOpenRequest(aConnectionHandler, 
										L"POST", // L"GET", // 
										aURLpath,
										NULL, // use HTTP version 1.1
										WINHTTP_NO_REFERER, 
										WINHTTP_DEFAULT_ACCEPT_TYPES, 
										flags));
		if (!getHandle())
		{
			DWORD errNo = GetLastError();
			std::string errormsg(cpccOS::getWindowsErrorCodeAndText("WinHttpOpenRequest", errNo));
			std::cerr << errormsg;
			errorLog().add(errormsg.c_str());
		}

		/*
		If the application uses the WinHttpSetTimeouts function or the SetTimeouts method on the WinHttpRequest 
		component to set a non-infinite DNS resolve timeout such as the dwResolveTimeout parameter, a thread 
		handle leak occurs each time WinHTTP resolves a DNS name. 
		Over a large number of HTTP requests, this causes a significant memory leak. 
		The workaround is to leave the default infinite resolve timeout setting unchanged (a value of 0 specifies 
		an infinite timeout). 
		This is strongly recommended in any case as supporting timeouts on DNS name resolutions in WinHTTP is 
		expensive in terms of performance. For Windows 2000 and later, setting a DNS resolve timeout in WinHTTP 
		is unnecessary as the underlying DNS client service implements its own resolve timeout.
		*/
		// all these times in msec
		int dwResolveTimeout	=500,  // used only for the calculations below. Finally, I will enter 0 (infinite wait)
			dwConnectTimeout	=500,
			dwSendTimeout		=1000 * aTimeout - dwResolveTimeout - dwConnectTimeout,
			dwReceiveTimeout	= dwSendTimeout;
		
		dwResolveTimeout = 0;
		
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384116(v=vs.85).aspx
		infoLog().addf("Set winhttp timeout of %i sec.", aTimeout);
		BOOL result = WinHttpSetTimeouts(getHandle(), dwResolveTimeout, dwConnectTimeout, dwSendTimeout, dwReceiveTimeout );
		if (result == FALSE)
		{
			DWORD errNo = GetLastError();
			std::string errormsg(cpccOS::getWindowsErrorCodeAndText("WinHttpSetTimeouts", errNo));
			std::cerr << errormsg;
			errorLog().add(errormsg.c_str());
		}
		
	}



};

