
/*  *****************************************
 *  File:		core.cpccOS.h
 *	Purpose:	Portable (cross-platform), light-weight, OS functions
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

#ifdef _WIN32
	#include <windows.h>
#endif
#ifdef __APPLE__
    #import <IOKit/pwr_mgt/IOPMLib.h>
    // you need to link the frameworks CoreFoundation, IOKit
#endif

class cpccComputerPower
{
private:
	#ifdef __APPLE__
		IOPMAssertionID preventSleepAssertionID;
	#endif

public:
	cpccComputerPower()
		#ifdef __APPLE__
				: preventSleepAssertionID(0)
		#endif
    { }
    

	const bool preventIdleSystemSleep(const char *textualReason)
	{
#ifdef _WIN32
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa373208%28v=vs.85%29.aspx
		return SetThreadExecutionState(
			// ES_AWAYMODE_REQUIRED |  // This value must be specified with ES_CONTINUOUS.

			ES_CONTINUOUS | // Informs the system that the state being set should remain in effect 
							// until the next call that uses ES_CONTINUOUS and one of the other state flags is cleared. 
							// Calling SetThreadExecutionState without ES_CONTINUOUS simply resets the idle timer; 
							// to keep the display or system in the working state, the thread must call SetThreadExecutionState periodically.

			// ES_DISPLAY_REQUIRED | // Forces the display to be on by resetting the display idle timer. 

			ES_SYSTEM_REQUIRED		// Forces the system to be in the working state by resetting the system idle timer. 
		) != NULL;


#elif __APPLE__
		// IOPMAssertionCreateWithName is new API available in Mac OS X 10.6 Snow Leopard. 
		// IOPMAssertionCreateWithName allows an application to return a return a brief string to the user explaining why that application is preventing sleep.
		//  https://developer.apple.com/library/mac/qa/qa1340/_index.html
		//  kIOPMAssertionTypePreventUserIdleDisplaySleep (only available on 10.7 or later)
		//  kIOPMAssertionTypeNoDisplaySleep - prevents display sleep AND idle sleep,
		//  kIOPMAssertionTypeNoIdleSleep - prevents idle sleep of the MAC (the screen can turn off)
		//  NOTE: IOPMAssertionCreateWithName limits the string to 128 characters.
		CFStringRef reasonForActivity;
		reasonForActivity = CFStringCreateWithCString(NULL, textualReason, kCFStringEncodingASCII);
		IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoIdleSleep,
			kIOPMAssertionLevelOn,
			reasonForActivity,
			&preventSleepAssertionID);

		CFRelease(reasonForActivity);
		return (success == kIOReturnSuccess);

#else
#error #8724: Unknown platform for cpccOS
		return false;
#endif
	}

    

	const bool preventMonitorSleep(const char *textualReason)
	{
		#ifdef _WIN32
				// https://msdn.microsoft.com/en-us/library/windows/desktop/aa373208%28v=vs.85%29.aspx
				return SetThreadExecutionState(
					// ES_AWAYMODE_REQUIRED |  // This value must be specified with ES_CONTINUOUS.

					ES_CONTINUOUS | // Informs the system that the state being set should remain in effect 
									// until the next call that uses ES_CONTINUOUS and one of the other state flags is cleared. 
									// Calling SetThreadExecutionState without ES_CONTINUOUS simply resets the idle timer; 
									// to keep the display or system in the working state, the thread must call SetThreadExecutionState periodically.
					
					ES_DISPLAY_REQUIRED | // Forces the display to be on by resetting the display idle timer. 

					ES_SYSTEM_REQUIRED		// Forces the system to be in the working state by resetting the system idle timer. 
					) != NULL;


		#elif __APPLE__
				// IOPMAssertionCreateWithName is new API available in Mac OS X 10.6 Snow Leopard. 
				// IOPMAssertionCreateWithName allows an application to return a return a brief string to the user explaining why that application is preventing sleep.
				//  https://developer.apple.com/library/mac/qa/qa1340/_index.html
				//  kIOPMAssertionTypePreventUserIdleDisplaySleep (only available on 10.7 or later)
				//  kIOPMAssertionTypeNoDisplaySleep - prevents display sleep AND idle sleep,
				//  kIOPMAssertionTypeNoIdleSleep - prevents idle sleep of the MAC (the screen can turn off)
				//  NOTE: IOPMAssertionCreateWithName limits the string to 128 characters.
				CFStringRef reasonForActivity;
				reasonForActivity = CFStringCreateWithCString(NULL, textualReason, kCFStringEncodingASCII);
				IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
					kIOPMAssertionLevelOn,
					reasonForActivity,
					&preventSleepAssertionID);

				CFRelease(reasonForActivity);
				return (success == kIOReturnSuccess);

		#else
			#error #8724: Unknown platform for cpccOS
				return false;
		#endif
	}


	
	const bool restorePowerSettings(void)
	{
		#ifdef _WIN32
			SetThreadExecutionState(ES_CONTINUOUS);
			return true;

		#elif __APPLE__
			if (preventSleepAssertionID == 0) return
				false;
			IOReturn success = IOPMAssertionRelease(preventSleepAssertionID);
			preventSleepAssertionID = 0;
			return (success == kIOReturnSuccess);
		#else
		#error #8724: Unknown platform for cpccOS
			return false;
		#endif
	}

	/*
	void cpccOS::keepAwakeTrigger(void)
	{
	#ifdef _WIN32

	#elif __APPLE__
	create an NSTimer that fires a function with this
	UpdateSystemActivity(OverallAct);

	// removed document:
	// https://developer.apple.com/library/mac/qa/qa1160/_index.html
	// 'UpdateSystemActivity' is deprecated: first deprecated in OS X 10.8
	// all these parameters stop the screensaver from running
	// UpdateSystemActivity(OverallAct);
	// UpdateSystemActivity(IdleActivity);
	// UpdateSystemActivity(HDActivity);
	// UpdateSystemActivity(UsrActivity);
	UpdateSystemActivity(NetActivity);

	#else
	#error #8724f: Unknown platform for cpccOS

	#endif
	}
	*/


};
