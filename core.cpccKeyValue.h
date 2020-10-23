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
#include <sstream>
#include <algorithm>
#include "data.cpccKeyValueStr.h"
#include "core.cpccStringUtil.h"
#include "cpccUnicodeSupport.h"

#include "cpccTesting.h"

/** 
    A small and portable (cross platform) C++ class 
	Provides a std:map that can store any (primitive type) value
		
*/



// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccKeyValue declaration
//
// /////////////////////////////////////////////////////////////////////////////////////////////////

class cpccKeyValue: private strConvertionsV3, public cpccKeyValueStr
{

public:		// get functions

    const cpcc_string	 get(const cpcc_char* aKey, const cpcc_char* aDefaultValue) const;
    const cpcc_string get(const cpcc_char *aKey, const cpcc_string &aDefaultValue)  const  {   return get(aKey, aDefaultValue.c_str());    }

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type >
    T    get(const cpcc_char *aKey, const T aDefaultValue) const
    {
        cpcc_string valueStr(get(aKey, _T("not-fOunD-there")));
        if (valueStr.compare(_T("not-fOunD-there")) == 0)
            return aDefaultValue;
        
        // if (!std::is_same<T, cpcc_char *>::value)
        return fromString(valueStr.c_str(), aDefaultValue);
    }
    
public:		// set functions

	// does the final set() of the pair, implementing also write caching and signaling to descendant classes
    // inline void set_impl(const cpcc_char* aKey, const cpcc_char* aValue);
	
	template <typename T>
	void		set(const cpcc_char   *aKey, const T aValue)              	{ cpccKeyValueStr::set(aKey, toString(aValue).c_str()) ;  }
	void     set(const cpcc_char   *aKey, const cpcc_string &aValue)		{ cpccKeyValueStr::set(aKey, aValue.c_str());  }
    // void        set(const cpcc_string &aKey, const cpcc_string &aDefaultValue) { m_map[aKey] = aDefaultValue; }
    // void		set(const cpcc_char   *aKey, const cpcc_char *aValue)		{ set(aKey, aValue);  }

public: // load from string functions

    // aKeyValueList is a key=value string. Pairs are separated by new lines
    // returns the number of pairs processed.
    const int   addFromSimpleString(const cpcc_char* aKeyValueList);

    // clears the list and adds a new
    const int   loadFromSimpleString(const cpcc_char* aKeyValueList);

};

// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccKeyValue implementation
//
// /////////////////////////////////////////////////////////////////////////////////////////////////



inline const cpcc_string	cpccKeyValue::get(const cpcc_char* aKey, const cpcc_char* aDefaultValue) const
{
    if (aKey)
    {
        if (keyExists(aKey))
            return cpccKeyValueStr::get(aKey);
        /*
        auto searchIterator = m_map.find(aKey);
        if (searchIterator != m_map.end())
            return searchIterator->second;
         */
    }

    return aDefaultValue ? aDefaultValue : _T("");
}


inline const int cpccKeyValue::loadFromSimpleString(const cpcc_char* aKeyValueList)
{
    clear();
    dataHasChanged();
    return addFromSimpleString(aKeyValueList);
}


