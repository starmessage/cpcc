/*  *****************************************
 *  File:		data.cpccKeyValueStr.h
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2020 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			    Commercial license for closed source projects.
 *	Web:		    http://www.StarMessageSoftware.com/cpcclibrary
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <map>
#include <algorithm>
#include "data.cpccWideCharSupport.h"


/** 
    A small and portable (cross platform) C++ class 
	Provides a std:map that can store any (primitive type) value
		
*/

// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//        class cpccKeyValueStr
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
class cpccKeyValueStr
{
private:
    typedef std::basic_string<TCHAR> tStringWN;
    typedef std::map<tStringWN, tStringWN> tKeysAndValues;

protected:
    tKeysAndValues    m_map;

public:
    
    typedef void (tEncodingFunc(tStringWN &));
    // typedef struct { TCHAR *recordSeparator, bool addRecordSepararorEof, TCHAR *keyValueSeparator; } tSerializeConfig;
    
    void                removeKey(const TCHAR* aKey);
    void                clear(void);
    const size_t          getCount(void) const { return m_map.size(); }
    const bool          keyExists(const TCHAR* aKey) const;

    const tStringWN     get(const TCHAR* aKey) const;
    
    void set(const TCHAR* aKey, const TCHAR* aValue);
    void set(const TCHAR* aKey, const tStringWN &aValue) { set(aKey, aValue.c_str()); }
    const tKeysAndValues &getMap(void) const { return m_map; }
    
    const tStringWN     serialize(const TCHAR* aRecordSeparator, const bool addRecordSeparatorToTheEnd, tEncodingFunc encodingFuncPtr) const;
    
    bool isEqual(const cpccKeyValueStr & other) const;
    
protected:
    // called when set() functions are called.
    // A descendant of this class can override this function to implement further actions,
    // e.g. saving to a file
    virtual void dataHasChanged(void) { }
    
};


// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccKeyValue declaration
//
// /////////////////////////////////////////////////////////////////////////////////////////////////

inline bool cpccKeyValueStr::isEqual(const cpccKeyValueStr & other) const
{
    // compare std::map
    // https://stackoverflow.com/questions/8473009/how-to-efficiently-compare-two-maps-of-strings-in-c
    if (m_map.size() != other.m_map.size())
        return false;
    return std::equal(m_map.begin(), m_map.end(), other.m_map.begin());
}


inline const cpccKeyValueStr::tStringWN     cpccKeyValueStr::serialize(const TCHAR* aRecordSeparator, const bool addRecordSeparatorToTheEnd, tEncodingFunc encodingFuncPtr) const
{
    tStringWN result;
    const TCHAR *recordSeparator = "";
    for (auto element : m_map)
    {
        tStringWN    value(element.second);
        if (encodingFuncPtr)
            encodingFuncPtr(value);
        
        result += recordSeparator + element.first + _T("=") + value;
        
        if (aRecordSeparator)
            recordSeparator = aRecordSeparator;
    }
    if (addRecordSeparatorToTheEnd)
        result += recordSeparator;
    
    return result;
}


inline void cpccKeyValueStr::removeKey(const TCHAR* aKey)
{
    if (!aKey)
        return;
    m_map.erase(aKey);
    dataHasChanged();
}


inline void	cpccKeyValueStr::clear(void)
{
    if (m_map.size() == 0)
        return;

    m_map.clear();
    dataHasChanged();
}


inline const bool	cpccKeyValueStr::keyExists(const TCHAR* aKey) const
{
    if (!aKey)
        return false;
    return (m_map.find(aKey) != m_map.end());
}


inline void cpccKeyValueStr::set(const TCHAR* aKey, const TCHAR* aValue)
{
    if (!aValue || !aKey) return;

    // caching here 
    if (keyExists(aKey))
        if (m_map[aKey].compare(aValue) == 0)
            return;	// the value is already there.

    // set
    m_map[aKey] = aValue;
    dataHasChanged(); // let descendant classes know that the data has changes so they need to save it somewhere
}


inline const cpccKeyValueStr::tStringWN	cpccKeyValueStr::get(const TCHAR* aKey) const
{
    // amap[k]
    // If k matches the key of an element in the container, the function returns a reference to its mapped value.
    // If k does not match the key of any element in the container, the function inserts a new element with that key and returns a reference to its mapped value.
    // this is not a const function:
    
    if (aKey)
    {
        auto searchIterator = m_map.find(aKey);
        if (searchIterator != m_map.end())
            return searchIterator->second;
    }

    return STR_WN"";
}


