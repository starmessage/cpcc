/*  *****************************************
 *  File:		core.cpccLocalisation.h
 *	Purpose:	Portable (cross-platform), light-weight, localisation (translate strings to different languages) helper class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
#pragma once

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "cpccUnicodeSupport.h"
#include "io.cpccLog.h"
#include "io.cpccFileSystemMini.h"
#include "io.cpccSettings.h"

//////////////////////////////////////////////
//
//  Enumerate controls
//
//  cocoa:
//      is there any method to get the list of controls (buttons, labels, splitters, etc) of a NSView?
//      [NSView subViews]
//      You can get the subviews of the main view and keep going down finding all views that way recursively
//      calling subviews on each, but it won't really help you as some of the controls make a collection of
//      views up when you instantiate them so you are going to find more than just the top-level views of the
//      controls you instantiated in IB.
//
//      I can explore all controls in a view with this code:
//      NSArray *listOfControls = [[window contentview] subViews];
//      for (NSInteger i=0;i<[listOfControls count];i++) 
//			NSControl * tmp = [listOfControls objectAtIndex: i];
//          
//      I can identify a control by its title
//
//	Win32:
//		https://vcpptips.wordpress.com/2009/10/16/how-to-get-all-the-child-controls-of-a-window/
//		The EnumChildWindows function enumerates all the child windows/controls belongs to a specified window. 
//		This function invokes a application defined CALLBACK function until the last child control is enumerated 
//		or the function returns false.
//		
//		EnumChildWindows(this->m_hWnd,EnumWindowsProc, 0);
//
//		BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
//		{
//			TCHAR buff[256];
//			::GetWindowText(hwnd,(LPSTR)buff, 255);
//			int nCtrlID = ::GetDlgCtrlID(hwnd);
//		   return TRUE;
//		}
//	
//////////////////////////////////////////////



//////////////////////////////////////////////
//
//  class cpccTranslationDictionary
//
//////////////////////////////////////////////
class cpccTranslationDictionary
{
private:
	std::map<cpcc_string, cpcc_string> m_lookupTable;		// codedText, translatedText

	void setTranslation(const cpcc_char *aKey, const cpcc_char *aValue) { m_lookupTable[aKey] = aValue; }

public:
	void clear(void) { m_lookupTable.clear(); }
	
	const cpcc_char *getTranslation(const cpcc_char *aKey) 
	{ 
		if (!aKey)
			return "null key to translate";

		if (m_lookupTable.size() == 0)
			setTranslation(aKey, aKey); // add a  phantom translation so it can return a static reference to the table

		if (m_lookupTable.find(aKey) == m_lookupTable.end()) // not found
			setTranslation(aKey, aKey); // add a  phantom translation so it can return a static reference to the table

		return m_lookupTable[aKey].c_str();
	}
		
	/// aFileStem should be like: <appname>
	/// resulting filenames should be like: <aContainingFolder>/<appname>.en.txt, <aContainingFolder>/<appname>.de.txt, etc.
	bool	loadFromFile(const cpcc_string &aFilename)
	{	
		clear();
		infoLog().addf(_T("loading translations from file:%s"), aFilename.c_str());
        
        cpccSettings translationFile(aFilename.c_str());
        m_lookupTable = translationFile.getMap();

        /*
		cpcc_string line;
		cpcc_ifstream translationFile(aFilename.c_str() );
		if (!translationFile.good())
		{
			warningLog().addf(_T("translation file (%s) could not be opened."), aFilename.c_str());
			return false;
		}
	
		cpcc_string key, value;
		while(getline(translationFile, key, _T('=')))
		{
			getline(translationFile, value);
			stringConversions::decodeStrFromINI(value);
			m_lookupTable[key] = value;;
		}
        */

		infoLog().addf(_T("%i translations loaded"), m_lookupTable.size());
		return true;
	}
};


//////////////////////////////////////////////
//
//  class cpccLocalisation
//
//////////////////////////////////////////////

typedef std::map<cpcc_string, cpcc_string> cLanguageList;	// languageCode, languageNameInEnglish

class cpccLocalisation
{
private:
	bool					m_loadedFromFile = false;
	cpccTranslationDictionary	m_dictionary;
	cpcc_string				m_selectedLanguageCode,
							m_folderWithTranslations,
							m_translationFileStem;
	cLanguageList			m_languagesTable;	
	

