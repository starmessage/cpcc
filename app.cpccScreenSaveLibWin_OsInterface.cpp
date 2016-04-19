/*  *****************************************
 *  File:		app.cpccScreenSaveLibWin_OsInterface.cpp
 *	Purpose:	Portable (cross-platform), light-weight, screensaver helper class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *              https://code.google.com/p/cpcc/
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
//////////////////////////////////////////
//
// engine configuration
//
#define USE_WM_PAINT	1
#define FramesPerSec	25
//
//////////////////////////////////////////

#include "io.cpccLog.h"
#define _WINSOCKAPI_    // stops windows.h including winsock.h

#pragma warning( push )
#pragma warning( disable : 4005 )
#include <windows.h>
#pragma warning( pop )

#include <ScrnSave.h>
#include <commctrl.h>
#include "app.cpccScreenSaverInterface.h"
#include "app.cpccApp.h"
#include "core.cpccOS.h"


/*  Handling Screen Savers
	http://msdn.microsoft.com/en-us/library/cc144066%28v=vs.85%29.aspx
*/

/*  command line arguments:
 ScreenSaver           - Show the Settings dialog box.
 ScreenSaver /c        - Show the Settings dialog box, modal to the foreground window.
 ScreenSaver /c <HWND> - Show the Settings dialog box, as child to the  HWND window.
 ScreenSaver /p <HWND> - Preview Screen Saver as child of window <HWND>.
 ScreenSaver /s        - Run the Screen Saver.
 ScreenSaver /sm	   - emulate multi-monitor

 Multiple monitors:
 1) if you've only got a single monitor, you can still test multi-monitor support with "fake" monitors 
 by running your saver with argument
    mysaver.scr /sm         // test multi-monitor
 
 2) Example of two monitors, where the main monitor A is 1920 x 1080 
    the 2nd monitor 1280x1024, is placed on the left of the main monitor
	the OS passes a single window handle to cover the merged area of both monitors.
	Window rect: TopLeft:-74,-1280 screen width:3200, height:1154
	Monitor 0: Left 0, top 0, right 1920, bottom 1080
	Monitor 1: Left -1280, top -74, right 0, bottom 950
 
	These are cases where the top left is not at 0,0
 */


#pragma comment(lib, "comctl32.lib")
#ifdef UNICODE
	#pragma comment(lib, "ScrnSavw.lib")
#else
	#pragma comment(lib, "ScrnSave.lib")
	// the windows SDK 6.0A (comes with vs2008) creates an error under XP: changewindowmessagefilter user32.dll
	// SDK 7.0 is OK
	
#endif
//	If using Dev-C++ in the pragma comment change 'scrnsave.lib' to 'libscrnsave.a'



