/*  *****************************************
 *  File:		time.cpccTimer.h
 *	Purpose:	Portable (cross-platform), simple timer function
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

/*
	NSTimer tutorial
	I have a little tutorial on my page
	http://cocoa-coding.de/timer/nstimer.html

	Use C++ with Cocoa Instead of Objective-C
	http://stackoverflow.com/questions/525609/use-c-with-cocoa-instead-of-objective-c

	Have a look at NSTimer (Cocoa) or CFRunLoopTimer (Core Foundation). \
	You could even use both if needed - they're toll-free bridged, so instances 
	of either one can be used interchangeably, with a simple typecast.
	
	http://stackoverflow.com/questions/13009705/adding-nstimer-to-a-c-class
	use the CoreFoundation equivalent; CFRunLoopTimer.
	Using an NSTimer instead would often mean you need a binding objc object type to use 
	as the parameter for your timer callbacks.
	
	
	UNIX
	---------
	http://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html#Setting-an-Alarm
	The alarm and setitimer functions provide a mechanism for a process to interrupt itself in the future. 
	They do this by setting a timer; when the timer expires, the process receives a signal. 
	
	
	WINDOWS:
	--------------------
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms644906%28v=vs.85%29.aspx
	setTimer() / killTimer() needs a window handle that is owned by the application (not HWND_DESKTOP)

	If hwnd=NULL, there is no window associated with the timer so the WM_TIMER message will not be used. 
	The system will directly call your timer function (TIMERPROC) and the HWND parameter will be NULL. 
	
	To handle a timer message when a window isn't specified:
	That is done by creating a global (not associated with a window) function like this:
	VOID CALLBACK MyFunc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
	}

	Can I use a SetTimer() API in a console C++ application?	
	http://stackoverflow.com/questions/7531650/can-i-use-a-settimer-api-in-a-console-c-application
	You can use CreateTimerQueueTimer function
	HANDLE timer_handle_;
	CreateTimerQueueTimer(&timer_handle_, NULL, TimerProc, user_object_ptr, 10, 0, WT_EXECUTEDEFAULT);
	//callback
	void TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
	{
		user_object* mgr = (user_object*) lpParameter;
		mgr->do();
		DeleteTimerQueueTimer(NULL, timer_handle_, NULL);
		timer_handle_ = NULL;
	}
	
	CreateTimerQueueTimer function:
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms682485%28v=vs.85%29.aspx
	example:
	http://www.adp-gmbh.ch/win/misc/timer.html
	
	TimerProc callback function
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms644907%28v=vs.85%29.aspx
	VOID CALLBACK TimerProc( HWND     hwnd,  UINT     uMsg,  UINT_PTR idEvent,  DWORD    dwTime );
	
	Waitable Timer Objects
	 CreateWaitableTimer, CreateWaitableTimerEx,  SetWaitableTimer,  CancelWaitableTimer,  CloseHandle 
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms687012%28v=vs.85%29.aspx
	A waitable timer object is a synchronization object whose state is set to signaled 
	when the specified due time arrives. 
	There are two types of waitable timers that can be created: 
	manual-reset and synchronization. 
	A timer of either type can also be a periodic timer.
	
	Timer Queues
	Known as timer-queue timers, are lightweight objects that enable you to specify a callback function 
	to be called when the specified due time arrives. 
	To add a timer to the queue, call the CreateTimerQueueTimer function. 
	To update a timer-queue timer, call the ChangeTimerQueueTimer function. 
	You can specify a callback function to be executed by a worker thread from the thread pool when the timer expires.
	To cancel a pending timer, call the DeleteTimerQueueTimer function. 
	When you are finished with the queue of timers, call the DeleteTimerQueueEx function to delete the timer queue. 
	https://msdn.microsoft.com/en-us/library/ms687003%28v=vs.85%29.aspx
	
	boost::asio
	--------------
	example: http://www.boost.org/doc/libs/1_60_0/doc/html/boost_asio/tutorial/tuttimer2.html
	
	
	Examples:
	---------------
	timed callbacks
	http://www.drdobbs.com/cpp/timed-callbacks-in-c/184408856
	
	Simple C++ Timer Wrapper
	http://www.codeproject.com/Articles/146617/Simple-C-Timer-Wrapper
	
	http://pocoproject.org/docs/Poco.Timer.html
	TimerCallback<MyClass> callback(*this, &MyClass::onTimer);
	timer.start(callback);
*/


///////////////////////////////////////////////////////////////////
//		class cpccTimerWin
///////////////////////////////////////////////////////////////////
class cpccTimerWin
{
private:
	UINT m_timerHandle = NULL; // SetTimer's return value 
	
public:

	virtual void startTimer(const time_t msecIterval)
	{
		m_timerHandle = SetTimer(NULL , /* nIDEvent= */ 0, /* uElapse= */ msecInterval, /* lpTimerFunc= */ (TIMERPROC) NULL);

	}

	virtual void stopTimer(void)
	{
	
	}
	
	
};


///////////////////////////////////////////////////////////////////
//		class cpccTimer
///////////////////////////////////////////////////////////////////
class cpccTimer
{
public:
	virtual void Timer::timerCallback(void)
	{
	
	}
	
};

