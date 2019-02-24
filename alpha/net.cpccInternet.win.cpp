/*  *****************************************
 *  File:		net.cpccInternet.cpp
 *	Purpose:	Windows implementation of the cpccInternet class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include "net.cpccInternet.h"
#include "../cpccUnicodeSupport.h"
#include "../core.cpccIdeMacros.h"

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "Wininet.lib") // for InternetCheckConnection
// #include <Netlistmgr.h> // for get_IsConnectedToInternet()


#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi")


bool checkInternetConnectionIsOn_viaDefaultRoute(void)
{
    /*
    another way:
        https://www.codeproject.com/Tips/67577/Check-for-an-active-Internet-connection

    check for the presence of the default route in the IP forwarding table that is maintained by windows. This can be checked by using GetIPForwardTable function found in the iphlpapi.dll library. The default route is present if one of the entries in the table has a forwarding destination of 0.0.0.0.
    */


        bool bIsInternetAvailable = false;
        // Get the required buffer size
        DWORD dwBufferSize = 0;
        if (ERROR_INSUFFICIENT_BUFFER == GetIpForwardTable(NULL, &dwBufferSize, false))
        {
            BYTE *pByte = new BYTE[dwBufferSize];
            if (pByte)
            {
                PMIB_IPFORWARDTABLE pRoutingTable = reinterpret_cast<PMIB_IPFORWARDTABLE>(pByte);
                // Attempt to fill buffer with routing table information
                if (NO_ERROR == GetIpForwardTable(pRoutingTable, &dwBufferSize, false))
                {
                    DWORD dwRowCount = pRoutingTable->dwNumEntries; // Get row count
                    // Look for default route to gateway
                    for (DWORD dwIndex = 0; dwIndex < dwRowCount; ++dwIndex)
                    {
                        if (pRoutingTable->table[dwIndex].dwForwardDest == 0)
                        { // Default route designated by 0.0.0.0 in table
                            bIsInternetAvailable = true; // Found it
                            break; // Short circuit loop
                        }
                    }
                }
                delete[] pByte; // Clean up. Just say "No" to memory leaks
            }
        }
        return bIsInternetAvailable;

}


bool cpccInternet::isConnectedToInternet(void)
{
   	/*
	InternetCheckConnection function
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa384346.aspx
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa370773%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396

    InternetGetConnectedState (wininet.dll)
    BOOL bIsConnected = InternetGetConnectedState(&dwState, 0);
        https://docs.microsoft.com/en-us/windows/desktop/api/wininet/nf-wininet-internetgetconnectedstate
        Returns TRUE if there is an active modem or a LAN Internet connection, or FALSE if there is no Internet connection,
        or if all possible Internet connections are not currently active.

        

	*/

    // for InternetCheckConnection. WinINET, uses the IE settings for accessing the internet. Security, firewall, proxy, etc.
    
	// The function was never successful if I didn't specify a URL to use.
    // Todo: It fails if behind a proxy
	return InternetCheckConnection( _T("http://www.google.com"), FLAG_ICC_FORCE_CONNECTION, 0) !=0 ;

	/* e.g. with COM objects
	VARIANT_BOOL *pbIsConnected;
	INetworkListManager NLM;
	INetworkConnection::get_IsConnectedToInternet(pbIsConnected);
	return ((*pbIsConnected)==true)
	*/
}

// todo: tha to do argotera
template<typename Thandle>
class cAutoReleasedHandle
{
public:
    
    Thandle m_handle;

    cAutoReleasedHandle() : m_handle(0) { }
    virtual ~cAutoReleasedHandle() { repleaseHandle(); }

    inline const Thandle operator =(const Thandle aHandle)  { return m_handle = aHandle; }

    bool isGood(void) const { return m_handle != 0; }

    virtual void repleaseHandle(void) = 0;

};

class cAutoReleasedHINTERNET: public cAutoReleasedHandle<HINTERNET>
{
public:

    virtual void repleaseHandle(void) override
    {
        if (m_handle)
            InternetCloseHandle(m_handle);
        m_handle = NULL;
    }

};


class cWininet_ConnectToServer
{
private:
    HINTERNET hInternetSession = NULL, hURL = NULL;
    

public:
    HINTERNET getUrlOpenHandle(void) const { return hURL; }