	/// aFileStem should be like: <appname>
	/// resulting filenames should be like: 
	///		<aContainingFolder>/<appname>.en.txt, 
	///		<aContainingFolder>/<appname>.de.txt, etc.
	cpcc_string				composeFilename(const cpcc_string &aLangCode, const cpcc_string &aContainingFolder, const cpcc_string &aFileStem)
	{
		cpcc_string filename = aContainingFolder;

		// add the path delimiter if needed
		if (!filename.empty())
		{
			char lastChar = *filename.rbegin();
			if ((lastChar != _T('/')) && (lastChar != _T('\\')))
			{
#ifdef _WIN32
				filename += _T('\\');
#elif defined(__APPLE__)
				filename += _T('/');
#endif
			}
		}
		filename += aFileStem;
		filename += _T(".");
		filename += aLangCode;
		filename += _T(".txt");
		return filename;
	}
    
public:		// ctors,  factory

	static cpccLocalisation     &getInstance(void)
	{
		static cpccLocalisation _localisation;
		return _localisation;
	}
	
	cpccLocalisation()
	{
		// std::map inserts them in ordered way
		m_languagesTable.insert(std::pair<std::string, std::string>("DE", "German"));
		m_languagesTable.insert(std::pair<std::string, std::string>("EL", "Greek"));
		m_languagesTable.insert(std::pair<std::string, std::string>("EN", "English"));
		m_languagesTable.insert(std::pair<std::string, std::string>("ES", "Spanish"));
		m_languagesTable.insert(std::pair<std::string, std::string>("FR", "French"));
		m_languagesTable.insert(std::pair<std::string, std::string>("IT", "Italian"));
		
		//..... add more language codes
	}
	
    virtual ~cpccLocalisation() {}
    
public:

	const cpcc_char *getTranslation(const cpcc_char *aCodedTxt)
	{
		if (!m_loadedFromFile)
		{
			m_loadedFromFile = true;
			cpcc_string filename(composeFilename(m_selectedLanguageCode, m_folderWithTranslations, m_translationFileStem));
			m_dictionary.loadFromFile(filename);
		}

		return m_dictionary.getTranslation(aCodedTxt);
	}
	

	void setLanguageAndFilespec(const char *aLangCode, const char *aContainingFolder, const char *aFileStem)
	{
		
		m_loadedFromFile = false;	// force a reload
		m_selectedLanguageCode = aLangCode;
		m_folderWithTranslations = aContainingFolder;
		m_translationFileStem = aFileStem;
		infoLog().addf("setLanguageAndFilespec(%s, %s/%s)", aLangCode, aContainingFolder, aFileStem);
	}

	// Scans the folder where the language files should be and returns a list of found translation languages
	// Must be called after setLanguageAndFilespec()
	// Must be called only once. A second call will only scan for the languages found in the first scan
	cLanguageList &getInstalledLanguages(void)
	{
		if ((m_folderWithTranslations.length() == 0) || (m_translationFileStem.length() == 0))
			warningLog().add("getInstalledLanguages() called but m_folderWithTranslations or m_translationFileStem is empty");

		cpcc_string filename, languagesFound;
		// reverse iterate std::map and erase the elements that do not match. 
		// STL does not provide a simple solution to this simple problem.
		// std::map.erase() cannot take a reverse_iterator
		// So you need to do all the following. Thanks STL :(
		cLanguageList::reverse_iterator rIter = m_languagesTable.rbegin();
		while (rIter != m_languagesTable.rend())
		{
			filename = composeFilename(rIter->first, m_folderWithTranslations, m_translationFileStem);

			if (!cpccFileSystemMini::fileExists(filename.c_str()))
			{
				++rIter;
				rIter = cLanguageList::reverse_iterator(m_languagesTable.erase(rIter.base())); 
			}
			else
			{ 
				languagesFound += " " + rIter->first;
				++rIter;
			}
		}
		
		infoLog().addf("%i installed languages:%s", m_languagesTable.size() , languagesFound.c_str());
		return m_languagesTable;
	}

};



// convenience define
#define trTxt(x)	cpccLocalisation::getInstance().getTranslation(x)
