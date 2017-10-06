/*  *****************************************
 *  File:		net.cpccHttpRequestWin.cpp
 *	Purpose:	Portable (cross-platform), light-weight, httpDownloader class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#include "net.cpccHttpRequestWin.h"
#include "../cpccUnicodeSupport.h"
#include "../core.cpccOS.h"
#include "../core.cpccStringUtil.h"
#include "../cpccInstanceCounterPattern.h"
#include <string>
#include <iostream>
#include <windows.h>

#pragma comment(lib, "Winhttp.lib")

/*

	//	http://www.bogotobogo.com/cplusplus/C11/7_C11_Thread_Sharing_Memory.php

#include <mutex>

std::mutex mu;

void shared_cout(std::string msg, int id)
{
mu.lock();
std::cout << msg << ":" << id << std::endl;
mu.unlock();
}

or to ensure desrtruction:
	// the access to this function is mutually exclusive
	std::lock_guard<std::mutex> guard(myMutex);

*/


/*
	async demo at https://msdn.microsoft.com/en-us/library/aa383138(VS.85).aspx
	Warning  WinHTTP is not reentrant except during asynchronous completion callback. 
	That is, while a thread has a call pending to one of the WinHTTP functions such as 
	WinHttpSendRequest, WinHttpReceiveResponse, WinHttpQueryDataAvailable, WinHttpReadData, or WinHttpWriteData, 
	it must never call WinHTTP a second time until the first call has completed.
	
	Status callback functions must be thread safe.

	The WINHTTP_ASYNC_RESULT structure provides the error code and the function that caused the error.
	case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
		pAR = (WINHTTP_ASYNC_RESULT *)lpvStatusInformation;
		swprintf_s( szBuffer,L"%s: REQUEST_ERROR - error %d, result %s",
		cpContext->szMemo, pAR->dwError,
		GetApiErrorString(pAR->dwResult));
		Cleanup(cpContext);
		break;


*/



/*
simple example
https://msdn.microsoft.com/en-us/library/aa384270%28VS.85%29.aspx?f=255&MSPPError=-2147217396#Posting_data_to_the_

HttpSendRequest function
https://msdn.microsoft.com/en-us/library/windows/desktop/aa384247%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396



Session -> Connection -> Request
To use WinHTP functions in full asynchronous mode, you must do things in the right order:

[ok] 1. Open the session with WinHttpOpen() - Use INTERNET_FLAG_ASYNC
Check for errors

2. Set a callback using WinHttpSetStatusCallback()
Check for errors

[ok] 3. Open the connection using WinHttpConnect() - Specify the URL
Check for errors

4. Open a request with WinHttpOpenRequest() - Specify path and VERB
Check for errors
WINHTTP_CALLBACK_STATUS_HANDLE_CREATED notification indicates that a request handle was created.

5. Open a request with WinHttpSendRequest() - Specify the data length
Check for errors
This function optionally can send data immediately after the request headers.
An application can use the same HTTP request handle in multiple calls to WinHttpSendRequest <-------- ***
to re-send the same request, but the application must read all data returned from the previous
call before calling this function again.
Upon receiving the WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE statuscallback, the application
can start to receive a response from the server with WinHttpReceiveResponse. Before then,
no other asynchronous functions can be called, otherwise, ERROR_WINHTTP_INCORRECT_HANDLE_STATE is returned.

6. Send the Data with WinHttpWriteData()
Check for errors
Warning  When using WinHTTP asynchronously, always set the lpdwNumberOfBytesWritten parameter to NULL
and retrieve the bytes written in the callback function; otherwise, a memory fault can occur.
When the application is sending data, it can call WinHttpReceiveResponse to end the data transfer.
If WinHttpCloseHandle is called, then the data transfer is aborted.

7. Receive the Response with WinHttpReceiveResponse()
If this function returns TRUE, the application should expect
WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE completion callback indicating success

8. Find Bytes available with WinHttpQueryDataAvailable()
When WinHTTP is used in asynchronous mode, always set the lpdwNumberOfBytesAvailable parameter to NULL
and retrieve the bytes available in the callback function
If no data is available and the end of the file has not been reached,
the function returns TRUE, and when data becomes available, calls the callback function with
WINHTTP_STATUS_CALLBACK_DATA_AVAILABLE and indicates the number of bytes immediately available
to read by calling WinHttpReadData.
The amount of data that remains is not recalculated until all available data indicated by the call to
WinHttpQueryDataAvailable is read.

8. Read the Available Data with WinHttpReadData()
When WinHTTP is used in asynchronous mode, always set the lpdwNumberOfBytesRead parameter to NULL
and retrieve the bytes read in the callback function.
Warning: The Buffer and BuffLen values are modified asynchronously by WinHTTP!
If this function returns TRUE, use theWINHTTP_CALLBACK_STATUS_READ_COMPLETE completion to determine
whether this function was successful and the value of the parameters.
When the read buffer is very small, WinHttpReadData might complete synchronously.  If you call
WinHttpReadData after receiving WINHTTP_CALLBACK_STATUS_READ_COMPLETE the result is a stack overflow.
In general, it is best to use a read buffer that is comparable in size, or larger than the internal
read buffer used by WinHTTP of 8 KB
Use the return value of WinHttpReadData rather than that of WinHttpQueryDataAvailable to
determine whether the end of the response has been reached, because an improperly terminated
response can cause WinHttpQueryDataAvailable to continue to indicate that more data is
available even when the response has been completely read.

9. Close the Request and connection handles with WinHttpCloseHandle and wait for INTERNET_STATUS_HANDLE_CLOSING
and the facultative INTERNET_STATUS_REQUEST_COMPLETE notification (sent only if an error occurs –
like a sudden closed connection - you must test thees cases).
At this state, you can either begin a new connection process or close the session handle. But before
closing it, you should un-register the callback function.

*/


