/*  *****************************************
 *  File:		core.cpccKeyValue.h
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <map>
#include <assert.h>
#include <sstream>
#include "core.cpccStringUtil.h"
#include "cpccUnicodeSupport.h"


/** A small and portable (cross platform) C++ class 
	providing a std:map that can store any value type (primitive data only)
		
	Dependencies: cpcc_SelfTest.h
*/




///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccKeyValue
///////////////////////////////////////////////////////////////////////////////////////////////////

class cpccKeyValue
{
private:
	typedef std::map<cpcc_string, cpcc_string> tKeysAndValues;
	tKeysAndValues	m_map;


private: 	// data

public: 	// ctors	

	
public:		// functions
	/*
	template <typename T>
	const T get(const cpcc_char *aKey) 				{ return stringConversions::fromStr(read(aKey, "").c_str(); }
	const cpcc_string get(const cpcc_char *aKey)	{ return mSettings[aKey] : cpcc_string(aDefault); }
	const cpcc_string get(const cpcc_char *aKey)	{ return read(aKey, aDefault.c_str()); }
    */
	template <typename T>
	const bool	get(const cpcc_char *aKey, T& aValue)	
		{
			if (!keyExists(aKey)) return false;
			return stringConversions::fromStrNew(m_map[aKey].c_str(), aValue);
		}

	template <typename T>
	void		set(const cpcc_char *aKey, const T aValue)				{ m_map[aKey] = stringConversions::toStr(aValue); }
	void        set(const cpcc_char *aKey, const cpcc_string &aValue)	{ m_map[aKey] = aValue; }
    void		set(const cpcc_char *aKey, const cpcc_char *aValue)		{ m_map[aKey] = aValue; }
	void		clear(void)												{ m_map.clear(); }
	const bool	keyExists(const cpcc_char *aKey) const					{ return (m_map.find(aKey) != m_map.end()); }

public:	// class metadata and selftest

	static void selfTest(void)
	{
    #ifndef NDEBUG
		cpccKeyValue testSubject;
		testSubject.set(_T("value123"), 123);
		testSubject.set(_T("keyMike"), _T("Mike Value"));
		testSubject.set(_T("value1.23456789"), 1.23456789f);

		assert((testSubject.keyExists(_T("eeerrrooor")) == false) && _T("SelfTest #8622: non-existing key found"));


		int tmp_i;
		assert((testSubject.get(_T("value123"), tmp_i) == true) && _T("SelfTest #8622a: key not found"));
		assert((tmp_i == 123) && _T("SelfTest #8622b: integer write/read failed"));

		float tmp_f;
		assert((testSubject.get(_T("value1.23456789"), tmp_f) == true) && _T("SelfTest #8622c: key not found"));
		assert((tmp_f == 1.23456789f) && _T("SelfTest #8622d: float write/read failed"));

		cpcc_string tmp_s;
		assert((testSubject.get(_T("keyMike"), tmp_s) == true) && _T("SelfTest #8622e: key not found"));
		// assert((tmp_s.compare("Mike Value") == 0) && "SelfTest #8622f: string write/read failed");
		assert((tmp_s == _T("Mike Value")) && _T("SelfTest #8622f: string write/read failed"));
    #endif
	}
};

