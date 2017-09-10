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

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "Wininet.lib") // for InternetCheckConnection


bool cpccInternet::isConnectedToInternet(void)
{
	/*
	InternetCheckConnection function
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa384346.aspx
		https://msdn.microsoft.com/en-us/library/windows/desktop/aa370773%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	*/

    // for InternetCheckConnection. WinINET, uses the IE settings for accessing the internet. Security, firewall, proxy, etc.
    
	// The function was never successful if I didn't specify a URL to use.
	return InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);

	/* e.g. with COM objects
	VARIANT_BOOL *pbIsConnected;
	INetworkListManager NLM;
	INetworkConnection::get_IsConnectedToInternet(pbIsConnected);
	return ((*pbIsConnected)==true)
	*/
}
