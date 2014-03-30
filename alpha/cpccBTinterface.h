/*  *****************************************
 *  File:		cpccBTinterface.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, bluetooth library
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
#include <vector>
#include "cpccFriendlyNames.h"

//////////////////////////////////////////////////////////
///		class cpccBTinterface
//////////////////////////////////////////////////////////



/** A small and portable (cross platform) C++ class 
	with basic bluetooth functions.
	Dependencies: cpcc_SelfTest.h
*/


/*

 references:

 IOBluetoothDeviceInquiry Class
 http://developer.apple.com/library/mac/#documentation/IOBluetooth/Reference/IOBluetoothDeviceInquiry_reference/translated_content/IOBluetoothDeviceInquiry.html#//apple_ref/doc/uid/TP40011424
 
 Performing Device Inquiries
 https://developer.apple.com/library/mac/#documentation/devicedrivers/conceptual/bluetooth/BT_Develop_BT_Apps/BT_Develop_BT_Apps.html#//apple_ref/doc/uid/TP30000997-CH216-BACFJHEB
 
 */


struct structBluetoothDevice
{
	std::string id, mac, name, label;
};


/// bluetooth functions
class cpccBTinterface
{
public:	// class metadata and selftest
	const char * getClassVersion(void) { return "0.50"; };
	static void selfTest(void) { };

	
public:
	typedef std::vector<structBluetoothDevice> BtDeviceList;

	
private:
	cpccFriendlyNames mDeviceLabels;
	
	
protected:

public:
	
	virtual const float	 getDeviceSignalStrenght(const char *DeviceName)=0;
	
	
	virtual const bool	 isBluetoothEnabled(void) 
	{	
		return false; 
	};
	
	
	virtual void		 getListOfVisibleDevices(BtDeviceList &aList)
	{
		aList.clear();
	};
	
	
	void setFriendlyName(const char *aDeviceId, const char *aFriendlyName)
	{
		mDeviceLabels.setFriendlyName(aDeviceId, aFriendlyName);
	};
	
							  
};



