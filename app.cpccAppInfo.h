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
		cpcc_string result("<a href=\"");
		result.append(url);
		result.append("\">");
		result.append(label);
		result.append("</a>");
		return result;
	}

public:
	enum xtraInfo { includeSalesEmail=1, includeSupportEmail=2, includeWebsite=4, useHtmlLinks=8 };
    enum licenseType { eltFree, eltFreeTrialPeriod, eltFull, eltSpecialEdition };

	const static cpcc_char
		*CompanyName,
		*ProgramName,
        *Version,
		*Build,
		*WebSiteNoHttp,
		*WebSite;
#ifdef _WIN32
	const static WCHAR
		*DonateURL_W;
#endif
	const static char
		*BuyURL,
        *DonateURL,
		*CheckForUpdatesURL,
		*EmailSales,
		*EmailSupport;
    const static int
        licenseType;

    
    
	static cpcc_string getText_AboutThisSoftware(const int xi, const char *aLineBreak, const char *aLicenseInfo, const char *aCallToAction)
	{
		std::string infoText(cpccAppInfo::ProgramName);
        infoText += " ";
        infoText += cpccAppInfo::Version;
        
		if (!aLineBreak)
		{ 
			infoText += "#8572: const char *aLineBreak was null\n";
			return infoText;
		}

		const bool htmlLinks = ((xi & useHtmlLinks)!=0);

		if (aLicenseInfo)
			if (strlen(aLicenseInfo) > 0)
			{
				infoText.append(aLineBreak);
				infoText.append("License: ");
				infoText.append(aLicenseInfo);
			}

		infoText.append(aLineBreak);
		infoText.append(aLineBreak);
		
		infoText.append("Build: ");
		infoText.append(Build);
		infoText.append(aLineBreak); 
		infoText.append("(c) ");
		infoText.append(cpccAppInfo::CompanyName);

		if ((strlen(WebSiteNoHttp)>0) && (xi & includeWebsite))
		{
			infoText.append(aLineBreak);
			infoText.append("Website: ");
			if (htmlLinks)
				infoText.append(createLink(WebSiteNoHttp, WebSite));
			else
				infoText.append(WebSiteNoHttp);
									
		}
		
		if ((strlen(EmailSales) > 0) && (xi & includeSalesEmail))
		{
			infoText.append(aLineBreak);
			infoText.append("Email (sales): ");
			infoText.append(EmailSales);
		}

		if ((strlen(EmailSales) > 0) && (xi & includeSupportEmail))
		{
			infoText.append(aLineBreak);
			infoText.append("Email (support): ");
			infoText.append(EmailSupport);
		}

		
		if (aCallToAction)
			if (strlen(aCallToAction) > 0)
			{
				infoText.append(aLineBreak);
				infoText.append(aLineBreak);
				infoText.append(aCallToAction);
			}

		return infoText;
	}
    


};