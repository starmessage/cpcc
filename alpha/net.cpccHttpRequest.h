/*  *****************************************
 *  File:		net.cpccHttpRequestClient.h
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

#include <cstdio>
#include "../io.cpccLog.h"
#include "../net.cpccURLbuilder.h"

#ifdef __APPLE__
        #include "net.cpccHttpRequestMac.h"
        typedef cpccHttpPostMac cpccHttpPostImpl;
#elif _WIN32
        #include "net.cpccHttpRequestWin.h"
        typedef cpccHttpRequestClientWin cpccHttpPostImpl;
#endif


/*
 // split a full URL to postURL and postData
 std::string aURLwithParametersStr(aURLwithParameters);
 auto index = aURLwithParametersStr.find('?');
 if (index == std::string::npos)
 return 901; // no parameters found
 
 std::string strURL = aURLwithParametersStr.substr(0,index);
 std::string strParams = aURLwithParametersStr.substr(index+1);
 std::cout << "strURL=" << strURL << std::endl;
 std::cout << "strParams=" << strParams << std::endl;
 */

class cpccHttpRequestClient
{
private:
    cpccHttpPostImpl m_impl;
    
public:
    explicit cpccHttpRequestClient(	const char *aProtocolAndHostAddress,
									const char *aURLpath,
									const bool isHTTPS,
									const char *aUserAgent = 0,
									const bool runAsync=false)
		: m_impl(aProtocolAndHostAddress, aURLpath, isHTTPS, aUserAgent, runAsync)
    {   
		if (runAsync)
			infoLog().add("cpccHttpRequestClient created with Async flag");

		if (isHTTPS)
			infoLog().add("cpccHttpRequestClient created with HTTPS flag");
	}
    
    virtual ~cpccHttpRequestClient() {  }
    
    // static bool internetIsOn(void) { return cpccHttpPostImpl::internetIsOn(); }

    int httpPost(const char *postData, const int timeoutInSec) {  return m_impl.httpPost(postData, timeoutInSec); 	}

	int httpPostAsync( std::atomic<bool> &errorOccured, std::atomic<int> &nPending, const char *postData, const int timeoutInSec)
	{
		int i = nPending;
		infoLog().addf("httpPostAsync() found %i pending tasks", i);
		return m_impl.httpPostAsync(errorOccured, nPending, postData, timeoutInSec);
	}
     
 };


 

