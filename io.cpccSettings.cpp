/*  *****************************************
 *  File:		cpccSettings.cpp
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to save/load application settings from an INI-like file
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#include <cassert> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <locale>
#include <codecvt>

#include "core.cpccStringUtil.h"
#include "io.cpccSettings.h"

#include "fs.cpccSystemFolders.h"   // todo: remove the system (and user folders) The INI class does not have somemthing to do with them.
#include "fs.cpccUserFolders.h" 



/*
 ini file location tests:
 
 
 
 MAC OS X
    info: The User Domain values accessed by NSUserDefaults are serialized to a file ~/Library/Preferences/application.plist.
 
    user settings:
        non-sandboxed:
            /Users/cto/Library/Preferences/com.StarMessageSoftware.StarMessage.ini (ok)
            /Users/cto/Library/Preferences/com.StarMessageSoftware.StarMessage/com.StarMessageSoftware.StarMessage.ini (ok)
 
        sandboxed:
 
    system settings (app-wide, like common APPDATA):
        non-sandboxed:
            /users/shared/com.StarMessageSoftware.StarMessage.ini (ok)
            /Library/Preferences/com.StarMessageSoftware.StarMessage.ini (failed)
            /Library/Preferences/com.StarMessageSoftware.StarMessage/ (failed to create folder)
 
            If you need to create a directory in /Library/Application Support for your application to use then you need to do privilege elevation. Applications in the App Store cannot use privilege elevation
 
        sandboxed:
 
 */


 // /////////////////////////////////////////////////////////////////////////////////////////////////
 //
 //		class cSerialCodec declaration
 //
 // /////////////////////////////////////////////////////////////////////////////////////////////////


class cSerialCodec
{
private:

    typedef cpcc_char* tEncodingsTable[5][2];
    static const tEncodingsTable& encondingTable(void);

public:

    static void    encode(cpcc_string& str);
    static void    decode(cpcc_string& str);

};


 // /////////////////////////////////////////////////////////////////////////////////////////////////
 //
 //		class cSerialCodec implementation
 //
 // /////////////////////////////////////////////////////////////////////////////////////////////////

inline const cSerialCodec::tEncodingsTable& cSerialCodec::encondingTable(void)
{
    // static variable
    // encode characters to fit to the INI file (e.g. multiline text)
    static const tEncodingsTable encodedINIcharacterTable =
    {
        //        normal     ->       encoded
        { (cpcc_char*)_T("="),  (cpcc_char*)_T("\\=") },
        { (cpcc_char*)_T("\n"), (cpcc_char*)_T("\\n") },
        { (cpcc_char*)_T("\r"), (cpcc_char*)_T("\\r") },
        { (cpcc_char*)_T("\t"), (cpcc_char*)_T("\\t") },
        // this must be the last rule  for the decoding and first rule for the encoding
        { (cpcc_char*)_T("\\"), (cpcc_char*)_T("\\\\") }
    };
    return encodedINIcharacterTable;
}


inline void cSerialCodec::encode(cpcc_string& str)
{
    const tEncodingsTable& table = encondingTable();
    // traver backwards
    for (int i = (sizeof(table) / sizeof(table[0])) - 1; i >= 0; --i)
        stringUtils::findAndReplaceAll(str, table[i][0], table[i][1]);
}


