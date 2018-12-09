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
#include "core.cpccIdeMacros.h"
#include "core.cpccStringUtil.h"
#include "cpccUnicodeSupport.h"


/** A small and portable (cross platform) C++ class 
	providing a std:map that can store any value type (primitive data only)
		
	Dependencies: cpcc_SelfTest.h
*/



///////////////////////////////////////////////////////////////////////////////////////////////////
//		cpccKeyValue
///////////////////////////////////////////////////////////////////////////////////////////////////

class cpccKeyValue: private strConvertionsV3
{
public:
    typedef std::map<cpcc_string, cpcc_string> tKeysAndValues;
    tKeysAndValues	m_map;
	
public:		// generic functions

    void        removeKey(const cpcc_char *aKey) 
    {   
        if (!aKey)
            return;
        m_map.erase(aKey);
        dataChanged();
    }
    
    
    void		clear(void) 
    { 
        if (m_map.size()==0)
            return;

        m_map.clear(); 
        dataChanged();
    }


    const bool	keyExists(const cpcc_char *aKey) const 
    {
		if (!aKey) return false;
        //const cpcc_string keyStr(aKey);
        // return (m_map.find(keyStr) != m_map.end()); 
		return (m_map.find(aKey) != m_map.end()); 
    }
    
	// called when set() functions are called. 
	// A descendant of this class can override this function to implement further actions,
	// e.g. saving to a file
	virtual void dataChanged(void) { }

public:		// get functions

    virtual const cpcc_string	get(const cpcc_char *aKey, const cpcc_char *aDefaultValue)
    {
        if (aKey)
            if (keyExists(aKey))
                return m_map[aKey];
        return aDefaultValue ? aDefaultValue : _T("");
    }


    const cpcc_string    get(const cpcc_char *aKey, const cpcc_string &aDefaultValue)  
	{   return get(aKey, aDefaultValue.c_str());    }


    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type >
    const T    get(const cpcc_char *aKey, const T aDefaultValue)
    {
        cpcc_string valueStr(get(aKey, _T("not-fOunD-there")));
        if (valueStr.compare(_T("not-fOunD-there")) == 0)
            return aDefaultValue;
        
        // if (!std::is_same<T, cpcc_char *>::value)
        return fromString(valueStr.c_str(), aDefaultValue);
    }
    
public:		// set functions

	// does the final set() of the pair, implementing also write caching and signaling to descendant classes
	inline void setRaw(const cpcc_char   *aKey, const cpcc_char *aValue) 
	{
		if (!aValue || !aKey) return;
		
		// caching here 
		if (keyExists(aKey))
			if (m_map[aKey].compare(aValue)==0)
				return;	// the value is already there.
				
		// set
		m_map[aKey] = aValue;
		dataChanged(); // let descendant classes know that the data has changes so they need to save it somewhere
	}
	
	template <typename T>
	void		set(const cpcc_char   *aKey, const T aValue)              	{ setRaw(aKey, toString(aValue).c_str()) ;  }
	void        set(const cpcc_char   *aKey, const cpcc_string &aValue)		{ setRaw(aKey, aValue.c_str());  }
    // void        set(const cpcc_string &aKey, const cpcc_string &aDefaultValue) { m_map[aKey] = aDefaultValue; }
    void		set(const cpcc_char   *aKey, const cpcc_char *aValue)		{ setRaw(aKey, aValue);  }


public:	// class metadata and selftest

	static void selfTest(void)
	{
    #ifdef cpccDEBUG
		cpccKeyValue testSubject;

        testSubject.set(_T("bool-value-true"), true);

		testSubject.set(_T("value123"), 123);
		testSubject.set(_T("keyMike"), _T("Mike Value"));
		testSubject.set(_T("value1.23456789"), 1.23456789f);
        const cpcc_char *grText = _T("ΜιαΕβδομαδα\n\rΠροσπαθώ");
        testSubject.set(_T("GreekTextWithLineBreaks"), grText);
        time_t aTimeStamp = time(NULL);
        testSubject.set(_T("time_t_test"), aTimeStamp);
        
        assert((testSubject.get(_T("bool-value-true"), false) == true) && _T("SelfTest #8622v: bool failed"));

		assert((testSubject.keyExists(_T("eeerrrooor")) == false) && _T("SelfTest #8622: non-existing key found"));
        assert((testSubject.get(_T("time_t_test"), (time_t) 40000) == aTimeStamp) && _T("SelfTest #8622a: time_t failed"));

        int tmp_i = testSubject.get(_T("value123"), 45);
		assert((tmp_i == 123) && _T("SelfTest #8622b: key not found"));
		
		float tmp_f = testSubject.get(_T("value1.23456789"), 1.333f);
		assert((tmp_f == 1.23456789f) && _T("SelfTest #8622d: float write/read failed"));

		cpcc_string tmp_s(testSubject.get(_T("keyMike"), _T("ha he hi")));
		assert((tmp_s.compare(_T("Mike Value"))==0) && _T("SelfTest #8622f: string write/read failed"));

        tmp_s = testSubject.get(_T("GreekTextWithLineBreaks"), _T("iiii"));
        assert((tmp_s.compare(grText) == 0) && _T("SelfTest #8622g: string write/read failed"));

        

    #endif
	}
};