// This sample demonstrate the use of Winhttp APIs to send asynchronous requests to a server 
// and how to cancel such requests.
// https://github.com/pauldotknopf/WindowsSDK7-Samples/tree/master/web/winhttp/winhttpasyncsample

// good example
// https://stackoverflow.com/questions/10661014/winhttp-multiple-asynchronous-requests

// background info
// http://www.coastrd.com/winhttp

// http://www.naughter.com/winhttpwrappers.html
// WinHttpWrappers, a set of C++ classes to encapsulate WinHTTP and especially its asynchronous support.

// Asynchronous Completion in WinHTTP
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa383138(v=vs.85).aspx

// The AsyncWinHttp is a simple wrapper of WinHTTP to achieve asynchronous HTTP.
// https://www.codeproject.com/Articles/47125/Asynchronous-WinHTTP-Library

/* callback function
https://msdn.microsoft.com/en-us/library/windows/desktop/aa384115%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

The WinHttpSetStatusCallback function sets up a callback function that WinHTTP can call as progress is made during an operation.

If you set the callback on the session handle before creating the request handle, the request handle inherits the callback function pointer from its parent session.
However, changing the callback function on a handle does not change the callbacks on derived handles, such as that returned by WinHttpConnect.
You must change the callback function at each level.
Αρα καλυτερα να την ενωσω στο WinHttpRequest?

Both synchronous and asynchronous functions use the callback function to indicate the progress of the request,
such as resolving a name, connecting to a server, and so on.
The callback function is required for an asynchronous operation.

At the end of asynchronous processing, the application may set the callback function to NULL.
This prevents the client application from receiving additional notifications.
WinHttpSetStatusCallback( hOpen,
NULL,
WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,
NULL );


*/



///////////////////////////////////////////////////////////
//
//
//		class cpccHttpRequestAsync
//
//
///////////////////////////////////////////////////////////

class cpccHttpRequestAsync
{
public:

	//enum  WF { step0_noAction, step1_sendRequest, step2_receiveResponse, step3_getStatusCode, step4_autoDestruct, step5_finished };
	//WF					m_nextStep = step0_noAction;
	// DWORD				m_callback_status = 0;

private:
	
	cWinHttp_request	m_request;
	std::atomic<int>	&m_nPending;
	std::atomic<bool>	&m_errorOccured;
	cpccInstanceCounterPattern m_instanceCounter;
	std::string			m_postPayloadBuffer; // This buffer must remain available until the request handle is closed or the call to WinHttpReceiveResponse has completed.
	bool				m_receiveResponseCalled = false,
						m_getStatusCodeCalled = false;

public:

	explicit cpccHttpRequestAsync(std::atomic<bool> &errorOccured, std::atomic<int> &nPending, const HINTERNET aConnectionHandle, const bool isHTTPS, const LPCWSTR aPostPath, const char *aPostPayload, const int aTimeout, WINHTTP_STATUS_CALLBACK aCallback):
		m_nPending(nPending), m_errorOccured(errorOccured),
		m_postPayloadBuffer(aPostPayload? aPostPayload:""),
		m_request(aConnectionHandle, isHTTPS, aPostPath, aTimeout),
		m_instanceCounter(nPending)
	{
		int i = nPending;
		infoLog().addf(__FUNCTION__ ", asyncCountrer=%i", i);

		if (!m_request.isGood())
		{
			m_errorOccured = true;
			return;
		}
		m_request.SetStatusCallback(aCallback);
		infoLog().add("will sendRequest()");
		sendRequest();

	}


	~cpccHttpRequestAsync()
	{
		infoLog().addf("destructor with context =%lu", (DWORD_PTR)this);
	}


	void autoDestruct(void)
	{
		// cannot be called from the constructor
		delete this;
	}


	void sendRequest(void)
	{
		if (m_errorOccured)
			return;

		infoLog().addf("sendRequest() called for context %lu", (DWORD_PTR)this);
		DWORD_PTR context = (DWORD_PTR)this;
		BOOL winApiResult = m_request.sendRequest((LPSTR)m_postPayloadBuffer.c_str(), (DWORD)m_postPayloadBuffer.length(), context);
		/* δημιουργει τα παρακατω messages
		WINHTTP_CALLBACK_STATUS_SENDING_REQUEST
		WINHTTP_CALLBACK_STATUS_REQUEST_SENT

		WINHTTP_CALLBACK_STATUS_SENDING_REQUEST
		WINHTTP_CALLBACK_STATUS_REQUEST_SENT

		WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE
		*/
		if (winApiResult == FALSE)
		{
			m_errorOccured = true;
			return;
		}
	}


	void receiveResponse(void)
	{
		if (m_receiveResponseCalled)
		{
			warningLog().addf("receiveResponse() called AGAIN for context %lu", (DWORD_PTR)this);
			return;
		}

		m_receiveResponseCalled = true;
		infoLog().addf("receiveResponse() called for context %lu", (DWORD_PTR) this);
		if (m_errorOccured)
			return;

		if (m_request.ReceiveResponse() == FALSE)
			m_errorOccured = true;
		/* δημιουργει τα παρακατω messages
		WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE
		WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED
		WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE
		*/
	}


	void getStatusCode(void)
	{
		
		if (m_getStatusCodeCalled)
		{
			warningLog().addf("getStatusCode() called AGAIN for context %lu", (DWORD_PTR)this);
			return;
		}
		m_getStatusCodeCalled = true;
		infoLog().addf("getStatusCode() called for context %lu", (DWORD_PTR)this);
		// to keep or not to keep?
		// m_request.removeStatusCallback();

		DWORD dwStatusCode;
		if (m_request.QueryHeaders_statuscode(&dwStatusCode)==false)
		{
			m_errorOccured = true;
		}
		infoLog().addf("async status code=%lu", dwStatusCode);
	}
	 	
};


///////////////////////////////////////////////////////////
//
//
//		class callback function
//
//
///////////////////////////////////////////////////////////