inline void cSerialCodec::decode(cpcc_string& str)
{
    const tEncodingsTable& table = encondingTable();

    for (unsigned int i = 0; i < sizeof(table) / sizeof(table[0]); ++i)
        stringUtils::findAndReplaceAll(str, table[i][1], table[i][0]);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccSettings
///////////////////////////////////////////////////////////////////////////////////////////////////
cpccSettings::cpccSettings(const cpcc_char *aFilename)
{
    if (!aFilename)
        return;
    
    mFilename = aFilename;
    
    cpccPathString folder(cpccPathHelper::getParentFolderOf(aFilename));
    
    // make sure the folder for the INI file exists
    if (! folder.pathExists())
        if (!cpccFileSystemMini::createFolder(folder.c_str()))
            cpcc_cerr << _T("#5813: During cpccSettings constructor could not create folder:") << folder << std::endl;
    
    assert((folder.pathExists()) && _T("#7712i: folder for INI was not created"));

    if (!load())
        cpcc_cerr << _T("Error #1351: loading cpccSettings from file:") << mFilename << std::endl;
}


cpcc_string cpccSettings::getAutoFilename(const settingsScope aScope, const cpcc_char* aCompanyName, const cpcc_char* aAppName, const cpcc_char* aBundleID)
{
    cpccPathString fname(aScope== settingsScope::scopeAllUsers ? cpccSystemFolders::getCommonAppData() : cpccUserFolders::getUserData());
    assert(cpccFileSystemMini::folderExists(fname.c_str()) && _T("#5381: folder for saving the settings file does not exist"));
    
    #ifdef __APPLE__
        /*
         at this point, fname contains:
         - "/users/shared" for scopeAllUsers,
         - "~/Library/Preferences" for scopeCurrentUser
         
         For the scopeAllUsers, we need to create a subfolder with the bundleID
         */
    
        // fname("Preferences");
        if (aScope==settingsScope::scopeAllUsers)
            if (aBundleID)
                if (cpcc_strlen(aBundleID)>0)
                    fname.appendPathSegment(aBundleID);
    #else
        if (aCompanyName)
            if (cpcc_strlen(aCompanyName)>0)
                fname.appendPathSegment(aCompanyName);
    #endif
    // now the fname contains the containing folder for the INI file.
    
    // add the appName as part of the filename
    // operator && evaluates left operand first
    if (aAppName && (cpcc_strlen(aAppName)>0))
        fname.appendPathSegment(aAppName);
    else
        fname.appendPathSegment(_T("no AppName cpccSettings error no.7511"));
    
    fname.append(_T(".ini"));
    return std::move(fname);
}

/*
cpccSettings::cpccSettings(const settingsScope aScope):
	instantSaving(true)
{
	// std::cout << "cpccSettings constructor\n";
	
	assert(cpcc_strlen(config_getCompanyName())>0 && _T("#5351: cpccSettings: blank company name"));
	assert(cpcc_strlen(config_getAppName())>0 && _T("#5351: cpccSettings: blank Software name"));

	cpccPathString _settingsFilename(aScope==scopeAllUsers ? cpccSystemFolders::getFolder_CommonAppData() : cpccSystemFolders::getFolder_UserData());
	assert(cpccFileSystemMini::folderExists(_settingsFilename.c_str()) && _T("#5381: folder for saving the settings file does not exist"));
#ifdef __APPLE__
	// _settingsFilename.appendPathSegment("Preferences");
    if (aScope==scopeAllUsers)
        _settingsFilename.appendPathSegment(config_getAppBundleID());
    
#else
    _settingsFilename.appendPathSegment(config_getCompanyName());
#endif

    // make sure the folder for the INI file exists
    if (! _settingsFilename.pathExists())
        if (!cpccFileSystemMini::createFolder(_settingsFilename.c_str()))
            cpcc_cerr << _T("During cpccSettings constructor could not create folder:") << _settingsFilename << _T("\n");
    
    assert((_settingsFilename.pathExists()) && _T("#7712i: folder for INI was not created"));
    
    _settingsFilename.appendPathSegment(config_getAppName());
	_settingsFilename.append(_T(".ini"));
    
	mFilename = _settingsFilename;
	
	if (!load())
		cpcc_cerr << _T("Error #1351: loading cpccSettings from file:") << mFilename << std::endl;
	
	//std::cout << "cpccSettings construction point 2\n";
}
*/

	
cpccSettings::~cpccSettings()
{
	if (m_needsSaving)
		if (!save())
			cpcc_cerr << _T("Error #1352: saving cpccSettings to file:") << mFilename << std::endl;
}


bool cpccSettings::load(void)
{
    if (!cpccFileSystemMini::fileExists(mFilename.c_str()))
    {
        clear();
        return true; // consider the INI loaded (empty file)
    }
        
    cpcc_ifstream iniFile(mFilename);
    if (!iniFile.is_open())
    {
        cpcc_cerr << _T("#8551: Could not open file:") << mFilename << _T("\n");
        return false;
    }

    // todo: is this needed for MacOS ?
    std::locale my_utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
    iniFile.imbue(my_utf8_locale);

    // directly manipulate the map so that the save-to-file is not triggered
    clear();
    cpcc_string key, value;

    // http://forums.codeguru.com/showthread.php?511066-RESOLVED-Is-it-possible-to-use-getline-with-unicode
    while (getline(iniFile, key, _T('=')))
    {
        if (key.compare(0, 3, _T(UTF8_BOM)) == 0)  // the file has a UTF-8 BOM
            key.erase(0, 3);                  // Now get rid of the BOM.

        getline(iniFile, value);
        cSerialCodec::decode(value);
        // set(key.c_str(), value);
        // directly add into the map so that the save to file is not triggered
        m_map[key] = value;
    }

    return true;

}


#ifdef _WIN32
std::string helper_to_utf8(const wchar_t* buffer, int len)
{
    std::string result;
    int nChars = ::WideCharToMultiByte( CP_UTF8, 0, buffer, len, NULL, 0, NULL, NULL);
    if (nChars > 0)
    {
        result.resize(nChars);
        // ::WideCharToMultiByte( CP_UTF8, 0, buffer, len, const_cast<char*>(result.c_str()), nChars, NULL, NULL);
        ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, &result[0], nChars, NULL, NULL);
    }
    return result;
}
#endif


bool cpccSettings::save(void)
{
    cpcc_ostringstream ss;

    // for (cpccKeyValue::tKeysAndValues::const_iterator it = m_map.begin(); it != m_map.end(); ++it)
    for (auto element : getMap())
    {
        const cpcc_char* key = element.first.c_str();
        cpcc_string    value = element.second; // todo: value( it->second);
        
        cSerialCodec::encode(value);
        ss << key << _T("=") << value << std::endl;
    }

#ifdef DEBUG
    cpcc_string test = serialize(_T("\n"), true, cSerialCodec::encode);
    assert (test.compare(ss.str())==0 && _T("#9582: error in save() serialize*()"));
#endif
    
#ifdef UNICODE	// write the BOM if UTF-8
    //const cpcc_string tmpTxt(ss.str());
    //const cpcc_string tmpTxtAfterUTF8(helper_to_utf8(tmpTxt.c_str(), tmpTxt.length()));
    //return saveStringToFile(mFilename.c_str(), tmpTxtAfterUTF8.c_str(), true);
    return cpccFileSystemMini::writeTextFile(mFilename.c_str(), ss.str().c_str(), true);
#else
    return cpccFileSystemMini::writeTextFile(mFilename.c_str(), ss.str().c_str(), false);
#endif
    
}



void cpccSettings::resumeInstantSaving(void)
{
	instantSaving = true;
	if (!save())
		cpcc_cerr << _T("Error #1353: saving cpccSettings to file:") << mFilename << std::endl;
}

