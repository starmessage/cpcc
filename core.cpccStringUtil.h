/*  *****************************************
 *  File:		core.cpccStringUtil.h
 *	Purpose:	Portable (cross-platform), light-weight library
 *  			String utility functions
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
#include <assert.h>
#include <sstream>
#include <time.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "cpccUnicodeSupport.h"

 
typedef std::vector<cpcc_string> cpcc_stringList;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class stringUtils
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class stringUtils
{

public:
    static inline void selfTest(void);
    
    static inline void stringTrimL(cpcc_string &s, const cpcc_string &charsToRemove = _T("\t\n\v\f\r "));
    static inline void stringTrimR(cpcc_string &s, const cpcc_string &charsToRemove = _T("\t\n\v\f\r "));
    static inline void stringTrim(cpcc_string &s, const cpcc_string &charsToRemove = _T("\t\n\v\f\r "));
    
	static inline void removeFromStringToTheEnd(cpcc_string& str, const cpcc_char* aChar);


    template<typename aPCharType>
    static bool stringEndsWith(const std::basic_string<aPCharType> &aStr, const aPCharType *aSuffix)
    {
        std::basic_string<aPCharType> suffixStr(aSuffix);
        return stringEndsWith(aStr, suffixStr);
    }
    
    
    template<typename aPCharType>
    static bool stringEndsWith(const std::basic_string<aPCharType> &aStr, const std::basic_string<aPCharType> &aSuffix)
    {
        if (aSuffix.size() > aStr.size()) return false;
        return std::equal(aSuffix.rbegin(), aSuffix.rend(), aStr.rbegin());
    }
    
    
	static bool stringStartsWith(const cpcc_char *aStr, const cpcc_char *aPrefix)
	{
    #ifdef _WIN32
		#pragma warning( suppress : 4996 )
		return (cpcc_strnicmp(aStr, aPrefix, cpcc_strlen(aPrefix)) == 0);
    #else
        // Unix libraries use strcasecmp, from <strings.h>.
        return (strncasecmp(aStr, aPrefix, strlen(aPrefix)) == 0);
    #endif
	}

    
	// todo: to delete. Check if needed for the mac #define stricmp strcasecmp
	static bool stringsAreEqual(const cpcc_char *aStr1, const cpcc_char *aStr2)
	{
    #ifdef _WIN32
        #pragma warning( suppress : 4996 )
        return (cpcc_stricmp(aStr1, aStr2) == 0);
    #else
        // Unix libraries use strcasecmp, from <strings.h>.
        return (strcasecmp(aStr1, aStr2) == 0);
    #endif
	}


	
    

	// a pity the whole std library does not contain a ready function for this tasl
	static void findAndReplaceAll(cpcc_string& source, const cpcc_char* find, const cpcc_char* replace)
	{
		if (!find || !replace)
			return;

		size_t findLen = cpcc_strlen(find);
		size_t replaceLen = cpcc_strlen(replace);
		size_t pos = 0;

		while ((pos = source.find(find, pos)) != std::string::npos)
		{
			source.replace(pos, findLen, replace);
			pos += replaceLen;
		}
	}


	static void stringSplit(const cpcc_string &inputStr, const cpcc_char delimiter, cpcc_stringList &outputList)
	{
		size_t startPos = 0;
		size_t endPos = inputStr.find_first_of(delimiter);
		outputList.clear();
	
		while (endPos <= cpcc_string::npos)
		{
			outputList.push_back(inputStr.substr(startPos, endPos - startPos));

			if (endPos == cpcc_string::npos)
				break;

			startPos = endPos + 1;
			endPos = inputStr.find_first_of(delimiter, startPos);
		}
	}

};



/// --------------------------------------
/// Implementation of stringUtils
/// --------------------------------------


inline void stringUtils::selfTest(void)
{
    // logObjectLife logThis("stringUtils::selfTest()");
    cpcc_string s(_T(" hello  "));
    
    stringTrimL(s);
    assert((s.compare(_T("hello  ")) == 0) && "#2381a: stringTrimL");
    
    stringTrimR(s);
    assert((s.compare(_T("hello")) == 0) && "#2381b: stringTrimR");
 
    s = _T("   ");
    
    stringTrim(s);
    assert((s.compare(_T("")) == 0) && "#2381c: stringTrim");
    
    stringTrim(s);
    assert((s.compare(_T("")) == 0) && "#2381d: stringTrim");

	s = _T("word1.word2   ");
	removeFromStringToTheEnd(s, _T("."));
	assert((s.compare(_T("word1")) == 0) && "#2381e: removeFromStringToTheEnd");

	removeFromStringToTheEnd(s, _T("2"));
	assert((s.compare(_T("word1")) == 0) && "#2381f: removeFromStringToTheEnd");

	removeFromStringToTheEnd(s, _T("ord1"));
	assert((s.compare(_T("w")) == 0) && "#2381h: removeFromStringToTheEnd");

	s = _T("w");
	removeFromStringToTheEnd(s, _T(""));
	assert((s.compare(_T("w")) == 0) && "#2381g: removeFromStringToTheEnd");

	s = _T("");
	removeFromStringToTheEnd(s, _T("xxx"));
	assert((s.compare(_T("")) == 0) && "#2381k: removeFromStringToTheEnd");

}


// trim from start (in place)
inline void stringUtils::stringTrimL(cpcc_string &s, const cpcc_string &charsToRemove)
{
    // 1st Way
    // s.erase(s.begin(), std::find_if(s.begin(), s.end(),
    //    std::not1(std::ptr_fun<int, int>(std::isspace))));
    
    // 2nd way
    // s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); } ));
    
    // 3rd way
    s.erase(0, s.find_first_not_of(charsToRemove));
}


// trim from end (in place)
inline void stringUtils::stringTrimR(cpcc_string &s, const cpcc_string &charsToRemove)
{
    // 1st Way
    //s.erase(std::find_if(s.rbegin(), s.rend(),
    //    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    
    // 2nd way
    //  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch);} ).base(), s.end());
    
    // 3rd way
    s.erase(s.find_last_not_of(charsToRemove) + 1);
}


// trim from both ends (in place)
inline void stringUtils::stringTrim(cpcc_string &s, const cpcc_string &charsToRemove)
{
    stringTrimL(s, charsToRemove);
    stringTrimR(s, charsToRemove);
}


inline void stringUtils::removeFromStringToTheEnd(cpcc_string& str, const cpcc_char* aSubString)
{
	if (!aSubString)
		return;

	if (cpcc_strlen(aSubString) == 0)
		return;

	size_t pos = str.find(aSubString);
	if (pos == std::string::npos)
		return;

	str.erase(pos);
}


// --------------------------------------
//  class strConvertionsV3
// --------------------------------------

class strConvertionsV3
{
public:
    static cpcc_string toString(const bool value) { return (value) ? _T("yes") : _T("no"); }
    static cpcc_string toString(const cpcc_string &value) { return value; }
    static cpcc_string toString(const cpcc_char *value) { return (value) ? value : _T(""); }

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type >
    static cpcc_string toString(const T value)
    {
        cpcc_stringstream ss;
        ss.precision(14);
        ss << value;
        return ss.str();
    }
    
    //////////////////////////////////////////////////////

    static const cpcc_string fromString(const cpcc_string &aStr, const cpcc_string &defaultValue)     { return aStr;  }
    static const cpcc_string fromString(const cpcc_string &aStr, const cpcc_char *defaultValue)       { return aStr; }
    static const cpcc_string fromString(const cpcc_char   *aStr, const cpcc_char *defaultValue)       { return (aStr)?aStr:(defaultValue?defaultValue:_T("")); }
    
    static const bool fromString(const cpcc_char *aStr, const bool aDefaultValue)
    {
        cpcc_string strValue(aStr ? aStr : _T(""));

        if (strValue.compare(_T("yes")) == 0 || strValue.compare(_T("true")) == 0 || strValue.compare(_T("1")) == 0 || strValue.compare(_T("on")) == 0)
            return true;

        if (strValue.compare(_T("no")) == 0 || strValue.compare(_T("false")) == 0 || strValue.compare(_T("0")) == 0 || strValue.compare(_T("off")) == 0)
            return false;

        return aDefaultValue;
    }
    
    /*
    template <typename T>
    static T fromString(const cpcc_string &aStr, const T defaultValue)
    {
        cpcc_stringstream ss(aStr);
        T result;
        return (ss >> result) ? result : defaultValue;
    }
    */
    
    template <typename T>
    static const T fromString(const cpcc_char *aStr, const T defaultValue)
    {
        if (!aStr)
            return defaultValue;
        
        cpcc_stringstream ss(aStr);
        T result(0);
        return (ss >> result)? result : defaultValue;
    }
    
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//		stringConversions
///////////////////////////////////////////////////////////////////////////////////////////////////

