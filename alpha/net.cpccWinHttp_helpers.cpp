/*  *****************************************
 *  File:		net.cpccWinHttp_helpers.cpp
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

#include "net.cpccHttpRequest.h"
#include "../io.cpccLog.h"



///////////////////////////////////////////////////////////
//
//
//		winHttp_ProgressCallback
//
//
///////////////////////////////////////////////////////////

void CALLBACK winHttp_ProgressCallback(HINTERNET hInternet, _In_ DWORD_PTR dwContext, _In_ DWORD dwInternetStatus, _In_ LPVOID lpvStatusInformation, _In_ DWORD     dwStatusInformationLength)
{
	// Status callback functions must be threadsafe.
	cpccWinHttpRequestAsync *ptr = (cpccWinHttpRequestAsync *)dwContext;
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
