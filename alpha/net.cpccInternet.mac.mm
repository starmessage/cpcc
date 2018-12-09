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

#include <SystemConfiguration/SystemConfiguration.h> // for internet connectivity check SCNetworkReachabilityRef



bool cpccInternet::isConnectedToInternet(void)
{
	// https://developer.apple.com/documentation/systemconfiguration/scnetworkreachability-g7d
	// https://developer.apple.com/library/content/documentation/Networking/Conceptual/SystemConfigFrameworks/SC_ReachConnect/SC_ReachConnect.html
	// linking on MacOS: needs SystemConfiguration.network
    //           on IOS: needs SystemConfiguration.framework
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

// e..g downloadSync("http://www.google.com/intl/en_ALL/images/srpr/logo1w.png", "c:\file.png");
bool cpccInternet::downloadToFile(const cpcc_char *aUrl, const cpcc_char *saveTo, const long maxSize, long &downloadedBytes)
{
	/* 
	https://developer.apple.com/documentation/foundation/url_loading_system?language=objc
	https://developer.apple.com/documentation/foundation/url_loading_system/fetching_website_data_into_memory?language=objc

	For small interactions with remote servers, you can use the NSURLSessionDataTask class to 
	receive response data into memory (as opposed to using the NSURLSessionDownloadTask class, 
	which stores the data directly to the file system). 
	A data task is ideal for uses like calling a web service endpoint.
	*/


	NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:aUrl]];
    /*
     Don't use this synchronous method to request network-based URLs. For network-based URLs,
     this method can block the current thread for tens of seconds on a slow network,
     resulting in a poor user experience, and in iOS, may cause your app to be terminated.
     
     Instead, for non-file URLs, consider using the dataTaskWithURL:completionHandler:
     method of the NSURLSession class.
     */
    // NSData *data = [[NSData alloc] initWithContentsOfURL:url options:(NSDataReadingOptions) error:<#(NSError **)#>];
    NSData *data = [[NSData alloc] initWithContentsOfURL:url];
    
    [data writeToFile:[NSString stringWithUTF8String:saveTo] atomically:YES];
    [data release];
    return true;
    
	// https://stackoverflow.com/questions/26851121/using-nsurlsessiondownloadtask-to-display-an-image
	// https://stackoverflow.com/questions/47935498/nsurlsessiondownloadtask-how-to-change-the-location-where-tem-files-are-saved
    /*
     you can get the contents of a local file URL as an NSString object using the initWithContentsOfURL:encoding:error: initializer, or as an NSData object using the initWithContentsOfURL:options:error: initializer.
     */
    
    /*
	NSURLSessionDownloadTask *downloadTask = [[NSURLSession sharedSession]
 			downloadTaskWithURL:url completionHandler:^(NSURL *location, NSURLResponse *response, NSError *error) 
 			{
  			// Handle the response here
 			
 			// if error, handle it and quit
			if (error) 
			{
        		NSLog(@"downloadTaskWithRequest failed: %@", error);
        		return false;
    		}

  			NSFileManager *fileManager = [NSFileManager defaultManager];
    		NSString *destinationFileName = downloadTask.originalRequest.URL.lastPathComponent;
    		NSURL *destinationURL = [self.downloadDirURL URLByAppendingPathComponent:destinationFileName];
    if([fileManager fileExistsAtPath:[destinationURL path]])
    {
        [fileManager removeItemAtURL:destinationURL error:nil];
    }
    BOOL success = [fileManager moveItemAtURL:location toURL:destinationURL error:&error];
			}];

	// All of the the different tasks from NSURLSession start in a suspended state. Start the task here.	
	[downloadPhotoTask resume];
     */
    
}


bool cpccInternet::downloadToBuffer(const cpcc_char *aUrl, const char *aBuffer, const long bufferSize, long &downloadedBytes)
{
    return false;
}
