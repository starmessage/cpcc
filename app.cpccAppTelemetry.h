
/*  *****************************************
 *  File:		app.cpccAppTelemetry.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

// ToDo: these paths must be neutralised to work on any developement environment
#ifdef _WIN32

    #include "../libs/trackerBird/win/Trackerbird.h"
	#if defined _M_X64
		#pragma comment(lib, "F:/work/C-Proj/libs/trackerBird/win/x64/trackerbird.lib")
	#else
		#pragma comment(lib, "F:/work/C-Proj/libs/trackerBird/win/x86/trackerbird.lib")
	#endif
	
#elif defined(__APPLE__)

	#include "../libs/trackerBird/osx/libstdc++/TrackerbirdSDK.h"
    // #include "../libs/trackerBird/osx/libc++/TrackerbirdSDK.h"
#endif





/*  TrackerBird info

-------------------------------
Windows:
-------------------------------
	http://docs.trackerbird.com/CPP/quick-start-guide.html

	//Initialize the Trackerbird Configuration
	wchar_t* purl=L"http://INSERT-YOUR-URL";
	wchar_t* pproduct_id=L"INSERT-YOUR-PROD-ID";
	wchar_t* pproduct_version=L"1.2.3";
	wchar_t* pproduct_build_number=L"4567";
	BOOL pmulti_session_enabled=FALSE;
	tbCreateConfig(purl, pproduct_id, pproduct_version, pproduct_build_number, pmulti_session_enabled);

	//Inform Trackerbird that a new runtime session has been started.
	tbStart();

	//Your program logic...

	//Program closing - inform Trackerbird that this runtime session is closing down.
	tbStop();

	//Your program logic...

-------------------------------
OSX:
-------------------------------
 TrackerbirdSDK::TBConfig config = TrackerbirdSDK::TBConfig(<Callhome URL>,
                                                              <Product ID>,
                                                              <Your product's version> productBuildNumber:<Your product's build number>,
                                                              <Your product's build number>,
                                                              <Multiple Sessions Enabled>,
                                                              <Your product's edition>,
                                                              <Your product's language>,
                                                              <SDK working files path>);
 
 
 You will have to link your application against the following frameworks:
 
 Cocoa.framework
 Foundation.framework
 CoreFoundation.framework
 AppKit.framework
 Security.framework
 SystemConfiguration.framework
 WebKit.framework
 libz.dylib
 IOKit.framework

 */

// helper class
class wchar_from_char
{
private:
	std::wstring wstr;

public:
	wchar_from_char(const char *str)
	{
		int len = strlen(str);
		wstr.insert(0, len+1, L'-');
		#pragma warning(suppress : 4996)
		mbstowcs(&wstr[0], str, len);
	}

	inline  operator const wchar_t *(void) const     { return wstr.c_str(); }
};


// cross platform wrapper class/
// usage: in your main() or other central location of your program, just create a variable out of this class
// e.g.
// cpccAppTelemetryTrackerBird telemetry( "http://xxxxx.tbnet1.com",  "xxxxxx", SM_VERSION,  __DATE__, false, TM_PRODUCT_EDITION);

// todo: support multiple sessions
class cpccAppTelemetryTrackerBird
{
private:
    bool m_multipleSessionsEnabled;
    
public:
	cpccAppTelemetryTrackerBird(const char* callhomeURL,
								const char* productID,
								const char* productVersion,
								const char* productBuildNumber,
                                bool        multiSessionEnabled,
								const char* productEdition) :
                        m_multipleSessionsEnabled(multiSessionEnabled)
    {
		#ifdef _WIN32
			wchar_from_char	callhomeURL_w(callhomeURL),
							productID_w(productID),
							productVersion_w(productVersion),
							productBuildNumber_w(productBuildNumber);

			tbCreateConfig(callhomeURL_w, productID_w, productVersion_w, productBuildNumber_w, m_multipleSessionsEnabled);
			tbStart();
		#elif defined(__APPLE__)
			TrackerbirdSDK::TBConfig config = TrackerbirdSDK::TBConfig(callhomeURL, productID, productVersion, productBuildNumber, multiSessionEnabled, productEdition, "en", "");
			TrackerbirdSDK::TBApp::start(config, NULL);
		#endif
    }
    
    
    ~cpccAppTelemetryTrackerBird()
    {
        //if  (m_multipleSessionsEnabled)
        //    TrackerbirdSDK::TBApp::sessionStop();

		#ifdef _WIN32
			tbStop();
		#elif defined(__APPLE__)
			TrackerbirdSDK::TBApp::stop(NULL);
		#endif
    }
    
    
};


