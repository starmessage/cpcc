/*  *****************************************
 *  File:		io.cpccSound.h
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

#pragma once


#ifdef _WIN32
	//typedef		int		cpccSoundIdType;

#elif defined(__APPLE__)
	//typedef		char *	cpccSoundIdType;
    #import <AppKit/AppKit.h>

#endif


class cpccSound
{
private:	// data
    #if defined(__APPLE__)
        NSSound *nsSoundPtr = NULL;
    #endif

public:	// ctors
    cpccSound()
    {
    #if defined(__APPLE__)
        if (nsSoundPtr)
        {   // [nsSoundPtr release];
            nsSoundPtr = NULL;
        }
    #endif
    }

	~cpccSound(void)	{ stop(); }
	
public:
    
	#ifdef _WIN32
        void 	playResourceSound(const int aSoundResource, const bool loop);
    
    #elif defined(__APPLE__)
        void 	playResourceSound(NSString *aBundleClass, NSString *aResourceFile, const bool loop);
        void    playSoundFile(NSString *aFilename, const bool loop);

    #endif
    
    void    playSoundFile(const char *aFilename, const bool loop);
	void	stop(void);

};






