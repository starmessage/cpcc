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
#define FramesPerSec	40
//
//////////////////////////////////////////


#include <windows.h>
#include <ScrnSave.h>
#include <commctrl.h>
#include "io.cpccLog.h"
#include "app.cpccScreenSaverInterface.h"
#include "app.cpccApp.h"
#include "core.cpccOS.h"



/*  
http://msdn.microsoft.com/en-us/library/cc144066%28v=vs.85%29.aspx
http://www.dreamincode.net/forums/topic/17214-write-your-own-windows-screen-saver/

http://social.msdn.microsoft.com/forums/en-US/vcgeneral/thread/3f2544bd-698d-4e96-a784-10d2bc92c23f/

minimal example:
http://ohwhsmm7.web.fc2.com/win/Scrnsave.cpp

minimal with openGL
http://www.windows-tech.info/17/0f61ee9913615fc5.php

*/

/*  command line arguments:
 ScreenSaver           - Show the Settings dialog box.
 ScreenSaver /c        - Show the Settings dialog box, modal to the foreground window.
 ScreenSaver /p <HWND> - Preview Screen Saver as child of window <HWND>.
 ScreenSaver /s        - Run the Screen Saver.
 ScreenSaver /sm	   - emulate multi-monitor

 Note: if you've only got a single monitor, you can still test multi-monitor support with "fake" monitors by running your saver with argument
    mysaver.scr /sm         // test multi-monitor

 */


#pragma comment(lib, "comctl32.lib")
#ifdef UNICODE
	#pragma comment(lib, "ScrnSavw.lib")
#else
	#pragma comment(lib, "ScrnSave.lib")
	// the SDK 6.0A (comes with vs2008) creates an error under XP: changewindowmessagefilter user32.dll
	// SDK 7.0: OK
	
	// here is what the scrnsave.lib is equivalent with (winmain)
	//		http://www.wischik.com/scr/howtoscr.html#WinMainCode
	
#endif
/*
If using Dev-C++ in the pragma comment change 'scrnsave.lib'
to 'libscrnsave.a' without the quotes.
*/ 


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
	static cpccApp	app;	
	static cpccScreenSaverInterface_PerMonitor* ssNewPtr=NULL;

	switch(wMessage)
		{

		case WM_CREATE: // Retrieve any initialization data from the file REGEDIT.INI.  
						// Set a window timer for the screen saver window. 
						// Perform any other required initialization.	
			infoLog().add("ScreenSaverProc() received WM_CREATE");

			if (!ssNewPtr)
			{
				ssNewPtr = createScreenSaver();
				ssNewPtr->initWithWindowHandle( hwnd, 0);
			}

			// Last job: Set the timer
            uTimer = SetTimer(hwnd, 1, 1000/FramesPerSec, NULL); 
            return 0;
		
		
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
				if (ssNewPtr)
					ssNewPtr->drawOneFrame();
				
				PAINTSTRUCT ps;
				BeginPaint(hwnd,&ps);

				if (ssNewPtr)
					ssNewPtr->flushOneFrame();
				
				EndPaint(hwnd,&ps);
				isDrawing = false;
			#endif
			//ns_cmiLog::cmiQLog().put() << "WM_PAINT exiting";
			break;

		case WM_TIMER:	

			// if a tick from a different timer arrives, ignore it.
			if (wParam!=uTimer)
				return 1;

			if (isDrawing)
				return 0;

			if (ssNewPtr)
			{
				ssNewPtr->animateOneFrame();

				#if (USE_WM_PAINT)
					// ask windows for a redraw
					RECT tmpRect;
					GetClientRect(hwnd, &tmpRect);
					InvalidateRect(hwnd, &tmpRect, false);
				#else
					isDrawing = true;
					ssNewPtr->drawOneFrame();
					ssNewPtr->flushOneFrame();
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
            
			if (ssNewPtr)
			{
				ssNewPtr->shutDown();
				delete(ssNewPtr);
				ssNewPtr=NULL;
			}

			/*		
			if (desktop_hDC)
				ReleaseDC(desktop_hWnd, desktop_hDC);
			desktop_hDC = NULL;
			*/

			// PostQuitMessage(0);
			// isos edo thelei break gia na kalestei i DefScreenSaverProc() na kanei to sosto cleanup
			break;	//return 0l;  


		case WM_ERASEBKGND:	
			
				// About WM_ERASEBKGND: wParam holds the HDC;
				// erases the background in the case of password protected running
				// when the user cancels the password entry dialog.
			infoLog().add("ScreenSaverProc() received WM_ERASEBKGND");

			if (ssNewPtr)
				ssNewPtr->fadeoutUsersDesktop();

			// debugging WM_ERASEBKGND
			/*
			{	RECT tmpRect = { 10,20, 60, 80 };
				FillRect((HDC) wParam, &tmpRect, (HBRUSH) GetStockObject(LTGRAY_BRUSH));
				cpccOS::sleep(2000);
			}
			*/

			/*
			the typical erase background is:
			{ RECT tmpRect;
				
			GetClientRect(hwnd, &tmpRect); 
			FillRect((HDC) wParam, &tmpRect, (HBRUSH) GetStockObject(BLACK_BRUSH));
			}
			*/
			return 1L; //An application should return nonzero if it erases the background
		
		
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
	static cpccScreenSaverInterface_PerMonitor* ssNewPtr=NULL;

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
			//MessageBox(NULL,"ConfigureSheet that must have been overiden by the child class",NULL,NULL); 
			
			if (!ssNewPtr)
			{
				ssNewPtr = createScreenSaver();
			}

			if (ssNewPtr)
				if (ssNewPtr->hasConfigureSheet())
					ssNewPtr->showConfigureSheet();
				
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
