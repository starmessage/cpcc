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

    cpccURLbuilder(const cpcc_char *aConnectionProtocol, const cpcc_char *aHost, const cpcc_char *aPath):
			m_connectionProtocol(aConnectionProtocol ? aConnectionProtocol : _T("")),
			m_hostAddress(aHost?aHost: _T("")),
			m_path(aPath ? aPath : _T(""))
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
		return (cpcc_stricmp(_T("https://"), m_connectionProtocol.c_str()) == 0);
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
    

    static cpcc_string urlEncode(const cpcc_char *s)
    {
        const cpcc_string safechars = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~");
        
		cpcc_string encoded= _T("");
        for(size_t i=0, len=cpcc_strlen(s); i<len; ++i)
        {
            if (safechars.find_first_of(s[i]) != cpcc_string::npos)
                encoded.push_back(s[i]);
            else
            {
                encoded.append(_T("%"));
				cpcc_char buf[3];
				// #pragma warning( disable : 4996 )  
				#pragma warning( suppress : 4996 )  
				cpcc_sprintf(buf, _T("%.2X"), (unsigned char) s[i]); // for Multibyte string.
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
	
    void setParameter(const cpcc_char *aName, const cpcc_char *aValue)
    {
        if (!aName)
            return;
        
        if (aValue)
            m_urlParamsList[aName] = urlEncode(aValue);
        else
            m_urlParamsList[aName] = _T("");
    }
    
    
    void setParameter(const cpcc_char *aName, const cpcc_string &aValue)
    {
        setParameter(aName, aValue.c_str());
    }
    
    
    void setParameter(const cpcc_char *aName, const int aValue)
    {
		cpcc_ostringstream ss; ss << aValue;
        setParameter(aName, ss.str());
    }
    
    
    const cpcc_string getFullURL(void)
    {
		cpcc_string result(m_connectionProtocol);
		result +=  m_hostAddress + m_path + _T("?") + getUrlParameters();
        return result;
    }
    
    
    /// without the starting ?
    const cpcc_string  getUrlParameters(void)
    {
		cpcc_string result, glueChar;
        for (tParameters::iterator it = m_urlParamsList.begin() ; it != m_urlParamsList.end() ; ++it, glueChar= _T("&"))
			result += glueChar + it->first + _T("=") + it->second;
        
        return result;
    }

	void setConnectionProtocol(const cpcc_char *aConnectionProtocol)
	{
		m_connectionProtocol = aConnectionProtocol ? aConnectionProtocol: _T("");
	}

    const cpcc_char * getHost(void) const
    {
        return m_hostAddress.c_str();
    }

	const cpcc_string getConnectionProtocolAndHost(void)
	{
		cpcc_string result(m_connectionProtocol);
		result += m_hostAddress;
		return result;
	}

	const cpcc_char * getPath(void) const
	{
		return m_path.c_str();
	}

	
};

 

