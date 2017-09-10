/*  *****************************************
 *  File:		net.cpccInternet.Mac.cpp
 *	Purpose:	Mac implementation of the cpccInternet class
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

#include <cocoa/cocoa.h>
#include <SystemConfiguration/SystemConfiguration.h> // for internet connectivity check SCNetworkReachabilityRef



bool cpccInternet::isConnectedToInternet(void)
{
	// https://developer.apple.com/documentation/systemconfiguration/scnetworkreachability-g7d
	// https://developer.apple.com/library/content/documentation/Networking/Conceptual/SystemConfigFrameworks/SC_ReachConnect/SC_ReachConnect.html
	// linking: needs SystemConfiguration.network

	#ifdef TARGET_OS_MAC

		struct sockaddr zeroAddress;
		bzero(&zeroAddress, sizeof(zeroAddress));
		zeroAddress.sa_len = sizeof(zeroAddress);
		zeroAddress.sa_family = AF_INET;

		SCNetworkReachabilityRef reachabilityRef = SCNetworkReachabilityCreateWithAddress(NULL, (const struct sockaddr*)&zeroAddress);


	#elif TARGET_OS_IPHONE

		struct sockaddr_in address;
		size_t address_len = sizeof(address);
		memset(&address, 0, address_len);
		address.sin_len = address_len;
		address.sin_family = AF_INET;

		SCNetworkReachabilityRef reachabilityRef = SCNetworkReachabilityCreateWithAddress(NULL, (const struct sockaddr*)&address);

	#endif
		bool result = false;
		if (reachabilityRef != NULL)
		{
			SCNetworkReachabilityFlags flags = 0;

			if (SCNetworkReachabilityGetFlags(reachabilityRef, &flags))
			{
				bool isReachable = ((flags & kSCNetworkFlagsReachable) != 0);
				bool connectionRequired = ((flags & kSCNetworkFlagsConnectionRequired) != 0);
				result = (isReachable && !connectionRequired);
			}

			CFRelease(reachabilityRef);
		}

		return result;

}
