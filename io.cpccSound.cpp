
/*  *****************************************
*  File:		io.cpccSound.cpp
*	Purpose:	Portable (cross-platform), light-weight, sound class
*	*****************************************
*  Library:	Cross Platform C++ Classes (cpcc)
*  Copyright: 	2016 StarMessage software.
*  License: 	Free for opensource projects.
*  			Commercial license for closed source projects.
*	Web:		http://www.StarMessageSoftware.com/cpcclibrary
*	email:		sales -at- starmessage.info
*	*****************************************
*/


// do not compile this if added in the xcode project files.
// the .mm must be included as well
#if defined(_WIN32) || defined (IMPORTED_BY_io_cpccSound_mm)


#ifdef __APPLE__
    #if !defined __OBJC__
        #error File io.cpccSound.cpp must be compiled as obc++ under MAC. Did you include the .cpp instead of the wrapper .mm ?
    #endif
#endif


#include "io.cpccSound.h"
#include "io.cpccLog.h"
#include "core.cpccTryAndCatch.h"
#include "io.cpccFileSystemMini.h"

#ifdef _WIN32
	#include	<windows.h> // for _access on windows
	#include 	<mmsystem.h>
	#pragma comment(lib, "winmm.lib")
#elif defined(__APPLE__)
    #import <AppKit/AppKit.h>
#endif

/*
 The following shows how to load a sound file from disk using soundNamed:.
 nsSoundPtr = [[NSSound soundNamed:@"aSoundFilename.wav"] retain];
 
 If there is no known sound with the name you’ve specified, soundNamed: searches your application’s main bundle, and then the /Library/Sounds and ~/Library/Sounds directories for sound files with the specified name. If no data can be found for name, no object is created and nil is returned.
 */


/*
bool 	cpccSound::loadFromAppResources(const cpccSoundIdType aSoundResource)
{	
	m_soundId = aSoundResource;	
	return m_loaded = true; 
}
*/


void	cpccSound::stop(void)
{
#ifdef _WIN32
	/*
	If you try stop play sounds asynchronously, but none sound is playing. This call will never return:
	PlaySound(NULL, 0, 0); //may produce deadlock
	For avoid that problem use:
	PlaySound(NULL, 0, SND_ASYNC); //all fine
	*/
	PlaySound(NULL, NULL, SND_ASYNC);

#elif defined(__APPLE__)
    if (nsSoundPtr)
    {
        [nsSoundPtr stop];
         #if !(__has_feature(objc_arc))
            [nsSoundPtr release];
            nsSoundPtr=NULL;
        #endif
    }
#endif

}


#ifdef _WIN32

void cpccSound::playResourceSound(const int aSoundResource, const bool loop)
{
	stop();
    int loopParam = (loop) ? SND_LOOP : 0;;
    // HINSTANCE hInst = (HINSTANCE) GetWindowLong(NULL, GWL_HINSTANCE );
    HINSTANCE hInst = GetModuleHandle(NULL);
    PlaySound(MAKEINTRESOURCE(aSoundResource), hInst, SND_RESOURCE | SND_ASYNC | loopParam);
}

#elif defined(__APPLE__)

void 	cpccSound::playResourceSound(NSString *aBundleClass, NSString *aResourceFile, const bool loop)
{
    logFunctionLife tmpLog("playResourceSound()");

    NSBundle *pBundle = [NSBundle bundleForClass:aBundleClass];
    NSString *resourcePath = [pBundle pathForResource: aResourceFile ofType:@"wav"];
     #if !(__has_feature(objc_arc))
        [pBundle release];
    #endif
    //infoLog().addf("path of sound:%s", [resourcePath UTF8String]);
    playSoundFile(resourcePath, loop);
}

#endif


#ifdef __APPLE__
void cpccSound::playSoundFile(NSString *aFilename, const bool loop)
{
    if (nsSoundPtr)
    {
        [nsSoundPtr release];
        nsSoundPtr = NULL;
    }
    
    CPCC_TRY_AND_CATCH(nsSoundPtr = [[NSSound alloc] initWithContentsOfFile:aFilename byReference:YES], "Loading sound");

    if (!nsSoundPtr)
    {   errorLog().addf("Failed to load sound:%s", [aFilename UTF8String]);
        return;
    }

    // No documentation by Apple on how to play a sound in a loop
    // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSSound_Class/
    // Someone found out that to play back more than once you can use
    // [nsSoundPtr setCurrentTime:0];
    
    [nsSoundPtr setLoops: loop?YES:NO];
    if (![nsSoundPtr isPlaying])
        [nsSoundPtr play];
    else
        warningLog().add("playResourceSound() : a sound was playing already");
}
#endif


void cpccSound::playSoundFile(const cpcc_char *aFilename, const bool loop)
{
#ifdef _WIN32
    //ToDo:
    
#elif defined(__APPLE__)
    if (!cpccFileSystemMini::fileExists(aFilename))
    {
        warningLog().addf("playSoundFile() cannot find file:%s", aFilename);
        return;
    }
    NSString *filenameStr = [NSString stringWithUTF8String: aFilename];
    playSoundFile(filenameStr, loop);
    // [filenameStr release]; // crashes: release sent to an already released object
#endif
    
}


#endif
