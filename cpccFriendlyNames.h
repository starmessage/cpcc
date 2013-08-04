/*  *****************************************
 *  File:		cpccFriendlyNames.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>
#include <map>
#include <assert.h>



//////////////////////////////////////////////////////////
///		class cpccFriendlyNames
//////////////////////////////////////////////////////////



/** A small and portable (cross platform) C++ class 
	that provides easy management of pairs of original names and friendly names.
    Example: Original name=Ea400, Friendly Name= Mike's bluetooth earphone
	Dependencies: cpcc_SelfTest.h
*/



class cpccFriendlyNames
{
public:	// class metadata and selftest
	const char * getClassVersion(void) { return "0.50"; };
	
	static void selfTest(void) 
	{
		
		cpccFriendlyNames fn;
		fn.setFriendlyName("key1", "firstValue");
		fn.setFriendlyName("our-cat", "Jerry");
		fn.setFriendlyName("our-mouse", "Tom");
		
		assert(fn.getFriendlyName("key1").compare("firstValue")==0 && 
			   "#7641a: cpccFriendlyNames.getFriendlyName()");
		
		assert(fn.getCount()==3 &&  
			   "#7641b: cpccFriendlyNames.getCount()");

		fn.setFriendlyName("key1", "updatedValue");
		assert(fn.getFriendlyName("key1").compare("updatedValue")==0 &&  
			   "#7641c: cpccFriendlyNames.getFriendlyName()");
		
		assert(fn.getOriginalName("Jerry").compare("our-cat")==0 &&  
			   "#7641d: cpccFriendlyNames.getOriginalName()");
				
		assert(fn.getOriginalName("NON-EXISTING").compare("")==0 &&  
			   "#7641g: cpccFriendlyNames.getOriginalName()");
		
		assert(fn.getFriendlyOrOriginalName("key111").compare("key111")==0 &&  
			   "#7641e: cpccFriendlyNames.getFriendlyOrOriginalName()");
	
		assert(fn.getFriendlyOrOriginalName("our-mouse").compare("Tom")==0 && 
			   "#7641f: cpccFriendlyNames.getFriendlyOrOriginalName()");
	
		fn.clear();
		assert(fn.getCount()==0 && 
			   "#7641k: cpccFriendlyNames.clear()");
	};

	
	typedef std::map<std::string, std::string> tFriendlyNames;

private:
	tFriendlyNames mFriendlyNames;
	
protected:

public:
	
	void clear(void)	
	{
		mFriendlyNames.clear();
	};
	
	
	const int getCount(void)
	{
		return mFriendlyNames.size();
	};
	
	
	void setFriendlyName(const char *aOriginalName, const char *aFriendlyName)
	{
		mFriendlyNames[std::string(aOriginalName)] = std::string(aFriendlyName);
	};
	
	
	/**
	 if no friendly name is found, it returns an empty string
	 */
	std::string getFriendlyName(const char *aOriginalName)
	{
		std::string key=aOriginalName;		
		tFriendlyNames::iterator position = mFriendlyNames.find(key);
	
		if (position!=mFriendlyNames.end())
			return mFriendlyNames[key];
		else
			return std::string("");  		
	};
	
	
	/**
	 returns the friendly name or the original name in case no friendly name is registered	 
	 */
	std::string getFriendlyOrOriginalName(const char *aOriginalName)
	{
		std::string friendlyName=getFriendlyName(aOriginalName);
		if (friendlyName.length()==0)
			return std::string(aOriginalName);
		
		return friendlyName;
	};
	
	
	/**
	 returns the original name of the friendly name.
	 returns an empty string if the friendly name is not registered
	 */
	std::string getOriginalName(const char *aFriendlyName)
	{
		tFriendlyNames::iterator position = mFriendlyNames.begin();
		while(position != mFriendlyNames.end())
		{
			if (position->second.compare(aFriendlyName)==0)
				return position->first;
			++position;
		};
		return std::string("");
	};
							  
};



