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

bool cpccInternet::isConnectedToInternet(void)
{
	/*
	InternetCheckConnection function
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa384346.aspx
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa370773%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	*/

    // for InternetCheckConnection. WinINET, uses the IE settings for accessing the internet. Security, firewall, proxy, etc.
    
	// The function was never successful if I didn't specify a URL to use.
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


class cConnectToServer
{
private:
    HINTERNET hInternetSession = NULL, hURL =NULL;
    

public:
    HINTERNET getUrlOpenHandle(void) const { return hURL; }

    bool connect(const cpcc_char *aUrl, const cpcc_char *aUserAgent)
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

        // Make connection to desired page.
        hURL = InternetOpenUrl( hInternetSession,                       // session handle
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


    ~cConnectToServer(void)
    {
        if (hURL)
            InternetCloseHandle(hURL);
        if (hInternetSession)
            InternetCloseHandle(hInternetSession);
        hURL = hInternetSession = NULL;
    }

};


bool cpccInternet::downloadToBuffer(const cpcc_char *aUrl, const char *aBuffer, const long bufferSize, long &downloadedBytes)
{
    cConnectToServer serverConn;
    if (!serverConn.connect(aUrl, _T("cpcc library")))
        return false;

    DWORD nBytes = 0;
    if (!InternetReadFile(serverConn.getUrlOpenHandle(), (LPVOID)aBuffer, bufferSize, &nBytes))
        return false;

    return ((downloadedBytes=(long)nBytes) < bufferSize);
}


// e..g downloadSync("http://www.google.com/intl/en_ALL/images/srpr/logo1w.png", "c:\file.png");
bool cpccInternet::downloadToFile(const cpcc_char *aUrl, const cpcc_char *saveTo, const long maxSize, long &downloadedBytes)
{
    if ((!aUrl) || (!saveTo))
        return false;

    if ((cpcc_strlen(aUrl)==0) || (cpcc_strlen(saveTo)==0))
        return false;

        
	// https://docs.microsoft.com/en-us/windows/desktop/api/wininet/nf-wininet-internetreadfile
	// https://stackoverflow.com/questions/3539951/wininet-trouble-downloading-file-to-client
    #ifdef cpccDEBUG
        cpcc_cout << _T("Will download from:") << aUrl << _T(" To:") << saveTo << std::endl;
    #endif

    cConnectToServer serverConn;
    if (!serverConn.connect(aUrl, _T("cpcc library")))
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