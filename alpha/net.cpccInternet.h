/*  *****************************************
 *  File:		net.cpccInternet.h
 *	Purpose:	Hack to include both wininet.h and winhttp 
 *				without producing the compiler errors like
 *				wininet.h: warning C4005: 'BOOLAPI': macro redefinition
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

#include "../cpccUnicodeSupport.h"

bool checkInternetConnectionIsOn_viaDefaultRoute(void);

class cpccInternet
{
public:
	static bool isConnectedToInternet(void);
    static bool wininet_downloadToFile(const cpcc_char *aUrl, const cpcc_char *saveTo, const long maxSize, 
                                long &downloadedBytes, const unsigned int timeout_sec, const cpcc_char *aUserAgent);
    static bool wininet_downloadToBuffer(const cpcc_char *aUrl, const char *aBuffer, const long bufferSize, 
                                long &downloadedBytes, const unsigned int timeout_sec, const cpcc_char *aUserAgent);


};