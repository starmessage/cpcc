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

#include <sstream>
#ifdef __APPLE__
	#include <ext/hash_map>
	#include <tr1/functional>
	typedef std::tr1::hash<cpcc_string> portableHashString;
#elif defined(_WIN32)
	typedef std::hash<cpcc_string>      portableHashString;
#endif


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
		static cpccPathString _folder((cpcc_char *)"");
		if (userOverrideFolder)
			_folder.assign(userOverrideFolder);

		if (_folder.length() == 0)
		{
			cpccFileSystemMiniEx fs;
			_folder.assign(fs.getFolder_CommonAppData());
            cpcc_string codedSubfolder;
            computeHash(m_AppID, codedSubfolder);
            codedSubfolder.insert(0, ".");
			_folder.appendPathSegment(codedSubfolder.c_str());

			if (!fs.folderExists(_folder))
				fs.createFolder(_folder);
		}

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
        cpccFileSystemMiniEx    fs;
        int    days;
        time_t current_time, firstrun_time;
        /* Obtain current time as seconds elapsed since the Epoch. */
        current_time = time(NULL);
        firstrun_time=fs.getFileModificationDate(getStorageFolder().c_str());
        days = (current_time - firstrun_time)/ (60*60*24); // convert from seconds to days
        
        return days;
    }
    
	const time_t		getTrustedCurrentDate(void)  { return 0; }

	inline void			saveText(const int aTextID, cpcc_char *aText) { }

	/// returns false if the aTextID is not found or text loading fails.
    inline bool			loadText(const int aTextID, cpcc_string & aText) { return false; }

	inline void			removeSavedText(const int aTextID) { }

	inline const bool	hackDetected(void) { return m_hackDetected; }
	
	const std::string	serialize(void) const
                        {	return cpcc_string("");
                        }
    
	void				deserialize(std::string &s) { };
	

	static 	void computeHash(const cpcc_string &txtOriginal, cpcc_string &txtHashed) 
	{	// 	http://www.cplusplus.com/reference/functional/hash/
		portableHashString str_hash;
		size_t result = str_hash(txtOriginal);

		std::stringstream ss;
		ss << result;

		txtHashed = ss.str();
	}


	static void			selfTest(void);
	

};