/*
	http://msdn.microsoft.com/en-us/library/windows/desktop/bb762098%28v=vs.85%29.aspx
	The ScreenSaverProc function must be exported by including it in the EXPORTS statement in the application's module-definition (.def) file.
	------------------------------
	When exporting this function in the module definition (.def) file you must consider 
	that there are two of them: the ANSI and the Unicode version, so you must export 
	either ScreenSaverProcA or ScreenSaverProcW depending on whether you are #defining UNICODE or not. 

	Also, it is unclear why exporting is necessary: without exporting it works anyway. 
	------------------------------
	This is how it is defined by microsoft:
	#ifdef UNICODE
		LRESULT WINAPI ScreenSaverProcW (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		#   define  ScreenSaverProc ScreenSaverProcW
	#else
		LRESULT WINAPI ScreenSaverProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	#endif

*/
LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{	// http://www.cityintherain.com/howtoscr.html

	static UINT_PTR	uTimer=NULL; /* timer identifier */
	static bool		isDrawing=false;
	static cpccScreenSaverInterface* screensaverPtr=NULL;
	static logObjectLife  logFileMarker("logFileMarker-D");
	
	/*
	// idea from
	// http://www.wischik.com/scr/howtoscr.html#ScrPrevCode
	HWND hwnd = cpccOS::getWindowHandleOfProgram("Notepad");
	if (!hwnd)
		hwnd = aHwnd;
	*/

	switch(wMessage)
		{

		case WM_CREATE: // Set a window timer for the screen saver window. 
						// Perform any other required initialization.	
			infoLog().add("ScreenSaverProc() received WM_CREATE");
			infoLog().addf("notepad window handle: %i", cpccOS::getWindowHandleOfProgram("Notepad"));
			/*	to run the screensaver under the notepad's window:
				open notepad, run the screensaver normally, open the log file and get the notepad's HWND
				run:
				myscreensaver.scr /p HWND_number_from_log
			*/

			if (!screensaverPtr)
			{
				screensaverPtr = cpccScreenSaverFactory::createScreenSaver();
				// something is quite not ready here in windows8 and the getDC does not work very well.
				// So I moved the initWithWindowHandle() into the WM_ERASEBKGND event
				// screensaverPtr->initWithWindowHandle( hwnd, 0);
			}
	
			if (screensaverPtr)
			{
				static bool		screensaverWindowInitialised = false;
				screensaverPtr->m_framesPerSec = FramesPerSec;
				if (!screensaverWindowInitialised)
				{
					int	monitorID = 0;
					cpccApp	app;
					cpcc_stringList args;
					app.getArgcArgv(args);
					if (args.size() > 1)
						monitorID = args[1] == "/p" ? -1 : 0;
					screensaverPtr->initWithWindowHandle(hwnd, monitorID);
					screensaverWindowInitialised = true;
				}
				
			}


			// Last job: Set the timer
            uTimer = SetTimer(hwnd, 1, 1000/FramesPerSec, NULL); 
            return 0;
		
			
		case WM_ERASEBKGND:	
			{
				logFunctionLife _tmpLog("WM_ERASEBKGND");
			/*
			// http://msdn.microsoft.com/en-us/library/cc144066%28v=vs.85%29.aspx#Using_the_Screen_Sav
			// The WM_ERASEBKGND message is issued before the WM_TIMER message, allowing the screen saver to paint the background as appropriate. 

            hdc = GetDC(hwnd); 
            GetClientRect (hwnd, &rc); 
            FillRect (hdc, &rc, GetStockObject(BLACK_BRUSH)); 
            ReleaseDC(hwnd,hdc); 
            break; 

			In the preview window, the WM_ERASEBKGND does not come when the screensaver is starting. 
			Only WM_CREATE is received.
			In normal operation (desktop window) the WM_ERASEBKGND comes after the WM_CREATE
			*/
				// About WM_ERASEBKGND: wParam holds the HDC;
				// erases the background in the case of password protected running
				// when the user cancels the password entry dialog.
			
			if (screensaverPtr)
				screensaverPtr->backgroundWasInvalidatedByOS();
			
			/*
			// the typical erase background is:
			{ RECT tmpRect;
				
			GetClientRect(hwnd, &tmpRect); 
			FillRect((HDC) wParam, &tmpRect, (HBRUSH) GetStockObject(BLACK_BRUSH));
			}
			*/

			//break;
			return 1L; //An application should return nonzero (1L) if it erases the background
			}

		case WM_CLOSE:	// An application can prompt the user for confirmation, 
						// prior to destroying a window, by processing the 
						// WM_CLOSE message and calling the DestroyWindow function 
						// only if the user confirms the choice. 
			infoLog().add("ScreenSaverProc() received WM_CLOSE");
			break;
		
		case WM_PAINT:
			//ns_cmiLog::cmiQLog().put() << "WM_PAINT";
			#if (USE_WM_PAINT)
				isDrawing = true;
				if (screensaverPtr)
					screensaverPtr->drawOneFrame();
				
				PAINTSTRUCT ps;
				BeginPaint(hwnd,&ps);

				if (screensaverPtr)
					screensaverPtr->flushOneFrame();
				
				EndPaint(hwnd,&ps);
				isDrawing = false;
			#endif
			//ns_cmiLog::cmiQLog().put() << "WM_PAINT exiting";
			break;

		case WM_TIMER:	

			// if a tick from a different timer arrives, ignore it.
			if (wParam!=uTimer)
				return 1;
			
			//infoLog().add("ScreenSaverProc() received WM_TIMER");

			if (isDrawing)
				return 0;

			if (screensaverPtr)
			{
				static const float animatePeriod_inSec = 1.0f / FramesPerSec;
				screensaverPtr->animateOneFrame(animatePeriod_inSec);

				#if (USE_WM_PAINT)
					// ask windows for a redraw
					RECT tmpRect;
					GetClientRect(hwnd, &tmpRect);
					InvalidateRect(hwnd, &tmpRect, false);
				#else
					isDrawing = true;
					screensaverPtr->drawOneFrame();
					screensaverPtr->flushOneFrame();
					isDrawing = false;
				#endif
			}

			return 0l; 

		case WM_DESTROY:	
			// Destroy the timer(s) created when the application processed the WM_CREATE message. 
			// Perform any additional required cleanup.
			infoLog().add("ScreenSaverProc() received  WM_DESTROY");

			if (uTimer)  
				KillTimer(hwnd, uTimer); 
			uTimer=NULL;			 
            
			if (screensaverPtr)
			{
				screensaverPtr->shutDown();
				delete(screensaverPtr);
				screensaverPtr=NULL;
			}

			// PostQuitMessage(0);
			// isos edo thelei break gia na kalestei i DefScreenSaverProc() na kanei to sosto cleanup
			break;	//return 0l;  



		
		
		case WM_MOUSEMOVE:			// mouse trap for the first 3 seconds
			//if (Snoozed) break;		// this rule does not apply when snoozed.
			
			// gia na mi kleinei o screensaver amesos apo tin arxi
			//if (GlobalTime < 4 * 1000/ 60  ) return 0;  // 60 is a proximation of RealTickTime
			break;

/*		case WM_ACTIVATEAPP: if( wParam ) break;  */
		case WM_LBUTTONDOWN: 
        case WM_MBUTTONDOWN: 
        case WM_RBUTTONDOWN: 
        case WM_SYSKEYDOWN: 
		case WM_KEYDOWN: // if the termination of the saver is because of KEYDOWN....
			//if (!Snoozed) TimeoutCorrection.Stop();
			break;
			
		}

	return DefScreenSaverProc(hwnd, wMessage, wParam, lParam);
}



