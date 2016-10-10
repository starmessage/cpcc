/*  *****************************************
 *  File:		io.cpccTrustedPlatform.h
 *	Purpose:	Portable (cross-platform), light-weight, screensaver helper class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *				A class for saving and loading encrypted and trusted information.
 *				Used by the shareware license registration system.
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
#pragma once

#include <vector>
#include <string>
#include "cpccUnicodeSupport.h"
#include "core.cpccOS.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccLog.h"
#include <sstream>

/* 
 to use hash in XCODE:
 What you need to do is open the project settings -> Build Settings and set
 C++ Language Dialect to C++11 and
 the C++ Standard Library to libc++ (LLVM C++ standard library with C++11 support)
 */
typedef std::hash<cpcc_string>      portableHashString;
/*
#ifdef __APPLE__
	#include <ext/hash_map>
	#include <tr1/functional>
	typedef std::tr1::hash<cpcc_string> portableHashString;
#elif defined(_WIN32)
	typedef std::hash<cpcc_string>      portableHashString;
#endif
*/

class cpccTrustedPlatform
{
protected:
	cpcc_string 	m_AppID;
	bool			m_hackDetected;
	void			encryptStr(cpcc_string & aString);
	void			decryptStr(cpcc_string & aString);
	void			saveCurrentDateAsFirstRun(void) 
	{ 
		
	}


	const cpcc_string& 	getStorageFolder(const cpcc_char *userOverrideFolder = NULL)
	{
		static cpccPathString _folder((cpcc_char *)_T(""));
        
		if (userOverrideFolder)
			_folder.assign(userOverrideFolder);

		if (_folder.length() == 0)
		{
		#ifdef OSX_SANDBOXED
			_folder.assign(cpccFileSystemMini::getFolder_UserData());
		#else
			_folder.assign(cpccFileSystemMini::getFolder_CommonAppData());
		#endif

            cpcc_string codedSubfolder;
            computeHash(m_AppID, codedSubfolder);
            codedSubfolder.insert(0, _T("."));
			_folder.appendPathSegment(codedSubfolder.c_str());
		}
        if (!cpccFileSystemMini::folderExists(_folder))
            cpccFileSystemMini::createFolder(_folder);
        
		return _folder;
	}

public:

	// aStorageFolder==NULL means to use a default location for windows and MAC
	cpccTrustedPlatform(cpcc_char *anAppID, cpcc_char *aStorageFolder=NULL) : 
		m_AppID(anAppID),  m_hackDetected(false) 
	{ 
		getStorageFolder(aStorageFolder);
	}

	const int			getVersionOfSaveFormat(void) { return 1; }

	void				getComputerID(cpcc_string & aComputerID)  { computeHash(cpccOS::getComputerName(), aComputerID); }

	const long int		getDaysSinceFirstRun(void)
    {
        
        time_t current_time, firstrun_time;
        /* Obtain current time as seconds elapsed since the Epoch. */
        current_time = time(NULL);
		cpcc_string _folder = getStorageFolder();

        firstrun_time= cpccFileSystemMini::getFileModificationDate(_folder.c_str());
		
        debugLog().addf(_T("Now (days) %lu, First run time (days) %lu"),
						(unsigned long) current_time/ (60*60*24),
						(unsigned long) firstrun_time / (60*60*24));
		debugLog().addf("By checking folder:%s", _folder.c_str());
		int    days = (int) ((current_time - firstrun_time)/ (60*60*24)); // convert from seconds to days
		// Or you could use difftime() which returns a double as difference between two time_t values so 
		// that you don't have to worry about the underlying type of time_t.
        return days;
    }
    
	const time_t		getTrustedCurrentDate(void)  { return 0; }

	inline void			saveText(const int, cpcc_char *aText) { }

	/// returns false if the aTextID is not found or text loading fails.
    inline bool			loadText(const int aTextID, cpcc_string & aText) { return false; }

	inline void			removeSavedText(const int aTextID) { }

	inline const bool	hackDetected(void) { return m_hackDetected; }
	
	const cpcc_string	serialize(void) const
                        {	return cpcc_string(_T("") );
                        }
    
	void				deserialize(std::string &s) { };
	

	static 	void computeHash(const cpcc_string &txtOriginal, cpcc_string &txtHashed) 
	{	// 	http://www.cplusplus.com/reference/functional/hash/
		portableHashString str_hash;
		size_t result = str_hash(txtOriginal);

		cpcc_stringstream ss;
		ss << result;

		txtHashed = ss.str();
	}


	static void			selfTest(void);
	

};


