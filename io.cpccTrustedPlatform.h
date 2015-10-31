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


	cpcc_string& 	getStorageFolder(const cpcc_char *userOverrideFolder = NULL)
	{
		static cpcc_string _folder("");
		if (userOverrideFolder)
			_folder = userOverrideFolder;

		if (_folder.length() == 0)
		{
			cpccFileSystemMiniEx fs;
			_folder = fs.getFolder_CommonAppData();
			_folder.append(".secstore");
			_folder.append(m_AppID);

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

	const long int		getDaysSinceFirstRun(void)  { return 0; }
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


