/*  *****************************************
 *  File:		app.cpcciShareware.h
 *	Purpose:	Portable (cross-platform), light-weight, screensaver helper class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2015 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *              https://code.google.com/p/cpcc/
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
#pragma once


/// shareware interface class
class cpcciShareware
{
public:	
	enum licenseExpiryType	{ freeTrial, fullVersionLicense, licenseForUpgrades };
	
	inline const bool  					fullVersionIsRegistered(void) { return getRemainingDaysUntilExpiry(fullVersionLicense)>=0; }
	virtual inline const int  			getRemainingDaysUntilExpiry(const licenseExpiryType aLicense) const =0;
	virtual const char * 				getBuyUrl(void) const =0;
	virtual inline const cpcc_char *	getComputerID(void) const =0;
	virtual inline const cpcc_char *	getCustomerID(void) const =0;
	virtual inline const cpcc_char *	getLicenseDetails(void) const =0;
	virtual inline const bool			applyLicenseCode(const cpcc_char * aLicenseCode) =0;
	virtual inline const bool			quickChecklLicenseCode(const cpcc_char * aLicenseCode) =0;
	virtual inline void					removeInstalledLicense(void) =0;
	virtual const long int				getDaysSinceFirstInstallation(void) const =0;
	
protected:
	virtual inline void					secureSave(void) =0;
	virtual inline void					secureLoad(void) =0;
	

};

