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
    
    typedef std::map<std::string, std::string> tParameters;
    std::string m_host,
				m_path,
                m_fullURL,
                m_UrlParametersCache;
    tParameters m_urlParamsList;
    
public:
    cpccURLbuilder(void)
    { }

    cpccURLbuilder(const char *aHost, const char *aPath): 
			m_host(aHost?aHost:""),
			m_path(aPath ? aPath : "")
    { }
    
    cpccURLbuilder(cpccURLbuilder &another): 
			m_host(another.m_host), 
			m_path(another.m_path),
			m_urlParamsList (another.m_urlParamsList)
    { }
    
    cpccURLbuilder & operator = (cpccURLbuilder &another)
    {
        copyFrom(another);
        return *this;
    }
    
    void copyFrom(cpccURLbuilder &another)
    {
        m_host = another.m_host;
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
    
    
    /*
    void setServer(const char *aServer)
    {
        if (aServer)
            m_server = aServer;
        else
            m_server = "";
    }
    */
    
    void clearParameters(void)
    {
        m_urlParamsList.clear();
    }
    
    
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
    
    
    const char * getFullURL(void)
    {
        m_fullURL = m_host + m_path + '?' + getUrlParameters();
        return m_fullURL.c_str();
    }
    
    
    /// without the starting ?
    const char * getUrlParameters(void)
    {
        m_UrlParametersCache = "";
        std::string glueChar="";
        for (tParameters::iterator it = m_urlParamsList.begin() ; it != m_urlParamsList.end() ; ++it, glueChar='&')
            m_UrlParametersCache += glueChar + it->first + "=" + it->second;
        
        return m_UrlParametersCache.c_str();
    }
    
    
    const char * getHost(void) const
    {
        return m_host.c_str();
    }

	const char * getPath(void) const
	{
		return m_path.c_str();
	}
};

 

