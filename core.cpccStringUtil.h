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

	// todo: to delete. Na do an xreiazetai gia to mac #define stricmp strcasecmp
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


	static void removeFromCharacterToTheEnd(cpcc_string &str, const cpcc_char *aChar)
	{
		str.substr(0, str.find(aChar, 0));
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
		size_t start = 0;
		size_t end = inputStr.find_first_of(delimiter);
		outputList.clear();
	
		while (end <= cpcc_string::npos)
		{
			outputList.push_back(inputStr.substr(start, end - start));

			if (end == cpcc_string::npos)
				break;

			start = end + 1;
			end = inputStr.find_first_of(delimiter, start);
		}
	}

};



///////////////////////////////////////////////////////////////////////////////////////////////////
//		stringConversions
///////////////////////////////////////////////////////////////////////////////////////////////////
class stringConversions
{
private:
	typedef cpcc_char *tEncodingsTable[5][2];

	static const tEncodingsTable& encondingsINI(void)
	{
		// encode characters to fit to the INI file (e.g. multiline text)
		static const tEncodingsTable encodedINIcharacterTable =
		{
			//        normal     ->       encoded
			{ (cpcc_char *)  _T("="),  (cpcc_char *) _T("\\=") },
			{ (cpcc_char *)  _T("\n"), (cpcc_char *) _T("\\n") },
			{ (cpcc_char *)  _T("\r"), (cpcc_char *) _T("\\r") },
			{ (cpcc_char *)  _T("\t"), (cpcc_char *) _T("\\t") },
			// this must be the last rule  for the decoding and first rule for the encoding
			{ (cpcc_char *)  _T("\\"), (cpcc_char *) _T("\\\\") }
		};
		return encodedINIcharacterTable;
	}

public:



	static void    encodeStrForINI(cpcc_string &str)
	{
		for (int i = (sizeof(encondingsINI()) / sizeof(encondingsINI()[0]))-1; i>=0; --i)
			stringUtils::findAndReplaceAll(str, encondingsINI()[i][0], encondingsINI()[i][1]);
	}

	static void    decodeStrFromINI(cpcc_string &str)
	{
		//for (int i = 0; i< sizeof(encodedINIcharacterTable) / sizeof(encodedINIcharacterTable[0]); ++i)
		//	findAndReplaceAll(str, encodedINIcharacterTable[i][1], encodedINIcharacterTable[i][0]);
		for (unsigned int i = 0; i< sizeof(encondingsINI()) / sizeof(encondingsINI()[0]); ++i)
			stringUtils::findAndReplaceAll(str, encondingsINI()[i][1], encondingsINI()[i][0]);
	}


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




	static const float fromStr(const cpcc_char* strValue, const float aDefaultValue)
	{
		cpcc_char* end;
		float n = (float)cpcc_strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
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


	static const bool fromStr(const cpcc_char*  strValue, const bool aDefaultValue)  { return fromStr(cpcc_string(strValue), aDefaultValue); }
    
	static const bool fromStr(const cpcc_string&  strValue, const bool aDefaultValue)
	{
		if (strValue.compare(_T("yes")) == 0 || strValue.compare(_T("true")) == 0 || strValue.compare(_T("1")) == 0 || strValue.compare(_T("on")) == 0)
			return true;

		if (strValue.compare(_T("no")) == 0 || strValue.compare(_T("false")) == 0 || strValue.compare(_T("0")) == 0 || strValue.compare(_T("off")) == 0)
			return false;

		return aDefaultValue;
	}

	static cpcc_string toStr(const bool value) { return cpcc_string(value ? _T("yes") : _T("no")); }
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
		cpcc_char buf[100];
        #pragma warning(disable : 4996)
		cpcc_sprintf(buf, _T("%.12f"), value);
		return cpcc_string(buf);
	}

	static cpcc_string toStr(const double value)
	{
		cpcc_char buf[200];
        #pragma warning(disable : 4996)
		cpcc_sprintf(buf, _T("%.12f"), value);
		return cpcc_string(buf);
	}
};

