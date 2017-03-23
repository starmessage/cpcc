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
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
#pragma once

#error DO NOT INCLUDE THIS FILE

#include <map>
#include <string>
#include "cpccUnicodeSupport.h"

/*
	internal notes:
	1) to activation code periexei to onoma kai email tou pelati kai to version number tou programmatos.
	2) kalo tha itan na mporousa na to parago kai me excel
	3) mporo na stelno mia sympiesmeni morfi tou H na stelno [onoma] [email] [code] kai na ta bazei monos tou
	4) to license code prepei na periexei to machine ID anti gia to [program-version]
	see TmioanShareware_2000.cpp
		
*/



template <typename T>
class cParameterDict
{
private:
    std::map<int, T> m_ParameterList;
    
public:
    void setParameter(const int aKey, const T aValue) { m_ParameterList[aKey] = aValue;  }
    T getParameter(const int aKey, const T aDefaultValue) 
		{ return (m_ParameterList.find(aKey) != m_ParameterList.end())? m_ParameterList[aKey]: aDefaultValue; }

};


//////////////////////////////////////////////
//
//  class cpccSharewareAbstract
//
//////////////////////////////////////////////

class cpccSharewareAbstract: public cParameterDict<bool>, cParameterDict<int>, cParameterDict<cpcc_string>
{

public:		// ctors,  factory
	static cpccSharewareAbstract     *getInstancePtr(void);

    virtual ~cpccSharewareAbstract() {}
    
public:
	virtual inline bool                 importActivationCode(const cpcc_char * aLicenseCode) = 0;
	virtual inline long int             getDaysSinceFirstRun(void) const = 0;
	//virtual inline void                 saveEncryptedLicense(void) const = 0;
	//virtual inline void                 loadEncryptedLicense(void) = 0;
	virtual inline bool                 isRegistered(void) = 0;

	virtual inline const cpcc_string	getComputerID(void) const = 0;
	// virtual inline const bool			quickCheckActivationCode(const cpcc_char * aLicenseCode) = 0;
	virtual inline void					removeInstalledLicense(void) = 0;
	virtual inline const time_t			getTrustedCurrentDate(void) const = 0;

    /*
    using cParameterDict<bool>::getParameter;
    using cParameterDict<bool>::setParameter;
    using cParameterDict<int>::getParameter;
    using cParameterDict<int>::setParameter;
    */

    // convenience conversion from char* to std::string
    void setParameter(const int aKey, const cpcc_char * aValue) { setParameter(aKey, cpcc_string(aValue));  }
	cpcc_string  getParameter(const int aKey, const cpcc_char *aDefaultValue) { return getParameter(aKey, cpcc_string(aDefaultValue)); }

    template <typename T>
    void setParameter(const int aKey, const T &aValue) { cParameterDict<T>::setParameter(aKey, aValue);  }
    
	template <typename T>
    T  getParameter(const int aKey, const T &aDefaultValue) { return cParameterDict<T>::getParameter(aKey, aDefaultValue); }



};



/*
	examples of supported cases:

	Type1:
		- free trial with nag screens starting X days after installation
		- full version

*/

/*
class cpccLicenseElement
{
protected:

public:
	enum licenseElementType	{ undefined, freeTrial, freeTrialWithExpiry, fullVersion, fullVersionWithExpiry, licenseForUpgrades };
	

	// data ////////////////////////

	licenseElementType	m_licenseType;
	bool				m_hasExpiryDate;
	time_t				m_ExpiryDate =0, // time_t  holds the number of seconds since the Epoch
						m_dateIssued =0; 
	bool				m_hasUsageLimit;
	int					m_usageLimit,
						m_usageCount;

	// constructor //////////////////
	cpccLicenseElement() :
			m_licenseType(undefined), 
			m_hasExpiryDate(false),
			m_usageLimit(0),
			m_usageCount(0),
			m_hasUsageLimit(false) 
	{}

	// functions ////////////////////////

	const std::string	serialize(void) const
                        {	return cpcc_string("");
                        }
    
	void				deserialize(std::string &s) { };
	
	const int			getDaysUntilExpiry(const time_t atDate) const
						{	return (m_hasExpiryDate)? (int) ((atDate - m_ExpiryDate)/(60L*60L*24L)) : -1;
						}

	const bool			isLicenseActive(const time_t atDate) const
						{	if (m_licenseType == undefined) return false;
							if (m_hasExpiryDate && (atDate > m_ExpiryDate)) return false;
							if (m_hasUsageLimit && (m_usageCount > m_usageLimit)) return false;
							return true;
						}
};


/// shareware interface class
class cpcciShareware_v1
{
public:		// ctors
	cpcciShareware_v1() : m_userDetails("") { }


public:	
	
	inline const bool  					isLicenseActive(const cpccLicenseElement::licenseElementType aLicenseType, const time_t atDate)  const
											{ 
												for (unsigned int i = 0; i < m_licenseElements.size(); ++i)
													if (m_licenseElements[i].m_licenseType == aLicenseType)
														if (m_licenseElements[i].isLicenseActive(atDate)) return true;
												return false; 
											}

	virtual inline const int  			getRemainingDaysUntilExpiry(const cpccLicenseElement::licenseElementType aLicenseType, const time_t atDate) const
											{
												for (unsigned int i = 0; i < m_licenseElements.size(); ++i)
													if (m_licenseElements[i].m_licenseType == aLicenseType)
														return m_licenseElements[i].getDaysUntilExpiry(atDate);
												return -1;
											}

	virtual const char * 				getBuyUrl(void) const =0;
	virtual inline const cpcc_string	getComputerID(void) const =0;
	virtual inline const cpcc_string	getCustomerID(void) const = 0;
	virtual inline const cpcc_string	getLicenseDescription(void) const = 0;
	virtual inline const bool			importLicenseActivationCode(const cpcc_char * aLicenseCode) =0;
	virtual inline const bool			quickChecklLicenseActivationCode(const cpcc_char * aLicenseCode) = 0;
	virtual inline void					removeInstalledLicense(void) =0;
	virtual const long int				getDaysSinceFirstInstallation(void) const =0;
	virtual const time_t				getTrustedCurrentDate(void) const = 0;
	const int							getLicenseElements(void) const { return m_licenseElements.size(); }
protected:
	cpcc_string							m_userDetails;
	std::vector<cpccLicenseElement>		m_licenseElements;
	virtual inline void					saveEncryptedLicense(void) const = 0;
	virtual inline void					loadEncryptedLicense(void) = 0;
	
};

*/