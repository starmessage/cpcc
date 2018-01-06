/*  *****************************************
 *  File:		net.cpccURLbuilder.h
 *	Purpose:	Portable (cross-platform), light-weight, URL builder class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2017 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <string>

#include <map>
#include <sstream>

#include "cpccUnicodeSupport.h"

/*
 tutorial and examples:
 
 // split a full URL to postURL and postData
 std::string aURLwithParametersStr(aURLwithParameters);
 auto index = aURLwithParametersStr.find('?');
 if (index == std::string::npos)
 return 901; // no parameters found
 
 std::string strURL = aURLwithParametersStr.substr(0,index);
 std::string strParams = aURLwithParametersStr.substr(index+1);
 std::cout << "strURL=" << strURL << std::endl;
 std::cout << "strParams=" << strParams << std::endl;
 */


class cpccURLbuilder
{
private:
    
    typedef std::map<cpcc_string, cpcc_string> tParameters;
	cpcc_string m_connectionProtocol, // http, https, etc
				m_hostAddress,
				m_path;

    tParameters m_urlParamsList;
    
public:

    cpccURLbuilder(void)    { }

    cpccURLbuilder(const char *aConnectionProtocol, const char *aHost, const char *aPath):
			m_connectionProtocol(aConnectionProtocol ? aConnectionProtocol : ""),
			m_hostAddress(aHost?aHost:""),
			m_path(aPath ? aPath : "")
    { }
    

    cpccURLbuilder(cpccURLbuilder &another): 
			m_connectionProtocol(another.m_connectionProtocol),
			m_hostAddress(another.m_hostAddress), 
			m_path(another.m_path),
			m_urlParamsList (another.m_urlParamsList)
    { }
    
    cpccURLbuilder & operator = (cpccURLbuilder &another)
    {
        copyFrom(another);
        return *this;
    }
    
	bool isHttps(void) const
	{
        
#ifdef _WIN32
		#pragma warning( suppress : 4996 )
		return (strcmpi("https://", m_connectionProtocol.c_str()) == 0);
#else
        // Unix libraries use strcasecmp, from <strings.h>.
        return (strcasecmp("https://", m_connectionProtocol.c_str()) == 0);
#endif
	}

    void copyFrom(cpccURLbuilder &another)
    {
		m_connectionProtocol = another.m_connectionProtocol;
        m_hostAddress = another.m_hostAddress;
        m_path = another.m_path;
        m_urlParamsList = another.m_urlParamsList;
    }
    

    static std::string urlEncode(const char *s)
    {
        const std::string safechars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
        
        std::string encoded="";
        for(size_t i=0, len=strlen(s); i<len; ++i)
        {
            if (safechars.find_first_of(s[i]) != std::string::npos)
                encoded.push_back(s[i]);
            else
            {
                encoded.append("%");
                char buf[3];
				// #pragma warning( disable : 4996 )  
				#pragma warning( suppress : 4996 )  
                sprintf(buf, "%.2X", (unsigned char) s[i]); // for Multibyte string.
				// #pragma warning( default : 4996; )  
				encoded.append(buf);
            }
        }
        return encoded;
    }
    
      
    void clearParameters(void)
    {
        m_urlParamsList.clear();
    }
    
	/*
    const std::string getParameter(const char *aName) const
	{
	    if (!aName)
            return std::string("");
		return m_urlParamsList[aName];
	}
	*/
	
    void setParameter(const char *aName, const char *aValue)
    {
        if (!aName)
            return;
        
        if (aValue)
            m_urlParamsList[aName] = urlEncode(aValue);
        else
            m_urlParamsList[aName] = "";
    }
    
    
    void setParameter(const char *aName, const std::string &aValue)
    {
        setParameter(aName, aValue.c_str());
    }
    
    
    void setParameter(const char *aName, const int aValue)
    {
        std::ostringstream ss; ss << aValue;
        setParameter(aName, ss.str());
    }
    
    
    const std::string getFullURL(void)
    {
		std::string result(m_connectionProtocol);
		result +=  m_hostAddress + m_path + '?' + getUrlParameters();
        return result;
    }
    
    
    /// without the starting ?
    const std::string  getUrlParameters(void)
    {
		std::string result, glueChar;
        for (tParameters::iterator it = m_urlParamsList.begin() ; it != m_urlParamsList.end() ; ++it, glueChar='&')
			result += glueChar + it->first + "=" + it->second;
        
        return result;
    }

	void setConnectionProtocol(const char *aConnectionProtocol)
	{
		m_connectionProtocol = aConnectionProtocol ? aConnectionProtocol:"";
	}

    const char * getHost(void) const
    {
        return m_hostAddress.c_str();
    }

	const std::string getConnectionProtocolAndHost(void)
	{
		std::string result(m_connectionProtocol);
		result += m_hostAddress;
		return result;
	}

	const char * getPath(void) const
	{
		return m_path.c_str();
	}

	
};

 

