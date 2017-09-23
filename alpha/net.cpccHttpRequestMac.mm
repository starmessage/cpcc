/*  *****************************************
 *  File:		net.cpccHttpRequestMac.mm
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


#include "net.cpccHttpRequestMac.h"
#include <string>
#include <iostream>
#include <cocoa/cocoa.h>
// #include <SystemConfiguration/SystemConfiguration.h> // for internet connectivity check SCNetworkReachabilityRef





/* async examples
 https://stackoverflow.com/questions/14902102/asynchronous-nsurlconnection-scheme-tutorial
 https://snippets.aktagon.com/snippets/350-how-to-make-asynchronous-http-requests-with-nsurlconnection
 https://github.com/jlindsey/NSURLConnection-Example
 http://iosdevelopmenttip.blogspot.gr/2013/06/nsurlconnection-asynchronous-request.html
 
 */


int cpccHttpPostMac::httpPostAsync(std::atomic<bool> &errorOccured, std::atomic<int> &nPending, const char *postData , const int timeoutInSec)
{
    NSLog(@"httpPostMac()");
    
    if (!postData)
        return 1901;
    
    //if (!internetIsOn())
    //    return 1902;
    
    std::string strPostFullURL(m_postHost);
    strPostFullURL += m_postPath;
    std::string strPostData(postData);
    
    std::cout << "postURL=" << strPostFullURL << std::endl;
    std::cout << "postData=" << strPostData << std::endl;
    
    
    NSString *URLmacstring = [NSString stringWithCString:strPostFullURL.c_str() encoding:NSASCIIStringEncoding];
    NSURL *url = [NSURL URLWithString:URLmacstring];
    if (url==nil)
        return 1999;
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    [request setHTTPMethod:@"POST"];
    
	/* to check claims of other developers about the setTimeoutInterval
			assigned limit works while using local WIFI network
			assigned limit DO NOT work while using sim card and GSM network
		
	*/
	[request setTimeoutInterval:timeoutInSec];
	// [request cachePolicy:NSURLRequestReloadIgnoringCacheData];
	
    if (m_userAgent.length()>0)
        [request setValue:[NSString stringWithCString:m_userAgent.c_str() encoding:NSASCIIStringEncoding] forHTTPHeaderField:@"User-Agent"]; // appears as the "browser" under GA.
    
    // example of headers in one line:
    // [request setAllHTTPHeaderFields:@{@"Accepts-Encoding": @"gzip", @"Accept": @"application/json"}];
    
    NSString *bodyData = [NSString stringWithCString:strPostData.c_str() encoding:NSUTF8StringEncoding];;
    [request setHTTPBody:[NSData dataWithBytes:[bodyData UTF8String] length:strlen([bodyData UTF8String])]];
    
    if (![NSURLConnection canHandleRequest:request])
    {
        std::cout << "Cannot handle the http request" << std::endl;
        return 1900;
    }
    
    
    NSOperationQueue *mainQueue = [[NSOperationQueue alloc] init];
    [mainQueue setMaxConcurrentOperationCount:5];
    
	nPending++;
    [NSURLConnection sendAsynchronousRequest:request queue:mainQueue completionHandler:^(NSURLResponse *response, NSData *responseData, NSError *error) {
        
            NSHTTPURLResponse *urlResponse = (NSHTTPURLResponse *)response;
			nPending--;
            // asynResult = (int) urlResponse.statusCode;
            if (!error)
                {
				
                NSLog(@"Status Code: %li", (long)urlResponse.statusCode);
                // NSLog(@"Response Body: %@", [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding]);
                }
            else
                {
                    errorOccured = true;
                    NSLog(@"An error occured, Status Code: %li  %@", (long)urlResponse.statusCode, [NSHTTPURLResponse localizedStringForStatusCode:urlResponse.statusCode]);
                    NSLog(@"Description: %@", [error localizedDescription]);
                    // NSLog(@"Response Body: %@", [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding]);
                }
            }
     ];
    
    return 299; // ok but pending tasks
}


/*
 
 https://stackoverflow.com/questions/3566516/simple-http-post-example-in-objective-c
 
 // In body data for the 'application/x-www-form-urlencoded' content type,
 // form fields are separated by an ampersand. Note the absence of a
 // leading ampersand.
 NSString *bodyData = @"name=Jane+Doe&address=123+Main+St";
 
 NSMutableURLRequest *postRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"https://www.apple.com"]];
 
 // Set the request's content type to application/x-www-form-urlencoded
 [postRequest setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
 
 // Designate the request a POST request and specify its body data
 [postRequest setHTTPMethod:@"POST"];
 [postRequest setHTTPBody:[NSData dataWithBytes:[bodyData UTF8String] length:strlen([bodyData UTF8String])]];
 
 // Initialize the NSURLConnection and proceed as described in
 // Retrieving the Contents of a URL
 
 */

int cpccHttpPostMac::httpPost(const char *postData, const int timeoutInSec)
{
    NSLog(@"httpPostMac()");
    
    if (!postData)
        return 1901;
    
    //if (!internetIsOn())
    //    return 1902;
    
    std::string strPostFullURL(m_postHost);
    strPostFullURL += m_postPath;
    std::string strPostData(postData);
    std::cout << "postURL=" << strPostFullURL << std::endl;
    std::cout << "postData=" << strPostData << std::endl;
    
    
    NSString *URLmacstring = [NSString stringWithCString:strPostFullURL.c_str() encoding:NSASCIIStringEncoding];
    NSURL *url = [NSURL URLWithString:URLmacstring];
    if (url==nil)
        return 1999;
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    [request setHTTPMethod:@"POST"];
    [request setTimeoutInterval:timeoutInSec];
    
    if (m_userAgent.length()>0)
        [request setValue:[NSString stringWithCString:m_userAgent.c_str() encoding:NSASCIIStringEncoding] forHTTPHeaderField:@"User-Agent"]; // appears as the "browser" under GA.
    
    // example of headers in one line:
    // [request setAllHTTPHeaderFields:@{@"Accepts-Encoding": @"gzip", @"Accept": @"application/json"}];

    NSString *bodyData = [NSString stringWithCString:strPostData.c_str() encoding:NSUTF8StringEncoding];;
    [request setHTTPBody:[NSData dataWithBytes:[bodyData UTF8String] length:strlen([bodyData UTF8String])]];

    if (![NSURLConnection canHandleRequest:request])
    {
        std::cout << "Cannot handle the http request" << std::endl;
        return 1900;
    }
    
    NSHTTPURLResponse *response; // NSURLResponse *response;
    NSError *err;
    NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&err];
    if (responseData==nil)
        std::cout << "Possible timeout" << std::endl;
    //if (responseData==nil)  // an piasei to timeout, bgainoume edo
    //    return 1997;
    
    int httpStatusCode = (int) [response statusCode]; // check for 2xx
    NSLog(@"httpStatusCode: %d", httpStatusCode);
    
    // NSDictionary* receivedHeaders = [(NSHTTPURLResponse *)response allHeaderFields];
    // now query `headers` for the header you want
    // NSString *strHeader = [[NSString alloc] initWithData:receivedHeaders encoding:NSUTF8StringEncoding];
    // NSLog(@"receivedHeaders as String: %@", receivedHeaders);
    
    return httpStatusCode;
}





