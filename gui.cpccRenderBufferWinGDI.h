/*  *****************************************
 *  File:		TmioanRenderBufferWinGDI.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include <Windows.h>


/* tutorial 

-------------
double buffer
-------------
http://www.codeproject.com/Articles/5865/Double-Buffered-DC-Class

Create a bitmap the size of the window, render into that bitmap, and 
blit that back into the window when you're done.
You can do a pretty straight-forward in-place replacement in your existing code. 
Instead of using a device context that renders into the window, use one that 
renders into the bitmap, and only use the original DC to blit the bitmap back.
Be sure to keep the bitmap around - don't create it in every paint call. 
You just need to recreate it when the window is resized.

http://msdn.microsoft.com/en-us/library/ms969905

-----------------------
http://www.powerbasic.com/support/pbforums/showthread.php?t=18940
Are you BitBlt()ing in WM_PAINT or is this a GetDC (draw whenever) operation?


-------------
bitmaps
-------------

http://www.cs.binghamton.edu/~reckert/360/3.html
-Data structure that stores a matrix of pixel values in memory
-GDI object that must be selected into a Device Context to be used
-Can be thought of as a canvas on a DC upon which drawing takes place
-Must be compatible with output device (video display or printer)
-Can be manipulated invisibly and apart from physical display device
-Can be transferred to/from physical a device-->flicker-free animation

We can say:      Bitmap           Window Client Area
                ----------   =   --------------------
                Memory DC                DC

Memory DC-- 
     Like a DC for a physical device, but not tied to the device
     Used to access a bitmap rather than a device
     Bitmap must be selected into a memory DC before displayable on a device DC 

hMemDC=CreateCompatibleDC(hDC)-- 
     Creates a memory DC with same physical attributes as DC of given device

SelectObject(hMemDC,hBitmap) ;
     Selects bitmap into the memory DC 

http://stackoverflow.com/questions/1748470/how-to-draw-image-on-a-window

-------------
GDI+
-------------
http://msdn.microsoft.com/en-us/library/windows/desktop/ms533895%28v=vs.85%29.aspx


*/


class renderBufferWinGDI
{
private:
	HBITMAP hBitmapMemory, hOldBitmap;
	HDC		hDCmemoryBuffer;
	
public:
	HDC dc(void) { return hDCmemoryBuffer; };


	renderBufferWinGDI(const HDC hDC, const int aWidth, const int aHeight)
	{	
		hDCmemoryBuffer = CreateCompatibleDC(hDC);
		SetBkMode(hDCmemoryBuffer, TRANSPARENT);
		
		hBitmapMemory = CreateCompatibleBitmap(hDC, aWidth, aHeight);
		// Select the bitmap into the off-screen DC.
		hOldBitmap = (HBITMAP) SelectObject(hDCmemoryBuffer, hBitmapMemory);
	}


	~renderBufferWinGDI()
	{
		if (hBitmapMemory)
			{
			SelectObject(hDCmemoryBuffer, hOldBitmap);
			DeleteObject(hBitmapMemory);
			DeleteDC(hDCmemoryBuffer);
			hDCmemoryBuffer=NULL;
			hBitmapMemory=NULL;
			}
	}

};

