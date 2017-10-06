/*  *****************************************
 *  File:		net.cpccHttpRequestWin.h
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

#pragma once

#include <windows.h>
#include <WinHttp.h>
#include <atomic>
#include "net.cpccWinHttp_helpers.h"
#include "../io.cpccLog.h"
#include "../core.cpccOS.h"




class cpccHttpRequestClientWin
{
private:
	
	bool				m_isHTTPS, m_disabled;
	std::string			m_postPath;
	cWinHttp_session	*m_sessionPtr;
	cWinHttp_connection *m_connectionPtr;

public:
	cpccHttpRequestClientWin(const char *aURLHost, const char *aURLpath, const bool isHTTPS, const char *aUserAgent = 0, const bool runAsync = false);

    int httpPost(const char *postData , const int timeoutInSec=60);
	
	int httpPostAsync(std::atomic<bool> &errorOccured, std::atomic<int> &nPending, const char *postData, const int timeoutInSec=60);

	bool isGood(void) const;
};