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
 //		class cWinHttp_sharedSessionAndConnection
 //
 //
 ///////////////////////////////////////////////////////////

class cWinHttp_sharedSessionAndConnection
{
private:
	HINTERNET   hSession = NULL, hConnection = NULL;
	bool        m_isGood;

	// INFO: WinInet Error Codes (12001 through 12156)
	// https://support.microsoft.com/en-us/help/193625/info-wininet-error-codes-12001-through-12156

public:
	cWinHttp_sharedSessionAndConnection::cWinHttp_sharedSessionAndConnection(const char * aURLHost, const bool isHTTPS, const char * aUserAgent, const bool runAsync) :
		m_isGood(true)
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

		wchar_from_char userAgent_w(aUserAgent);

		// Use WinHttpOpen to obtain a session handle.
		DWORD sessionFlags = 0;
		if (runAsync)
			sessionFlags |= WINHTTP_FLAG_ASYNC;

		hSession = WinHttpOpen(((aUserAgent) ? userAgent_w.get() : NULL), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, sessionFlags);

		if (!hSession)
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
		hConnection = WinHttpConnect(hSession, postHost_w, INTERNET_DEFAULT_PORT, 0);

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
		if (hSession) WinHttpCloseHandle(hSession);
		hConnection = NULL;
		hSession = NULL;
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
	cWinHttp_request(const HINTERNET aConnectionHandler, const bool isHTTPS, const char * aURLpath)
	{
		DWORD flags = WINHTTP_FLAG_REFRESH;
		if (isHTTPS)
			flags |= WINHTTP_FLAG_SECURE;

		wchar_from_char	postPath_w(aURLpath);

		// // you need to do WinHttpOpenRequest for every resource request.
		hRequest = WinHttpOpenRequest(aConnectionHandler, L"POST", postPath_w, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
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

