/*  *****************************************
 *  File:		net.cpccHttpRequestMac.h
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
#include <string>
#include <atomic>

int httpDownloadMac(const char *URL, void *buffer, const long bufferSize);


class cpccHttpPostMac
{
private:
    std::string m_postHost, m_postPath, m_userAgent;
    
public:
    explicit cpccHttpPostMac(const char *aURLhost,
                             const char *aURLpath,
                             const bool isHTTPS,
                             const char *aUserAgent = 0,
                             const bool runAsync=false)
            : m_postHost(aURLhost), m_postPath(aURLpath), m_userAgent(aUserAgent?aUserAgent:"")
    { }
    
    int httpPost(	const char *postData ,
                    const int timeoutInSec);
    
    int httpPostAsync(/* int &asynResult, */
                        std::atomic<bool> &errorOccured,
                        std::atomic<int> &nPending,
                        const char *postData ,
                        const int timeoutInSec);
    
    // static bool internetIsOn(void);
};
