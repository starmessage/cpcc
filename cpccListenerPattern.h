 /*  *****************************************
 *  File:		cpccListenerPattern.h
 *  Version:	see function getClassVersion()
 *	Purpose:	a template Listener pattern
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once
#ifndef CPCCLISTENERPATTERN_H
#define CPCCLISTENERPATTERN_H

#include <string>
#include <set>
#include <assert.h>


// ---------------- class cmiListener  -----------------
template<typename Tobject> 
class cpccListener
{
public:	// class metadata
	const char * getClassVersion(void) { return "1.01"; };
	
private:
	int _nSubscriptions;

public: // data
	std::string name;

public: // ctors
	cpccListener(const char *aName="Unnamed cpcclistener"): 
		name(aName), 
		_nSubscriptions(0)
	{ };

	~cpccListener()
	{	
		assert((_nSubscriptions==0) && "#2352: destroying cpcclistener, but is still subscribed");
	};
	
public: // functions

	void virtual beNotifiedAbout(Tobject &anObject) = 0;

	int getNumSubsciptions(void)	{ return _nSubscriptions; } ;

	void increaseSubscriptions(void) 
	{ 
		assert((_nSubscriptions>=0) && "#5370: _nSubscriptions<0");
		_nSubscriptions++; 
	};

	void decreaseSubscriptions(void) 
	{ 
		assert((_nSubscriptions>0) && "#5371: _nSubscriptions<=0");
		_nSubscriptions--; 
	};
};



template <typename Tobject>
class cpccListenersList 
{
private:	
	std::set<cpccListener<Tobject> *> _listenersList;
	
public: //constructors
	
	cpccListenersList()
	{
		assert(countListeners()==0 && "#3415 cmiListenersList constructor: Not empty list");
	};
	
	~cpccListenersList()
	{
		// at this point if the program is terminating
		// you cannot have any calls to IO like std::cout << "Destructor cmiListenersList"
		assert((countListeners()==0) && "#5274: destroying cmiListenersList, but listeners still exist");
	};

public: //functions

	void notifyListeners(Tobject &anObject)
	{	int n=countListeners();
		typename std::set<cpccListener<Tobject> *>::iterator itr;
		for (itr = _listenersList.begin(); itr != _listenersList.end(); itr++ )
			{
			(*itr)->beNotifiedAbout(anObject);
			n--;
			}
		assert(n==0 && "#8246: notifyListeners iterator did not work properly");
	};
	
	
	void addListener(cpccListener<Tobject> & o)
	{	_listenersList.insert(&o);	
		o.increaseSubscriptions();
	};

	void removeListener(cpccListener<Tobject> & o)
	{	_listenersList.erase(&o);	
		o.decreaseSubscriptions();
	};



	int countListeners(void) 
		{	return _listenersList.size();	};
		
};



#endif // CPCCLISTENERPATTERN_H