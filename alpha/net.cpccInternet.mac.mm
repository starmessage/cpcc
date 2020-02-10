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
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
    #include <Foundation/Foundation.h>
#else
    #include <cocoa/cocoa.h>
#endif

// you must also link the framework SystemConfiguration
#include <SystemConfiguration/SystemConfiguration.h> // for internet connectivity check SCNetworkReachabilityRef



bool cpccInternet::isConnectedToInternet(void)
{
	// https://developer.apple.com/documentation/systemconfiguration/scnetworkreachability-g7d
	// https://developer.apple.com/library/content/documentation/Networking/Conceptual/SystemConfigFrameworks/SC_ReachConnect/SC_ReachConnect.html
	// linking on MacOS: needs SystemConfiguration.network
    //           on IOS: needs SystemConfiguration.framework

    // see also:
    // https://developer.apple.com/library/archive/samplecode/Reachability/Listings/Reachability_Reachability_h.html#//apple_ref/doc/uid/DTS40007324-Reachability_Reachability_h-DontLinkElementID_8
	// https://cocoapods.org/pods/Reachability

	#ifdef TARGET_OS_MAC

		struct sockaddr zeroAddress;
		bzero(&zeroAddress, sizeof(zeroAddress));
		zeroAddress.sa_len = sizeof(zeroAddress);
		zeroAddress.sa_family = AF_INET;

		SCNetworkReachabilityRef reachabilityRef = SCNetworkReachabilityCreateWithAddress(NULL, (const struct sockaddr*)&zeroAddress);


	#elif TARGET_OS_IPHONE
		// todo: never compiled ?
		struct sockaddr_in address;
		size_t address_len = sizeof(address);
		memset(&address, 0, address_len);
		address.sin_len = address_len;
		address.sin_family = AF_INET;
		// todo: sockaddr_in instead of sockaddr ?
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





