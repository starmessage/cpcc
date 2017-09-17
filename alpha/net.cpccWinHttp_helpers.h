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


static bool stringStartsWith(const char *aStr, const char *aPrefix)
{
	#pragma warning( suppress : 4996 )
	return (strnicmp(aStr, aPrefix, strlen(aPrefix)) == 0);
}



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
	
public:

    cWinHttp_handle() : m_handle(0) { }

    virtual ~cWinHttp_handle() { close(); }

	HINTERNET getHandle(void) const { return m_handle; }
	
protected:

    bool attach(HINTERNET handle)
    {
		assert((0 == m_handle) && "#5723: cWinHttp_handle.attach() called with NULL handle");
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
	 explicit cWinHttp_connection(const HINTERNET aSessionHandler, const LPCWSTR aURLHost)
	 {

		 // WinHttpConnect takes only the hostname part.
		 // You should pass the rest of the path to WinHttpOpenRequest
		 /*
		 INTERNET_DEFAULT_HTTPS_PORT
		 Uses the default port for HTTPS servers (port 443).
		 Selecting this port does not automatically establish a secure connection.
		 You must still specify the use of secure transaction semantics by using the
		 WINHTTP_FLAG_SECURE flag with WinHttpOpenRequest.
		 */
		 attach(WinHttpConnect(aSessionHandler, aURLHost, INTERNET_DEFAULT_PORT, 0));
	 }

 };



 
 ///////////////////////////////////////////////////////////
 //
 //
 //		class cWinHttp_sharedSessionAndConnection
 //
 //
 ///////////////////////////////////////////////////////////

class cWinHttp_sharedSessionAndConnection
{
private:
	wchar_from_char		m_userAgent_w;

	cWinHttp_session	m_sessionObj;

	HINTERNET   // hSession = NULL, 	// only one session per application 
									// The session is used to create connection objects
				hConnection = NULL; // Only one connection per each webserver with which you wish to communicate
									// The connection is used to create request objects. 
									// It can have multiple concurrent requests.
	bool        m_isGood;

	// INFO: WinInet Error Codes (12001 through 12156)
	// https://support.microsoft.com/en-us/help/193625/info-wininet-error-codes-12001-through-12156

public:
	cWinHttp_sharedSessionAndConnection::cWinHttp_sharedSessionAndConnection(const char * aURLHost, const bool isHTTPS, const char * aUserAgent, const bool runAsync) :
				m_isGood(true),
				m_userAgent_w(aURLHost),
				m_sessionObj(m_userAgent_w.get(), runAsync)
	{
		// move this to the abstract class to catch also MAC OS
		bool startsWithHTTPS = false;
		const char *postHost_noProtocol = NULL;
		if (stringStartsWith(aURLHost, "http://"))
			postHost_noProtocol = &aURLHost[7];
		else if (stringStartsWith(aURLHost, "https://"))
		{
			postHost_noProtocol = &aURLHost[8];
			startsWithHTTPS = true;
		}
		else
		{
			printf("httpPost(): postURL must start with http:// or https://\n%s\n", aURLHost);
			m_isGood = false;
			return;
		}

		/*
		if (!stringStartsWith(aURLpath, "/"))
		{
		printf("httpPost(): postPath must start with /\n%s\n", aURLpath);
		m_isGood = false;
		return;
		}
		*/


		if (!m_sessionObj.getHandle())
		{
			std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpOpen", GetLastError());
			m_isGood = false;
			return;
		}


		// WinHttpConnect takes only the hostname part.
		// You should pass the rest of the path to WinHttpOpenRequest
		/*
		INTERNET_DEFAULT_HTTPS_PORT
		Uses the default port for HTTPS servers (port 443).
		Selecting this port does not automatically establish a secure connection.
		You must still specify the use of secure transaction semantics by using the
		WINHTTP_FLAG_SECURE flag with WinHttpOpenRequest.
		*/
		wchar_from_char	postHost_w(postHost_noProtocol);
		hConnection = WinHttpConnect(m_sessionObj.getHandle(), postHost_w, INTERNET_DEFAULT_PORT, 0);

		if (!hConnection)
		{
			std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpConnect", GetLastError());
			m_isGood = false;
			return;
		}
	}


	virtual ~cWinHttp_sharedSessionAndConnection()
	{
		// Close any open handles.

		if (hConnection) WinHttpCloseHandle(hConnection);
		hConnection = NULL;
		
	}

	HINTERNET getConnectionHandle(void) const { return hConnection; }
	bool isGood(void) const { return m_isGood; }

};


///////////////////////////////////////////////////////////
//
//
//		class cWinHttp_request
//
//
///////////////////////////////////////////////////////////

class cWinHttp_request
{
private:
	HINTERNET hRequest;

public:
	explicit cWinHttp_request(const HINTERNET aConnectionHandler, const bool isHTTPS, const char * aURLpath)
	{
		DWORD flags = WINHTTP_FLAG_REFRESH;
		if (isHTTPS)
			flags |= WINHTTP_FLAG_SECURE;
			
		wchar_from_char	postPath_w(aURLpath);

		// you need to do WinHttpOpenRequest for every resource request.
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384099(v=vs.85).aspx
		hRequest = WinHttpOpenRequest(aConnectionHandler, 
										L"POST", 
										postPath_w, 
										NULL, // use HTTP version 1.1
										WINHTTP_NO_REFERER, 
										WINHTTP_DEFAULT_ACCEPT_TYPES, 
										flags);
		if (!hRequest)
			std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpOpenRequest", GetLastError());
	}

	virtual ~cWinHttp_request(void)
	{
		if (hRequest) WinHttpCloseHandle(hRequest);
		hRequest = NULL;
	}

	bool isGood(void) const { return (hRequest != NULL); }

	HINTERNET getRequestHandle(void) const { return hRequest; }
};