    bool connect(const cpcc_char *aUrl, const cpcc_char *aUserAgent, const unsigned int aTimeout_sec)
    {
        if (!aUrl)
            return false;

        if (!cpccInternet::isConnectedToInternet())
            return false;

        // Make internet connection.
        hInternetSession = InternetOpen(aUserAgent, // agent
                                    INTERNET_OPEN_TYPE_PRECONFIG,  // access
                                    NULL, NULL, 0);                // defaults

        if (hInternetSession == NULL) 
        {
            // std::cerr <<  "Initializing WinINet failed." << std::endl;
            return false;
        }

        if (aTimeout_sec>0)
        { 
            DWORD dwTimeOut_msec = 1000 * aTimeout_sec; // timeout, but this is a MS bug and it is being ignored
            // http://forums.codeguru.com/showthread.php?355462-InternetReadFile()-timeout
            InternetSetOption(hInternetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut_msec, sizeof(dwTimeOut_msec));   
            InternetSetOption(hInternetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut_msec, sizeof(dwTimeOut_msec));
        }

        // Make connection to desired page.
        // use INTERNET_FLAG_RELOAD instead of NIL to redownload the file instead of using the cache 
        hURL = InternetOpenUrl( hInternetSession ,                       // session handle
                            aUrl,  // URL to access
                            NULL, 0, 0, 0);                         // defaults
        if (hURL == NULL) 
        {
            // std::cerr <<  "URL failed upon loading:" << aUrl << std::endl;
            InternetCloseHandle(hInternetSession);
            return false;
        }
        

        return true;
    }


    ~cWininet_ConnectToServer(void)
    {
        if (hURL)
            InternetCloseHandle(hURL);
        if (hInternetSession)
            InternetCloseHandle(hInternetSession);
        hURL = hInternetSession = NULL;
    }

};

// todo: add parameter with the useragent
bool cpccInternet::wininet_downloadToBuffer(const cpcc_char *aUrl, const char *aBuffer, const long aBufferSize, 
                                    long &downloadedBytes, const unsigned int timeout_sec, const cpcc_char *aUserAgent)
{
     if (!aUrl)
        return false;

    if (cpcc_strlen(aUrl)==0)
        return false;

    cWininet_ConnectToServer serverConn;
    if (!serverConn.connect(aUrl, aUserAgent, timeout_sec))
        return false;

    DWORD nBytes = 0, dwBufferSize = aBufferSize;
    if (!InternetReadFile(serverConn.getUrlOpenHandle(), (LPVOID)aBuffer, dwBufferSize, &nBytes))
        return false;

    return ((downloadedBytes=(long)nBytes) < aBufferSize);
}


// e..g downloadSync("http://www.google.com/intl/en_ALL/images/srpr/logo1w.png", "c:\file.png");
bool cpccInternet::wininet_downloadToFile(const cpcc_char *aUrl, const cpcc_char *saveTo, const long maxSize, long &downloadedBytes, const unsigned int timeout_sec, const cpcc_char *aUserAgent)
{
    if ((!aUrl) || (!saveTo))
        return false;

    if ((cpcc_strlen(aUrl)==0) || (cpcc_strlen(saveTo)==0))
        return false;

        
	// https://docs.microsoft.com/en-us/windows/desktop/api/wininet/nf-wininet-internetreadfile
	// https://stackoverflow.com/questions/3539951/wininet-trouble-downloading-file-to-client
    /*
        If the configured timeout has no effect (which is a known bug), then you should close the connection handle, 
        which will force the associated function to fail, thereby returning control to your program.
         Microsoft article Q224318, "How to control connection timeout value by creating second thread," 
         describes how to do that. (You need a separate thread because the other thread is stuck waiting for
         InternetReadFile to return.)
    */
    #ifdef cpccDEBUG
        cpcc_cout << _T("Will download from:") << aUrl << _T(" To:") << saveTo << std::endl;
    #endif

    cWininet_ConnectToServer serverConn;
    if (!serverConn.connect(aUrl, aUserAgent, timeout_sec))
        return false;

    /*
    if (!isConnectedToInternet())
    	return false;
  
	// Make internet connection.
    HINTERNET hInternetSession = InternetOpen( _T("cpcc"), // agent
    								INTERNET_OPEN_TYPE_PRECONFIG,  // access
    								NULL, NULL, 0);                // defaults

    if (hInternetSession == NULL) 
    {
        // std::cerr <<  "Initializing WinINet failed." << std::endl;
        return false;
    }

    // Make connection to desired page.
    HINTERNET hURL = InternetOpenUrl( hInternetSession,                       // session handle
    						aUrl,  // URL to access
    						NULL, 0, 0, 0);                         // defaults
    if (hURL == NULL) {
    	// std::cerr <<  "URL failed upon loading:" << aUrl << std::endl;
        InternetCloseHandle(hInternetSession);
        return false;
    }
    */

    #pragma warning( suppress : 4996 )
    FILE *fp = fopen(saveTo, "wb");
    if (fp == NULL)
        return false;
    
	// Read file.
    char buffer[1024];
    DWORD dwBytesRead;
    downloadedBytes = 0;
	while (InternetReadFile(serverConn.getUrlOpenHandle(), buffer, sizeof(buffer), &dwBytesRead))
	{
    	if (dwBytesRead == 0)
        	break;
        downloadedBytes += dwBytesRead;
	    fwrite(buffer, dwBytesRead, 1, fp);
        if (downloadedBytes > maxSize)
            break;
	}

	fclose(fp); 


    return (downloadedBytes <= maxSize);
}