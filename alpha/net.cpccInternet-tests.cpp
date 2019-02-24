/*  *****************************************
 *  File:		net.cpccInternet-tests.cpp
 *	Purpose:	
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2019 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */



#include "../cpcc_SelfTest.h"
#include "net.cpccInternet.h"
#include "../io.cpccLog.h"

SELFTEST_BEGIN(cpccInternet_SelfTest)


{
    logFunctionLife tmplog("cpccInternet_SelfTest");

    std::cout << "Is Connected to the internet? " << (cpccInternet::isConnectedToInternet() ? "Yes" : "No") << std::endl;
    /*
    const cpcc_char *smallFileUrl = _T("https://www.StarMessageSoftware.com/robots.txt");
    // const cpcc_char *smallFileUrl = _T("https://www.starmessagesoftware.com/robots.txt");
    const cpcc_char *bigFileUrl = _T("https://www.StarMessageSoftware.com/download/starmessage-setup.exe");

    const unsigned long int MByte(1024 * 1024);
    long bufferSize = 20 * MByte * sizeof(char);
    char *buffer = (char *)malloc(bufferSize );

    unsigned int timeout = 100;
    long bytesDownloaded = 0;
    assert(cpccInternet::downloadToBuffer(smallFileUrl, buffer, bufferSize, bytesDownloaded, timeout, _T("selfTest cpccInternet"))
        && "#2354a: cpccInternet::download");

    assert((bytesDownloaded > 0) && (bytesDownloaded < 3000) && "#2354b: cpccInternet::download");

    // download a big file and let the timeout kick in
    timeout = 1;
    
    assert(!cpccInternet::downloadToBuffer(bigFileUrl, buffer, bufferSize, bytesDownloaded, timeout, _T("selfTest cpccInternet"))
        && "#2354c: cpccInternet::download");
    

    free(buffer);
    */
}

SELFTEST_END