// Todo: maybe not needed anymore. To delete?
/*
class stringConversions
{

	// new way
	static const bool fromStrNew(const cpcc_char*  strValue, cpcc_string& aValue)
	{
		aValue = strValue;
		return true;
	}

	static const bool fromStrNew(const cpcc_string&  strValue, bool& aValue)
	{
		if (strValue.compare(_T("yes")) == 0 || strValue.compare(_T("true")) == 0 || strValue.compare(_T("1")) == 0 || strValue.compare(_T("on")) == 0)
			{ aValue = true;  return true; }

		if (strValue.compare(_T("no")) == 0 || strValue.compare(_T("false")) == 0 || strValue.compare(_T("0")) == 0 || strValue.compare(_T("off")) == 0)
			{ aValue = true;  return true; }

		return false;
	}


	static const bool fromStrNew(const cpcc_char* strValue, double& aValue)
	{
		cpcc_char* end;
		double n = cpcc_strtod(strValue, &end);
		if (end <= strValue)	return false;
		aValue = n;
		return true;
	}

	static const bool fromStrNew(const cpcc_char* strValue, float& aValue)
	{
		cpcc_char* end;
		float n = (float)cpcc_strtod(strValue, &end);
		if (end <= strValue)	return false;
		aValue = n;
		return true;
	}

	// old way

	static const double fromStr(const cpcc_char* strValue, const double aDefaultValue)
	{
		cpcc_char* end;
		double n = cpcc_strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}


    static const float fromStr(const cpcc_string &strValue, const float aDefaultValue)
    {
        // http://www.cplusplus.com/reference/string/stod/
        // http://www.cplusplus.com/reference/string/stof/
        // http://www.cplusplus.com/forum/articles/9645/

        cpcc_stringstream ss(strValue);
        float result;
        return (ss >> result) ? result : aDefaultValue;
    }

       
	static const float fromStr(const cpcc_char* strValue, const float aDefaultValue)
	{
        cpcc_char* end;
		float n = (float) cpcc_strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
        
        // return fromStr(cpcc_string(strValue), aDefaultValue);
	}


	static const long fromStr(const cpcc_char*  strValue, const long aDefaultValue)
	{
		cpcc_char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		long n = cpcc_strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

#ifndef __APPLE__
    // in OSX, time_t is defined as long, so there is a ready function for it
	static const time_t fromStr(const cpcc_char*  strValue, const time_t aDefaultValue)
	{
		cpcc_char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		time_t n = cpcc_strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}
#endif

	static const bool fromStrNew(const cpcc_char*  strValue, int& aValue)
	{
		cpcc_char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		int n = (int)cpcc_strtol(strValue, &end, 0);
		if (end <= strValue)	return false;
		aValue = n;
		return true;
	}

	static const int fromStr(const cpcc_char*  strValue, const int aDefaultValue)
	{
		cpcc_char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		int n = (int) cpcc_strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}



    //static cpcc_string toStr(bool value) { return cpcc_string(value ? "yes" : "no"); }

#ifndef __APPLE__
    // in OSX, time_t is defined as long, so there is a ready function for it
	// in Windows time_t is _int64
	static cpcc_string toStr(const time_t value) { cpcc_ostringstream ss; ss << value; return ss.str(); }

#endif

	static cpcc_string toStr(const long int value)	{ cpcc_ostringstream ss; ss << value; return ss.str();	}
	static cpcc_string toStr(const int value) { cpcc_ostringstream  ss; ss << value; return ss.str(); }

	static cpcc_string toStr(const float value)
	{
        cpcc_ostringstream  ss; 
        ss.precision(12);
        ss << value; 
        return ss.str();
	//	cpcc_char buf[100];
    //    #pragma warning(disable : 4996)
	//	cpcc_sprintf(buf, _T("%.12f"), value);
	//	return cpcc_string(buf);
        
	}



	static cpcc_string toStr(const double value)
	{
        cpcc_ostringstream  ss;
        ss.precision(15);
        ss << value;
        return ss.str();
        
		//cpcc_char buf[200];
        //#pragma warning(disable : 4996)
		//cpcc_sprintf(buf, _T("%.12f"), value);
		//return cpcc_string(buf);
        
	}
};

*/
