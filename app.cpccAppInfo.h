/*  *****************************************
 *  File:		app.cpccAppInfo.h
  *	Purpose:	Portable (cross-platform), light-weight, library
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
#include <string.h>
#include "cpccUnicodeSupport.h"
#include "core.cpccOS.h"

/* you need to allocate these global variables somewhere in one of your cpp files like this:

	const cpcc_char *cpccAppInfo::CompanyName			= "StarMessage software";
	const cpcc_char *cpccAppInfo::ProgramName			= "StarMessage screensaver";
	const cpcc_char *cpccAppInfo::Version				= "5.0";
	const cpcc_char *cpccAppInfo::ProgramNameAndVersion = "StarMessage screensaver vX.X";
	const cpcc_char *cpccAppInfo::WebSiteNoHttp			= "www.StarMessageSoftware.com";
	const cpcc_char *cpccAppInfo::WebSite				= "www.StarMessageSoftware.com";
	const char		*cpccAppInfo::EmailSales			= "xxx@ccc.aaa";
	const char		*cpccAppInfo::EmailSupport			= "yyy@ccc.aaa";

*/

class cpccAppInfo
{
private:
	static cpcc_string createLink(const cpcc_char *label, const cpcc_char *url)
	{
		cpcc_string result(_T("<a href=\""));
		result.append(url);
		result.append(_T("\">"));
		result.append(label);
		result.append(_T("</a>"));
		return result;
	}

public:
	enum xtraInfo { includeEmail=1, includeWebsite=4, useHtmlLinks=8 };
    enum licenseType { eltFree, eltFreeInAppleStore, eltPaidInAppleStore, eltFreeTrialPeriod, eltFull, eltSpecialEdition, eltCommercial };

	const static cpcc_char
				*CompanyName,
				*ProgramName,
				*Version,
				*Build,
				*MacBundleId;   // ignored under Windows
 
	const static char 
				*VersionA,
				*BuildA;
	const static cpcc_char
				*WebSiteNoHttp,
				*WebSite,
                *WebPrivacyPolicy,
				*BuyURL,
				*DonateURL,
				*CheckForUpdatesURL,
				*Email;
    const static int licenseType;

    
	static cpcc_string getText_pleaseBuyTheFullVersion_html(const cpcc_char *aProgramName)
	{
		cpcc_string tmp_callToAction = _T("If you like ");
		tmp_callToAction.append(aProgramName);
		tmp_callToAction.append(", please <A HREF=\"");
		tmp_callToAction.append(cpccAppInfo::BuyURL);
		tmp_callToAction.append(_T("\">buy now</a> the full version."));
		return tmp_callToAction;
	}

    
	static cpcc_string getText_AboutThisSoftware(const int xi, const cpcc_char *aLineBreak, const cpcc_char *aLicenseInfo, const cpcc_char *aCallToAction)
	{
		cpcc_string infoText(cpccAppInfo::ProgramName);
        infoText += _T(" ");
        infoText += cpccAppInfo::Version;
        
		if (!aLineBreak)
		{ 
			infoText += _T("#8572: const char *aLineBreak was null\n");
			return infoText;
		}

		const bool htmlLinks = ((xi & useHtmlLinks)!=0);

		if (aLicenseInfo)
			if (cpcc_strlen(aLicenseInfo) > 0)
			{
				infoText.append(aLineBreak);
				infoText.append(_T("License: "));
				infoText.append(aLicenseInfo);
			}

		infoText.append(aLineBreak);
		infoText.append(aLineBreak);
		
		infoText.append(_T("Build: "));
		infoText.append(Build);
		infoText.append(aLineBreak); 
		infoText.append(_T("(c) "));
		infoText.append(cpccAppInfo::CompanyName);

		if ((cpcc_strlen(WebSiteNoHttp)>0) && (xi & includeWebsite))
		{
			infoText.append(aLineBreak);
			infoText.append(_T("Website: "));
			if (htmlLinks)
				infoText.append(createLink(WebSiteNoHttp, WebSite));
			else
				infoText.append(WebSiteNoHttp);
									
		}
		
		if ((cpcc_strlen(Email) > 0) && (xi & includeEmail))
		{
			infoText.append(aLineBreak);
			infoText.append(_T("Email: "));
			if (htmlLinks)
				infoText.append(createLink(Email, (cpcc_string(_T("mailto:")) + Email).c_str()));
			else
				infoText.append(Email);
			
		}

				
		if (aCallToAction)
			if (cpcc_strlen(aCallToAction) > 0)
			{
				infoText.append(aLineBreak);
				infoText.append(aLineBreak);
				infoText.append(aCallToAction);
			}

		return infoText;
	}
    


};