﻿
/*  *****************************************
 *  File:		app.cpccAppTelemetryTrackerbird.h
 *	Purpose:	Portable (cross-platform), light-weight, library
  *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales (at) starmessage.info
 *	*****************************************
 */

#pragma once

#include "cpccUnicodeSupport.h"
#include "core.cpccErrorCollector.h"

#include "io.cpccLog.h"

// ToDo: these paths must be neutralised to work on any developement environment
//		 This is solved if the TB files are stored in the IDE's search path
#ifdef _WIN32

    #include "../libs/trackerBird/win/Trackerbird.h"
	#if defined _M_X64
		#pragma comment(lib, "F:/work/C-Proj/libs/trackerBird/win/x64/trackerbird.lib")
	#else
		#pragma comment(lib, "F:/work/C-Proj/libs/trackerBird/win/x86/trackerbird.lib")
	#endif
	
#elif defined(__APPLE__)

	//#include "../libs/trackerBird/osx/libstdc++/TrackerbirdSDK.h"
    #include "../libs/trackerBird/osx/libc++/TrackerbirdSDK.h"
#endif





/*  TrackerBird info

-------------------------------
Windows:
-------------------------------
	version 4 docs:
	http://devzone.revulytics.com/docs/v4/CPP/windows/quick-start-guide.html

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
	version 4 Mac docs
	http://devzone.revulytics.com/docs/v4/CPP/mac/quick-start-guide.html
	
 TrackerbirdSDK::TBConfig config = TrackerbirdSDK::TBConfig(<Callhome URL>,
                                                              <Product ID>,
                                                              <Your product's version>,
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


// cross platform wrapper class/
// usage: in your main() or other central location of your program, just create a variable out of this class
// e.g.
// cpccAppTelemetryTrackerBird telemetry( "http://xxxxx.tbnet1.com",  "xxxxxx", SM_VERSION,  __DATE__, false, TM_PRODUCT_EDITION);

// todo: support multiple sessions
class cpccAppTelemetryTrackerBird 
{
private:
    bool	m_multipleSessionsEnabled= false;
	bool	m_started = false,
			m_stopped  = false;
	cpccErrorCollector	&m_errorDump;

public:	// ctors
    cpccAppTelemetryTrackerBird(const char* callhomeURL,
                                const char* productID,
                                const char* productVersion,
                                const char* productBuildNumber,
                                const char* productEdition,
								const cpcc_char* cacheFolder,
								cpccErrorCollector	&anErrorDump
								) :
			m_errorDump(anErrorDump)
    {
		#ifdef _WIN32
		{
			debugLog().addf("trackerbird cache folder:%s", cacheFolder);
			wchar_from_char	_pathForCache_w(cacheFolder);

            TBRESULT res = tbSetFilePath(_pathForCache_w);
            if (res == TB_INVALID_PATH)
                warningLog().addf(_T("TB invalid path:%s"), cacheFolder);
            if (res == TB_ACCESS_DENIED)
                warningLog().addf(_T("TB access denied to path:%s"), cacheFolder);
        
			wchar_from_char	callhomeURL_w(callhomeURL),
							productID_w(productID),
							productVersion_w(productVersion),
							productBuildNumber_w(productBuildNumber),
							productEdition_w(productEdition);
        
			tbCreateConfig(callhomeURL_w, productID_w, productVersion_w, productBuildNumber_w, m_multipleSessionsEnabled);
			
			// TBRESULT tbSetProductEdition(const wchar_t* productEdition)
			// This function allows you to set the edition of your product. 
			// An example of this would be when a single product can be licensed/run 
			// in different modes such as as “Home” and “Business”.
			res = tbSetProductEdition(productEdition_w);
			if (res == TB_CONFIG_NOT_CREATED)
			    warningLog().add(_T("tbSetProductEdition() returned TB_CONFIG_NOT_CREATED"));
            if (res == TB_INVALID_PARAMETER)
			    warningLog().add(_T("tbSetProductEdition() returned TB_INVALID_PARAMETER"));
				
			tbStart();
		}
		#elif defined(__APPLE__)
            /*
             @note The file path where the Trackerbird SDK will create and save its working files. It is important to remember that the calling process should have read/write accessibility to the location.
             
             @note The default location is as following:
             
             @note * For non-sandboxed apps: `~/Library/Application Support/`
             @note * For sandboxed apps: `~/Library/Containers/<Bundle Identifier>/Data/Library/Application Support/`
             
             @note The SDK will create and keep its working files within the following direcotry structure: `<filePath>/TrackerbirdSDK/<PorductID Hash>/`

             */
            TrackerbirdSDK::TBConfig config = TrackerbirdSDK::TBConfig(callhomeURL, productID, productVersion, productBuildNumber, false, productEdition, "en", "");
            TrackerbirdSDK::TBApp::start(config, NULL);
            
		#endif
    }
    
    
    ~cpccAppTelemetryTrackerBird()
    {
        logFunctionLife			_tmpLog("~cpccAppTelemetryTrackerBird()");

		//if  (m_multipleSessionsEnabled)
		//    TrackerbirdSDK::TBApp::sessionStop();
		#ifdef _WIN32
			tbStop();
		#elif defined(__APPLE__)
			TrackerbirdSDK::TBApp::stop(NULL);
		#endif
    }



public: // functions

    
	void trackEvent(const char *eventCategory, const char *eventName)
	{
	/*
		windows: 	http://docs.trackerbird.com/CPP/event-tracking.html
		TBRESULT tbEventTrack(const wchar_t* category, const wchar_t* eventName, const wchar_t* sessionID, [BOOL allowExtendedNames=FALSE])
	
	
		http://blog.trackerbird.com/content/getting-started-with-trackerbird-part2-event-tracking/#sthash.ydaejNDj.dpuf
		
		TBApp.EventTrack("Feature Usage", eventName, null); 
		TBApp.EventTrack accepts 3 string parameters, the category of the event being tracked, 
		the name of the event, and the session ID. The category is used to group events in the 
		reports and in this example we have called it “Feature Usage”
		
		TBApp.EventTrackTxt("Feedback", "User Comments", txtFeedback.Text, null);     
		TBApp.EventTrackTxt accepts 4 parameters, the category of the event being tracked, 
		the name of the event, a string value, and the session ID. 
		In this example we have called the category “Feedback” and the event name “User Comments”.

    */
		#ifdef _WIN32
			wchar_from_char	eventCategory_w(eventCategory),
							eventName_w(eventName);
			
			//TBRESULT result = 
			tbEventTrack(eventCategory_w, eventName_w, NULL);
			// return (result==TB_OK);
			
		#elif defined(__APPLE__)
			TrackerbirdSDK::TBApp::eventTrack(eventCategory, eventName, NULL);
            //TrackerbirdSDK::TBApp::EventTrackTxt(eventCategory, eventName, "#9594 text from user", null);
			
		#endif
    }

};


