/*  *****************************************
 *  File:		net.cpccHttpPostClient.h
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


#ifdef __APPLE__
        #include "net.cpccHttpRequestMac.h"
        typedef cpccHttpPostMac cpccHttpPostImpl;
#elif _WIN32
        #include "net.cpccHttpRequestWin.h"
        typedef cpccHttpRequestClientWin cpccHttpPostImpl;
#endif


/*
 // how to split a full URL to postURL and postData
 std::string aURLwithParametersStr(aURLwithParameters);
 auto index = aURLwithParametersStr.find('?');
 if (index == std::string::npos)
 return 901; // no parameters found
 
 std::string strURL = aURLwithParametersStr.substr(0,index);
 std::string strParams = aURLwithParametersStr.substr(index+1);
 std::cout << "strURL=" << strURL << std::endl;
 std::cout << "strParams=" << strParams << std::endl;
 */

/* 
template <class T>
class cpccStaticVar
{
private:
	T m_value;

public:	// ctors
	cpccStaticVar(const T aValue) : m_value(aValue) { }
	cpccStaticVar(): m_value(0) { }

public:	// functions
	const T& get(void) { return m_value; }
	inline operator T& (void) const { return m_value; }
	inline cpccStaticVar<T>& operator= (const T &aValue) = { m_value = aValue; return this; }
};
*/

class cpccHttpRequestClient
{
protected:
	std::atomic<bool>	m_errorOccured;
	std::atomic<int>	m_pendingAsyncTasks;

public:
	cpccHttpRequestClient() : m_errorOccured(false), m_pendingAsyncTasks(0) { }

	int pendingRequestsCount(void)	{ return  m_pendingAsyncTasks; };
	bool errorOccured(void)			{ return m_errorOccured; };

};


class cpccHttpPostClient: public cpccHttpRequestClient
{
private:
    cpccHttpPostImpl m_impl; // todo:  to use as parent class

	
public:
    explicit cpccHttpPostClient(	const char *aProtocolAndHostAddress,
									const char *aURLpath,
									const bool isHTTPS,
									const char *aUserAgent = 0,
									const bool runAsync=false)
									// todo:  maybe split it into two classes, one sync and one async,
									// then maybe also the implementation classes need similar split
		: m_impl(aProtocolAndHostAddress, aURLpath, isHTTPS, aUserAgent, runAsync)
    {   
		if (runAsync)
			infoLog().add("cpccHttpPostClient created with Async flag");

		if (isHTTPS)
			infoLog().add("cpccHttpPostClient created with HTTPS flag");
	}
    
    virtual ~cpccHttpPostClient() {  }
    
    int httpPost(const char *postData, const int timeoutInSec) {  return m_impl.httpPost(postData, timeoutInSec); 	}

	int httpPostAsync( const char *postData, const int timeoutInSec)
	{
		infoLog().addf("httpPostAsync() found %i pending tasks", pendingRequestsCount());
		return m_impl.httpPostAsync(m_errorOccured, m_pendingAsyncTasks, postData, timeoutInSec);
	}
     
 };


 