void CALLBACK winHttp_ProgressCallback(HINTERNET hInternet, _In_ DWORD_PTR dwContext, _In_ DWORD dwInternetStatus, _In_ LPVOID lpvStatusInformation, _In_ DWORD     dwStatusInformationLength)
{
	// Status callback functions must be threadsafe.
	cpccHttpRequestAsync *ptr = (cpccHttpRequestAsync *)dwContext;
	if (!ptr)
	{
		errorLog().add("winHttp_ProgressCallback called with NULL dwContext");
		// return;
	}
	
	// infoLog().addf("winHttp_ProgressCallback() called with context %lu", dwContext);
	switch (dwInternetStatus)
	{
	default:
		infoLog().addf(__FUNCTION__ " ignored dwInternetStatus=%li", dwInternetStatus);
		break;

	case WINHTTP_CALLBACK_STATUS_HANDLE_CREATED:
		infoLog().addf("WINHTTP_CALLBACK_STATUS_HANDLE_CREATED for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_RESOLVING_NAME:
		infoLog().addf("WINHTTP_CALLBACK_STATUS_RESOLVING_NAME for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_NAME_RESOLVED:
		infoLog().addf("WINHTTP_CALLBACK_STATUS_NAME_RESOLVED for context %lu", dwContext);
		break;
		
	case WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE:
		infoLog().addf("WINHTTP_CALLBACK_STATUS_RECEIVING_RESPONSE for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_SENDING_REQUEST:
		infoLog().addf("WINHTTP_CALLBACK_STATUS_SENDING_REQUEST for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION:
		infoLog().addf("WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
		// 		This handle value has been terminated.The lpvStatusInformation parameter contains a pointer to the HINTERNET handle.
		//		There will be no more callbacks for this handle.
		//		the callback does not need to be able to handle reentrance for the same request, because this callback is guaranteed to be the last, and does not occur when other messages for this request are handled.
		infoLog().addf("WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_REQUEST_SENT:
		//  kaleitai 2 fores, h 2h meta to getstatus
		// TO BE IGNORED
		infoLog().addf("WINHTTP_CALLBACK_STATUS_REQUEST_SENT for context %lu", dwContext);
		break;

	case WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED:
		// Successfully received a response from the server. 
		// The lpvStatusInformation parameter contains a pointer to a DWORD indicating the number of bytes received.
		infoLog().addf("WINHTTP_CALLBACK_STATUS_RESPONSE_RECEIVED for context %lu", dwContext);
		break;


	//////////////////////////////////////////////
	//  messages that trigger further work
	//	

	case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
		// The request completed successfully. The lpvStatusInformation parameter is NULL.
		// From other sources:
		// Upon receiving the WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE statuscallback, the application
		// can start to receive a response from the server with WinHttpReceiveResponse.
		// Before then, no other asynchronous functions can be called, otherwise, ERROR_WINHTTP_INCORRECT_HANDLE_STATE is returned.
		infoLog().addf("WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE for context %lu", dwContext);
		
		////////////////////
		if (ptr)
			ptr->receiveResponse();
		/*	Receive the Response with WinHttpReceiveResponse()
			If this function returns TRUE, the application should expect
			WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE completion callback indicating success
		*/
		break;




	case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
		// The response header has been received and is available with WinHttpQueryHeaders. The lpvStatusInformation parameter is NULL.
		infoLog().addf("WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE for context %lu", dwContext);
		if (ptr)
		{
			ptr->getStatusCode();
			ptr->autoDestruct();
		}
		break;

	case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
		// An error occurred while sending an HTTP request. 
		// The lpvStatusInformation parameter contains a pointer to a WINHTTP_ASYNC_RESULT structure. 
		// Its dwResult member indicates the ID of the called function and dwError indicates the return value.
		errorLog().addf("WINHTTP_CALLBACK_STATUS_REQUEST_ERROR for context %lu", dwContext);
		{	DWORD rerror = ((WINHTTP_ASYNC_RESULT*)lpvStatusInformation)->dwError;
			errorLog().addf(cpccOS::getWindowsErrorCodeAndText("WINHTTP_CALLBACK_STATUS_REQUEST_ERROR", rerror).c_str());
		}
		break;

	}
}


///////////////////////////////////////////////////////////
//
//
//		class cpccHttpRequestClientWin
//
//
///////////////////////////////////////////////////////////



int cpccHttpRequestClientWin::httpPostAsync(std::atomic<bool> &errorOccured, std::atomic<int> &nPending, const char *postData, const int aTimeOutInSec)
{
	infoLog().add("cpccHttpRequestClientWin::httpPostAsync() called");

	// simple example
	// https://stackoverflow.com/questions/10661014/winhttp-multiple-asynchronous-requests

	// example from MS
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa383138(v=vs.85).aspx#_samples
	// For asynchronous requests, the WinHTTP functions require the application to provide a nonzero context value. 
	// The context value provides a way for your status callback function to identify the request that caused the 
	// status callback, and you can also use it to provide other information used in processing the callback notification.
	// The context value can be any variable that can be cast as a DWORD_PTR value.
	// One possibility is to pass the address of a structure that contains the resources required by your application.

	if (m_disabled)
		return 1201;

	if (!isGood())
	{
		errorLog().add("error 1959: " __FUNCTION__ " isGood()");
		return 1959;
	}

	// δημιουργει μηνυμα WINHTTP_CALLBACK_STATUS_HANDLE_CREATED for context 0
	// m_connectionPtr->SetStatusCallback(winHttp_ProgressCallback);
	
	wchar_from_char	postPath_w(m_postPath.c_str());

	cpccHttpRequestAsync *fireAndForgetRequest = 
		new cpccHttpRequestAsync(errorOccured, nPending, 
								 m_connectionPtr->getHandle(),
								 m_isHTTPS, postPath_w, postData, aTimeOutInSec, winHttp_ProgressCallback);
	if (!fireAndForgetRequest)
	{
		errorOccured = true;
		return 1301;
	}


	return 200;
}


int cpccHttpRequestClientWin::httpPost(const char *postData, const int aTimeOutInSec)
{

	// proxy example:
	// https://stackoverflow.com/questions/27977386/winhttpsendrequest-post-with-https-on-windows-server-2008-rc2
	if (m_disabled)
		return 1201;

	if (!isGood())
	{
		errorLog().add("error 1959: " __FUNCTION__ " isGood()");
		return 1959;
	}

	wchar_from_char	postPath_w(m_postPath.c_str());
	cWinHttp_request winhttp_request(m_connectionPtr->getHandle(), m_isHTTPS, postPath_w, aTimeOutInSec);

	if (!winhttp_request.isGood())
		return 1940;

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa383138%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396#initialize
	The first step in this process is to use WinHttpCrackUrl to parse the URL supplied by the user
	into component parts in a URL_COMPONENTS structure.
	Next, SendRequest calls WinHttpConnect to open an HTTP session with the specified host server.
	If this succeeds, it calls WinHttpOpenRequest to open an appropriate "GET" request handle, and
	WinHttpSetStatusCallback to set your callback function for that handle.
	Finally, it calls WinHttpSendRequest to send the GET request to the server.
	*/


	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384110%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	// std::cout << "httpPost() called with httpHeader_User_Agent=";
	/*
	if (httpHeader_User_Agent)
	std::cout << httpHeader_User_Agent << std::endl;
	else
	std::cout << "NULL" << std::endl;
	*/

	// test for XP; No improvement
	// WinHttpSetOption(winhttp_request.getHandle(), WINHTTP_OPTION_CLIENT_CERT_CONTEXT, WINHTTP_NO_CLIENT_CERT_CONTEXT, 0);

	if (!postData)
		return 1913;
	std::string postDataBuffer(postData);

	// you need to pass narrow strings not wide as the post data
	// https://stackoverflow.com/questions/38672719/post-request-in-winhttp-c
	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/aa384110(v=vs.85).aspx
	Even when WinHTTP is used in asynchronous mode, that is, when WINHTTP_FLAG_ASYNC has been
	set in WinHttpOpen, this function can operate either synchronously or asynchronously.
	In either case, if the request is sent successfully, the application is called back with
	the completion status set to WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE.
	The WINHTTP_CALLBACK_STATUS_REQUEST_ERROR completion indicates that the operation completed
	asynchronously, but failed.
	Upon receiving the WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE status callback,
	the application can start to receive a response from the server with WinHttpReceiveResponse.
	Before then, no other asynchronous functions can be called, otherwise,
	ERROR_WINHTTP_INCORRECT_HANDLE_STATE is returned.

	An application must not delete or alter the buffer pointed to by lpOptional until the request
	handle is closed or the call to WinHttpReceiveResponse has completed.
	*/

	// https://src.chromium.org/viewvc/chrome/trunk/src/net/http/http_transaction_winhttp.cc?pathrev=1919

	// std::cout << "debug post data:\n" << postDataBuffer << std::endl;

	DWORD_PTR noContext = 1;
	BOOL winApiResult =
		winhttp_request.sendRequest((LPSTR)postDataBuffer.c_str(), // This buffer must remain available until the request handle is closed or the call to WinHttpReceiveResponse has completed.
		(DWORD)postDataBuffer.length(), noContext);
	if (winApiResult == FALSE)
		return 1960;

	/*
	WinHTTP enforces a policy of one pending asynchronous operation per request handle.
	If one operation is pending and another WinHTTP function is called, the second function
	fails and GetLastError returns ERROR_INVALID_OPERATION.

	Error 4317 is a standard Win32 error code, ERROR_INVALID_OPERATION.

	WinHttpQueryAuthSchemes will typically return this error code if an
	authentication challenge has not been received, or (less likely) if WinHTTP
	does not recognize any of the authentication schemes listed in the
	authentication challenge.

	*/

	// amazon https error
	// https://stackoverflow.com/questions/29801450/winhttp-doesnt-download-from-amazon-s3-on-winxp

	/*
	SSL error on XP
	https://community.constantcontact.com/t5/Authentication-and-Access-ie-401/The-message-received-was-unexpected-or-badly-formatted/m-p/258579#M830
	It sounds like it might be an issue with the security protocol used by your XP environment.
	We stopped supporting TLS 1.0 with RC4 ciphers and SSL 3 to prevent the man - in - the - middle, or POODLE,
	attacks that were widely publicized around the web.More information on that is at
	http ://techblog.constantcontact.com/api/release-updates/support-ending-for-tls-v1-0-rc4-cipher-and-s...
	If you can upgrade the security protocol to TLS 1.1 or 1.2, you'll likely see the issue resolved.
	Constant Contact no longer supports or works on Windows XP.  The change mentioned by Shannon for
	TLS 1.0 and SSL v3 mean that there are no supported SSL security protocols left on Windows XP that
	comply with the security standards of Constant Contact.
	*/

	// End the WinHttpSendRequest.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384105(v=vs.85).aspx
	winApiResult = winhttp_request.ReceiveResponse();
	if (winApiResult == FALSE)
		return 1961;

	DWORD dwStatusCode = 1965;
	// WinHttpReceiveResponse must have been called for this handle and have completed before WinHttpQueryHeaders is called.
	// In Async mode, this must be called after WINHTTP_CALLBACK_STATUS_REQUEST_SENT
	winApiResult = winhttp_request.QueryHeaders_statuscode(&dwStatusCode);
	if (winApiResult == FALSE)
		return 1981;

	return (int)dwStatusCode;
}



cpccHttpRequestClientWin::cpccHttpRequestClientWin(const char *aURLHost, const char *aURLpath, const bool isHTTPS, const char *aUserAgent, const bool runAsync )
	:	m_isHTTPS(isHTTPS), m_postPath(aURLpath), m_sessionPtr(0), m_connectionPtr(0), m_disabled(false)

{  
	logFunctionLife _conLife("cpccHttpRequestClientWin Constructor");
	debugLog().addf("connection handler for server: [%s]", aURLHost);

	if (runAsync)
		infoLog().add("cpccHttpRequestClientWin created with Async flag");
	

	if (!WinHttpCheckPlatform())
	{
		infoLog().add("cpccHttpRequestClientWin was disabled becayse WinHTTP is not supported by your Windows version.");
		m_disabled = true;
		return;
	}

	// create the reusable WinHttp session 
	wchar_from_char m_userAgent_w(aUserAgent);
	m_sessionPtr = new cWinHttp_session(m_userAgent_w, runAsync);
	if (!m_sessionPtr)
		return;

	if (!m_sessionPtr->getHandle())
	{
		errorLog().add(cpccOS::getWindowsErrorCodeAndText("WinHttpOpen", GetLastError()));
		return;
	}

	if (!m_sessionPtr->isGood())
		return;
	

	// check if aURLpath starts with http:// or https:// and separate the server address that is needed for the WinHttp connection.
	// debugLog().add("cpccHttpRequestClientWin point 1");
	bool startsWithHTTPS = false;
	const char *postHost_noProtocol = NULL;
	if (stringUtils::stringStartsWith(aURLHost, "http://"))
		postHost_noProtocol = &aURLHost[7];
	else if (stringUtils::stringStartsWith(aURLHost, "https://"))
	{
		postHost_noProtocol = &aURLHost[8];
		startsWithHTTPS = true;
	}
	else
	{
		errorLog().addf("httpPost(): postURL must start with http:// or https://\n%s\n", aURLHost);
		//m_isGood = false;
		return;
	}

	// debugLog().add("cpccHttpRequestClientWin point 2");
	wchar_from_char m_serverAddress_w(postHost_noProtocol);	// bare address without https:// nor http://
	m_connectionPtr = new cWinHttp_connection(m_sessionPtr->getHandle(), m_serverAddress_w.get());
	if (!m_connectionPtr)
		return;

	// debugLog().add("cpccHttpRequestClientWin point 3");
	if (!m_sessionPtr->getHandle())
	{
		errorLog().add(cpccOS::getWindowsErrorCodeAndText("WinHttpConnect", GetLastError()));
		return;
	}

	if (!m_connectionPtr->isGood())
		return;

}


bool cpccHttpRequestClientWin::isGood(void) const
{
	if (!m_sessionPtr || !m_connectionPtr)
		return false;

	if (!m_sessionPtr->isGood() || !m_connectionPtr->isGood())
		return false;

	return true;
}




