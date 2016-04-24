﻿/*  *****************************************
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
public:
	enum xtraInfo { includeSalesEmail=1, includeSupportEmail=2, includeWebsite=4 };
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

    
    
	static cpcc_string getText_AboutThisSoftware(const int xi =0)
	{
		static std::string infoText(cpccAppInfo::ProgramName);
        infoText += " ";
        infoText += cpccAppInfo::Version;
        
		infoText.append("\n(c) ");
		infoText.append(cpccAppInfo::CompanyName);

		if ((strlen(WebSiteNoHttp)>0) && (xi & includeWebsite))
		{
			infoText.append("\n\nWebsite: ");
			infoText.append(WebSiteNoHttp);
		}
		
		if ((strlen(EmailSales) > 0) && (xi & includeSalesEmail))
		{
			infoText.append("\nEmail (sales): ");
			infoText.append(EmailSales);
		}

		if ((strlen(EmailSales) > 0) && (xi & includeSupportEmail))
		{
			infoText.append("\nEmail (support): ");
			infoText.append(EmailSupport);
		}

		infoText.append("\nBuild: ");
		infoText.append( Build );

		return infoText;
	}
    


};