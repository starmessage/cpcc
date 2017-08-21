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
#include <string>
#include <iostream>
#include <windows.h>
#include <WinHttp.h>
// #include <wininet.h> // for InternetCheckConnection
// #pragma comment(lib, "Wininet.lib") // for InternetCheckConnection
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





///////////////////////////////////////////////////////////
//
//
//		class helper_WinHttpInstallCallback
//
//
///////////////////////////////////////////////////////////


class helper_WinHttpInstallCallback
{

    
public:
    helper_WinHttpInstallCallback(const HINTERNET aHandle, WINHTTP_STATUS_CALLBACK aCallback)
    {
        // Install the status callback function.
        WINHTTP_STATUS_CALLBACK isCallback =
            WinHttpSetStatusCallback( aHandle,
                                      aCallback,
                                      WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,
                                    NULL);
        m_isGood = (isCallback!=WINHTTP_INVALID_STATUS_CALLBACK);
    }
    
    bool isGood(void) { return m_isGood; }
private:
    bool m_isGood;
};

///////////////////////////////////////////////////////////
//
//
//		class cpccHttpPostWin
//
//
///////////////////////////////////////////////////////////

bool cpccHttpPostWin::internetIsOn(void)
{
/*
	InternetCheckConnection function
	//	https://msdn.microsoft.com/en-us/library/windows/desktop/aa384346.aspx
	//	https://msdn.microsoft.com/en-us/library/windows/desktop/aa370773%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

BOOL InternetCheckConnection(
  _In_ LPCTSTR lpszUrl,
  _In_ DWORD   dwFlags,
  _In_ DWORD   dwReserved);

lpszUrl
	Pointer to a null-terminated string that specifies the URL to use to check the connection. This value can be NULL.
    If lpszUrl is non-NULL, the host value is extracted from it and used to ping that specific host.
    If lpszUrl is NULL and there is an entry in the internal server database for the nearest server, the host value is 
	extracted from the entry and used to ping that server.

Returns TRUE if a connection is made successfully, or FALSE otherwise.
*/
	// return (InternetCheckConnection(NULL,FLAG_ICC_FORCE_CONNECTION,0)==TRUE);
	return false;
}



/*
		https://msdn.microsoft.com/en-us/library/aa384270%28VS.85%29.aspx?f=255&MSPPError=-2147217396#Posting_data_to_the_

		HttpSendRequest function
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa384247%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

		http://www.cplusplus.com/forum/beginner/142026/
		
		// async post
		https://stackoverflow.com/questions/17888317/winhttpsendrequest-in-async-mode-doesnt-send-post-data

 
 Session -> Connection -> Request
 To use WinHTP functions in full asynchronous mode, you must do things in the right order:
 
 1. Open the session with WinHttpOpen() - Use INTERNET_FLAG_ASYNC
 Check for errors
 
 2. Set a callback using WinHttpSetStatusCallback()
 Check for errors
 
 3. Open the connection using WinHttpConnect() - Specify the URL
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

void asyncinfo()
{
	// This sample demonstrate the use of Winhttp APIs to send asynchronous requests to a server 
	// and how to cancel such requests.
	// https://github.com/pauldotknopf/WindowsSDK7-Samples/tree/master/web/winhttp/winhttpasyncsample


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
     If successful, returns a pointer to the previously defined status callback function or NULL if there was no previously defined status callback function. 
     Returns WINHTTP_INVALID_STATUS_CALLBACK if the callback function could not be installed. For extended error information, call GetLastError.
     If you set the callback on the session handle before creating the request handle, the request handle inherits the callback function pointer from its parent session.
     However, changing the callback function on a handle does not change the callbacks on derived handles, such as that returned by WinHttpConnect. You must change the callback function at each level.
     Αρα καλυτερα να την ενωσω στο WinHttpRequest?
     
     Both synchronous and asynchronous functions use the callback function to indicate the progress of the request, such as resolving a name, connecting to a server, and so on. The callback function is required for an asynchronous operation.
     
     At the end of asynchronous processing, the application may set the callback function to NULL. This prevents the client application from receiving additional notifications.
     WinHttpSetStatusCallback( hOpen,
                                NULL,
                                WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,
                                NULL );
     
     
    */
}

void test1()
{
	// from https://stackoverflow.com/questions/17888317/winhttpsendrequest-in-async-mode-doesnt-send-post-data
	/*
	WINHTTP_STATUS_CALLBACK cb = 
		WinHttpSetStatusCallback(
						request_handles.at(handler_index), 
						httpReadCallback, 
						WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE | 
						WINHTTP_CALLBACK_STATUS_REQUEST_ERROR |
						WINHTTP_CALLBACK_STATUS_REDIRECT | 
						WINHTTP_CALLBACK_STATUS_SECURE_FAILURE | 
						WINHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE |
    					WINHTTP_CALLBACK_STATUS_CLOSING_CONNECTION | 
    					WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED, 
    					NULL);
	WinHttpAddRequestHeaders(request_handles.at(handler_index), L"Content-Type:application/x-www-form-urlencoded", -1L, WINHTTP_ADDREQ_FLAG_ADD);
	
 	int rc = WinHttpSendRequest(request_handles.at(handler_index), WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)postData.c_str(), dataLen, dataLen, 0);
	*/
}


int cpccHttpPostWin::httpPostAsync(std::atomic<bool> &errorOccured, std::atomic<int> &nPending, 	const char *postData, const int timeoutInSec)
{
	return httpPost(postData, timeoutInSec);
}


int cpccHttpPostWin::httpPost(const char *postData, const int timeoutInSec)
{
	if (!m_reusedSessionAndConnection.isGood())
	{
		std::cerr << "error 1959: not m_reusedSessionAndConnection.isGood()";
		return 1959;
	}

	
	cWinHttp_request winhttp_request(m_reusedSessionAndConnection.getConnectionHandle(), m_isHTTPS, m_postPath.c_str());

	if (!winhttp_request.getRequestHandle())
	{
		std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpOpenRequest", GetLastError());
		return 1940;
	}


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
    
  	

    // I have found that for PHP to recognise the POSTed data, I had to also do this:
	LPCWSTR additionalHeaders = L"Content-Type: application/x-www-form-urlencoded";
   	DWORD additionalHeadersLen   = -1;
   
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
    bool bResults = WinHttpSendRequest(winhttp_request.getRequestHandle(),
                                    additionalHeaders,
                                    additionalHeadersLen,
                                    (LPSTR) postData, // This buffer must remain available until the request handle is closed or the call to WinHttpReceiveResponse has completed.
                                    strlen(postData),
                                    strlen(postData),
                                    NULL);
	if (!bResults)
	{ 
		std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpSendRequest", GetLastError());
		return 1960;
	}

	// End the WinHttpSendRequest.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa384102(v=vs.85).aspx
	bResults = WinHttpReceiveResponse(winhttp_request.getRequestHandle(), NULL);
	if (!bResults)
	{	
		std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpReceiveResponse", GetLastError());
		return 1960;
	}	

    DWORD dwStatusCode = 1965;
	DWORD dwSize = sizeof(dwStatusCode);

	// WinHttpReceiveResponse must have been called for this handle and have completed before WinHttpQueryHeaders is called.
	bool queryHeadersResult = WinHttpQueryHeaders(winhttp_request.getRequestHandle(),
									WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
    								WINHTTP_HEADER_NAME_BY_INDEX, 
    								&dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);

	if (!queryHeadersResult)
	{
		std::cerr << cpccOS::getWindowsErrorCodeAndText("WinHttpQueryHeaders", GetLastError());
		return 1981;
	}

    return (int) dwStatusCode;
}


