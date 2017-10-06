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




/*	
debugging with WinHttpTraceCfg.exe, a Trace Configuration Tool
https://msdn.microsoft.com/en-us/library/windows/desktop/aa384119(v=vs.85).aspx

Secure Socket Layer (SSL) and its successor Transport Layer Security (TLS) are protocols which use cryptographic algorithms to secure the communication between 2 entities.
Old protocol versions, including SSL version 3 (“SSLv3”) and TLS version 1.0, are no longer considered secure.

SSL 1.0, 2.0 and 3.0
TLS 1.0 (or SSL 3.1, released in 1999)
TLS 1.1 (or SSL 3.2, released in 2006)
TLS 1.2 (or SSL 3.3, released in 2008)



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
	bool m_hasCallback;

public:

    cWinHttp_handle() : m_handle(0), m_isGood(true), m_hasCallback(false) { }

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
		removeStatusCallback();
        if (0 != m_handle)
        {
			if (::WinHttpCloseHandle(m_handle) == FALSE)
			{
				DWORD errNo = GetLastError();
				errorLog().add(cpccOS::getWindowsErrorCodeAndText("WinHttpOpenRequest", errNo));
			}
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


	BOOL SetStatusCallback(WINHTTP_STATUS_CALLBACK aCallback)
	{
		// Install the status callback function.
		WINHTTP_STATUS_CALLBACK pCallback = WinHttpSetStatusCallback(m_handle, aCallback,
			// WINHTTP_CALLBACK_STATUS_REQUEST_SENT |
			// WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED |
			WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE |
			WINHTTP_CALLBACK_STATUS_REQUEST_ERROR |
			WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE 

			//| WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS 
			//| WINHTTP_CALLBACK_FLAG_REDIRECT 
			//|  WINHTTP_CALLBACK_FLAG_CLOSE_CONNECTION 
			//| WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS
			, NULL);
		/*
		Other status flags:
		WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE |
		WINHTTP_CALLBACK_STATUS_REQUEST_ERROR |
		WINHTTP_CALLBACK_STATUS_REDIRECT |
		WINHTTP_CALLBACK_STATUS_SECURE_FAILURE |
		WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE |
		WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION |
		WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED,

		If successful, returns a pointer to the previously defined status callback function or NULL
		if there was no previously defined status callback function.
		Returns WINHTTP_INVALID_STATUS_CALLBACK if the callback function could not be installed.
		*/
		if (pCallback != NULL)
		{
			errorLog().add("#4961: expected NULL after WinHttpSetStatusCallback()");
			if (pCallback == WINHTTP_INVALID_STATUS_CALLBACK)
			{
				DWORD errNo = GetLastError();
				std::string errormsg(cpccOS::getWindowsErrorCodeAndText("WinHttpSetStatusCallback", errNo));
				std::cerr << errormsg;
				errorLog().add(errormsg.c_str());
			}
			return FALSE;
		}
		m_hasCallback = true;
		infoLog().add("callback function added.");
		return TRUE;
	}


	void removeStatusCallback(void)
	{
		if (!m_hasCallback)
		{
			infoLog().add("removeStatusCallback() but no callback installed.");
			return;
		}
		/* At the end of asynchronous processing, the application may set the callback function to NULL.
		This prevents the client application from receiving additional notifications.
		*/
		WinHttpSetStatusCallback(m_handle, NULL, WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, NULL);
		m_hasCallback = false;
		infoLog().add("callback function removed.");
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
		 {
			 sessionFlags |= WINHTTP_FLAG_ASYNC;
			 infoLog().add("cWinHttp_session created with Async flag");
		 }
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


	BOOL sendRequest(const LPSTR postDataBuffer, const DWORD postDataBufferSize, DWORD_PTR aContextPtr)
	{
		// I have found that for PHP to recognise the POSTed data, I had to also do this:
		LPCWSTR additionalHeaders = L"Content-Type: application/x-www-form-urlencoded";
		DWORD additionalHeadersLen = -1;
		// infoLog().addf("payload=%s", postDataBuffer);
		BOOL result = ::WinHttpSendRequest(getHandle(),
								additionalHeaders,
								additionalHeadersLen,
								postDataBuffer, // This buffer must remain available until the request handle is closed or the call to WinHttpReceiveResponse has completed.
								postDataBufferSize,
								postDataBufferSize,
								aContextPtr);
		if (result == FALSE)
		{
			std::string errormsg(cpccOS::getWindowsErrorCodeAndText("WinHttpReceiveResponse", GetLastError()));
			errorLog().add(errormsg);
		}
		return result;
	}


	BOOL ReceiveResponse(void)
	{
		if (WinHttpReceiveResponse(getHandle(), NULL) == FALSE)
		{
			DWORD errNo = GetLastError();
			errorLog().add(cpccOS::getWindowsErrorCodeAndText("WinHttpReceiveResponse", errNo));
			return FALSE;
		}
		return TRUE;
	}


	BOOL QueryHeaders_statuscode(DWORD *statusCode)
	{
		if (!statusCode)
			return FALSE;

		DWORD dwSize = sizeof(DWORD);

		// WinHttpReceiveResponse must have been called for this handle and have completed before WinHttpQueryHeaders is called.
		BOOL result = WinHttpQueryHeaders(getHandle(),
											WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
											WINHTTP_HEADER_NAME_BY_INDEX,
											statusCode, &dwSize, 
											WINHTTP_NO_HEADER_INDEX);

		if (result == FALSE)
		{
			DWORD errNo = GetLastError();
			std::string errormsg(cpccOS::getWindowsErrorCodeAndText("WinHttpQueryHeaders", errNo));
			errorLog().add(errormsg);
		}
		return result;
	}
};