BOOL WINAPI RegisterDialogClasses( HANDLE  /* hInst */ )
{	
	return true;
}



BOOL WINAPI ScreenSaverConfigureDialog (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
/* 
http://msdn.microsoft.com/en-us/library/bb762097%28v=vs.85%29.aspx
http://msdn.microsoft.com/en-us/library/cc144066%28v=vs.85%29.aspx

If the function successfully processes the message, it should return TRUE. 
If not, it should return FALSE
The ScreenSaverConfigureDialog function must be exported by including it 
in the EXPORTS statement in the application's module-definition (.def) file.

Prepei na yparxei ena dialog template me to parakato ID
(mesa sto resource.h)
#define DLG_SCRNSAVECONFIGURE           2003

*/	
	//static cpccScreenSaverInterface* screensaverPtr=NULL;
	cpccScreenSaverInterface* screensaverPtr = NULL;
    switch(message) 
    { 
		case WM_INITDIALOG: 
			infoLog().add("ScreenSaverConfigureDialog received WM_INITDIALOG");
			/* In response to a WM_INITDIALOG message, 
			ScreenSaverConfigureDialog should return FALSE if it calls 
			the SetFocus function to set the keyboard focus to one of 
			the controls in the dialog box. 
			Otherwise, the function should return TRUE, in which case the 
			system sets the keyboard focus to the first control in the 
			dialog box that can be given the focus. */
			
			
			//MessageBox(NULL,"WM_INITDIALOG",NULL,NULL); 
			/*
			if (!screensaverPtr)
			{
				screensaverPtr = cpccScreenSaverFactory::createScreenSaver();
			}
			
			*/

			screensaverPtr = cpccScreenSaverFactory::createScreenSaver();
			if (screensaverPtr)
				if (screensaverPtr->hasConfigureSheet())
					screensaverPtr->showConfigureSheet(hDlg);
			
			delete(screensaverPtr);

			PostMessage(hDlg,WM_COMMAND, IDOK, NULL);
			return TRUE; 


        case WM_COMMAND: 
            switch(LOWORD(wParam)) 
            { 								
                case IDOK:
					EndDialog(hDlg, LOWORD(wParam) == IDOK);
					return TRUE;

                case IDCANCEL:  // common ending for OK and CANCEL
					EndDialog(hDlg, LOWORD(wParam) == IDOK); 
					return TRUE; 
			} 
    } 
    return TRUE; 
}
