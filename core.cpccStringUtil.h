/*  *****************************************
 *  File:		core.cpccStringUtil.h
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
#include <assert.h>
#include <sstream>
#include <time.h>
#include "cpccUnicodeSupport.h"


/** A small and portable (cross platform) C++ class 
	with string utility functions
*/





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
			{ (cpcc_char *)  "=", (cpcc_char *) "\\=" },
			{ (cpcc_char *)  "\n", (cpcc_char *) "\\n" },
			{ (cpcc_char *)  "\r", (cpcc_char *) "\\r" },
			{ (cpcc_char *)  "\t", (cpcc_char *) "\\t" },
			// this must be the last rule  for the decoding and first rule for the encoding
			{ (cpcc_char *)  "\\", (cpcc_char *) "\\\\" }	
		};
		return encodedINIcharacterTable;
	}


public:
	// a pity the whole std library does not contain a ready function for this
	static void findAndReplaceAll(cpcc_string& source, const cpcc_char* find, const cpcc_char* replace)
	{
		if (!find || !replace)
			return;

		size_t findLen = strlen(find);
		size_t replaceLen = strlen(replace);
		size_t pos = 0;

		while ((pos = source.find(find, pos)) != std::string::npos)
		{
			source.replace(pos, findLen, replace);
			pos += replaceLen;
		}
	}

	static void    encodeStrForINI(cpcc_string &str)
	{
		for (int i = (sizeof(encondingsINI()) / sizeof(encondingsINI()[0]))-1; i>=0; --i)
			findAndReplaceAll(str, encondingsINI()[i][0], encondingsINI()[i][1]);
	}

	static void    decodeStrFromINI(cpcc_string &str)
	{
		//for (int i = 0; i< sizeof(encodedINIcharacterTable) / sizeof(encodedINIcharacterTable[0]); ++i)
		//	findAndReplaceAll(str, encodedINIcharacterTable[i][1], encodedINIcharacterTable[i][0]);
		for (int i = 0; i< sizeof(encondingsINI()) / sizeof(encondingsINI()[0]); ++i)
			findAndReplaceAll(str, encondingsINI()[i][1], encondingsINI()[i][0]);
	}


	// new way
	static const bool fromStrNew(const cpcc_char*  strValue, cpcc_string& aValue)
	{
		aValue = strValue;
		return true;
	}

	static const bool fromStrNew(const cpcc_string&  strValue, bool& aValue)
	{
		if (strValue.compare("yes") == 0 || strValue.compare("true") == 0 || strValue.compare("1") == 0 || strValue.compare("on") == 0)
			{ aValue = true;  return true; }

		if (strValue.compare("no") == 0 || strValue.compare("false") == 0 || strValue.compare("0") == 0 || strValue.compare("off") == 0)
			{ aValue = true;  return true; }

		return false;
	}

	static const bool fromStrNew(const cpcc_char*  strValue, int& aValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		int n = strtol(strValue, &end, 0);
		if (end <= strValue)	return false;
		aValue = n;
		return true;
	}


	static const bool fromStrNew(const cpcc_char* strValue, double& aValue)
	{
		char* end;
		double n = strtod(strValue, &end);
		if (end <= strValue)	return false;
		aValue = n;
		return true;
	}

	static const bool fromStrNew(const cpcc_char* strValue, float& aValue)
	{
		char* end;
		float n = (float)strtod(strValue, &end);
		if (end <= strValue)	return false;
		aValue = n;
		return true;
	}

	// old way

	static const double fromStr(const cpcc_char* strValue, const double aDefaultValue)
	{
		char* end;
		double n = strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}




	static const float fromStr(const cpcc_char* strValue, const float aDefaultValue)
	{
		char* end;
		float n = (float)strtod(strValue, &end);
		return end > strValue ? n : aDefaultValue;
	}


	static const long fromStr(const cpcc_char*  strValue, const long aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		long n = strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

#ifndef __APPLE__
    // in OSX, time_t is defined as long, so there is a ready function for it
	static const time_t fromStr(const cpcc_char*  strValue, const time_t aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		time_t n = strtoll(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}
#endif

	static const int fromStr(const cpcc_char*  strValue, const int aDefaultValue)
	{
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		int n = strtol(strValue, &end, 0);
		return end > strValue ? n : aDefaultValue;
	}

	static const bool fromStr(const cpcc_char*  strValue, const bool aDefaultValue)  { return fromStr(cpcc_string(strValue), aDefaultValue); }
    
	static const bool fromStr(const cpcc_string&  strValue, const bool aDefaultValue)
	{
		if (strValue.compare("yes") == 0 || strValue.compare("true") == 0 || strValue.compare("1") == 0 || strValue.compare("on") == 0)
			return true;

		if (strValue.compare("no") == 0 || strValue.compare("false") == 0 || strValue.compare("0") == 0 || strValue.compare("off") == 0)
			return false;

		return aDefaultValue;
	}

	static cpcc_string toStr(const bool value) { return cpcc_string(value ? "yes" : "no"); }
    //static cpcc_string toStr(bool value) { return cpcc_string(value ? "yes" : "no"); }

#ifndef __APPLE__
    // in OSX, time_t is defined as long, so there is a ready function for it
	static cpcc_string toStr(const long int value)
	{
		std::ostringstream ss; ss << value; return ss.str();
	}
#endif
	
	static cpcc_string toStr(const time_t value)
	{
		std::ostringstream ss; ss << value; return ss.str();
	}

	
	static cpcc_string toStr(const int value)
	{
		std::ostringstream  ss; ss << value; return ss.str();
	}

	static cpcc_string toStr(const float value)
	{
		char buf[100];
        #pragma warning(disable : 4996)
		sprintf(buf, "%.12f", value);
		return cpcc_string(buf);
	}

	static cpcc_string toStr(const double value)
	{
		char buf[200];
        #pragma warning(disable : 4996)
		sprintf(buf, "%.12f", value);
		return cpcc_string(buf);
	}
};