inline const int cpccKeyValue::addFromSimpleString(const cpcc_char* aKeyValueList)
{
    // see also:
    // https://stackoverflow.com/questions/27006958/parsing-key-value-pairs-from-a-string-in-caz
    if (!aKeyValueList)
        return 0;

    int nPairs = 0;
    // this works only on non-unicode programs
    /*
        cpcc_string key, val;
        cpcc_istringstream iss(aKeyValueList);
        while (std::getline(std::getline(iss, key, '=') >> std::ws, val))
            m_map[key] = val;
    */

    cpcc_string s(aKeyValueList);
    cpcc_string::size_type key_pos = 0;
    cpcc_string::size_type key_end;
    cpcc_string::size_type val_pos;
    cpcc_string::size_type val_end;

    s.erase(std::remove(s.begin(), s.end(), _T('\r')), s.end());

    while ((key_end = s.find(_T('='), key_pos)) != cpcc_string::npos)
    {
        if ((val_pos = s.find_first_not_of(_T("="), key_end)) == cpcc_string::npos)
            break;

        // todo: na balo kai to \r eite na to sbiso eksarxis
        val_end = s.find(_T('\n'), val_pos);
        m_map[s.substr(key_pos, key_end - key_pos)] = s.substr(val_pos, val_end - val_pos);
        ++nPairs;

        key_pos = val_end;
        if (key_pos != cpcc_string::npos)
            ++key_pos;
    }

    dataHasChanged();
    return nPairs;

    /*
    other way:
    const cpcc_char *recordSeparator = _T('\n');
     for (const cpcc_string& tag : std::split(aKeyValueList, recordSeparator))
     {
         auto key_val = std::split(aKeyValueList, _T('='));
         m_map[key_val[0]] = key_val[1];
     }
    */
}



// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccKeyValue testing
//
// /////////////////////////////////////////////////////////////////////////////////////////////////


TEST_RUN(cpccKeyValue_test)
{
    const bool skipThisTest = false;

    if (skipThisTest)
    {
        TEST_ADDNOTE("Test skipped");
        return;
    }

    cpccKeyValue testSubject;

    testSubject.set(_T("bool-value-true"), true);
    testSubject.set(_T("value123"), 123);
    testSubject.set(_T("keyMike"), _T("Mike Value"));
    testSubject.set(_T("value1.23456789"), 1.23456789f);
    const cpcc_char* grText = _T("ΜιαΕβδομαδα\n\rΠροσπαθώ");
    testSubject.set(_T("GreekTextWithLineBreaks"), grText);
    time_t aTimeStamp = time(NULL);
    testSubject.set(_T("time_t_test"), aTimeStamp);

    TEST_EXPECT((testSubject.get(_T("bool-value-true"), false) == true) , _T("SelfTest #8622v: bool failed"));

    TEST_EXPECT((testSubject.keyExists(_T("eeerrrooor")) == false) , _T("SelfTest #8622: non-existing key found"));
    TEST_EXPECT((testSubject.get(_T("time_t_test"), (time_t)40000) == aTimeStamp) , _T("SelfTest #8622a: time_t failed"));

    int tmp_i = testSubject.get(_T("value123"), 45);
    TEST_EXPECT((tmp_i == 123) , _T("SelfTest #8622b: key not found"));

    float tmp_f = testSubject.get(_T("value1.23456789"), 1.333f);
    TEST_EXPECT((tmp_f == 1.23456789f) , _T("SelfTest #8622d: float write/read failed"));

    cpcc_string tmp_s(testSubject.get(_T("keyMike"), _T("ha he hi")));
    TEST_EXPECT((tmp_s.compare(_T("Mike Value")) == 0) , _T("SelfTest #8622f: string write/read failed"));

    tmp_s = testSubject.get(_T("GreekTextWithLineBreaks"), _T("iiii"));
    TEST_EXPECT((tmp_s.compare(grText) == 0) , _T("SelfTest #8622g: string write/read failed"));
    
    // test loadFromSimpleString
    int n = testSubject.loadFromSimpleString(_T("key1=ena\n\rkey2=dyo\nkey3=tria"));
    TEST_EXPECT( n==3, _T("SelfTest #8622f4: number of entries"));
    TEST_EXPECT((testSubject.get(_T("key1"), _T("null")).compare(_T("ena")) == 0) , _T("SelfTest #8622f1: loadFromSimpleString failed"));
    TEST_EXPECT((testSubject.get(_T("key2"), _T("null")).compare(_T("dyo")) == 0) , _T("SelfTest #8622f2: loadFromSimpleString failed"));
    TEST_EXPECT((testSubject.get(_T("key3"), _T("null")).compare(_T("tria")) == 0) , _T("SelfTest #8622f3: loadFromSimpleString failed"));

}


