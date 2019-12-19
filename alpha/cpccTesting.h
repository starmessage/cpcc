/*  *****************************************
 *  File:       cpccTesting.h
 *  *****************************************
 *  Library:    Cross Platform C++ Classes (cpcc)
 *  Copyright:  2019 StarMessage software.
 *  License:    Free for opensource projects.
 *              Commercial license exists for closed source projects.
 *  Web:        http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:   https://github.com/starmessage/cpcc
 *  email:    sales -at- starmessage.info
 *  *****************************************
 */

#ifndef cpccTesting_h
#define cpccTesting_h

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include "../fs.cpccUserFolders.h"

#ifdef    _WIN32
    #include <tchar.h>
    #include <windows.h>
	#include <Shlobj.h> // for SHGetFolderPath()
	#pragma comment(lib,"Shell32.lib") // instruct visual studio to link the library
#endif


#ifdef __APPLE__    // define the _T() macro that is a MS VC macro
    #ifndef _T
        #define _T(s) s
    #endif
    #ifndef TCHAR
        #define  TCHAR char
    #endif
#endif


#if defined(_DEBUG) || defined(DEBUG)
    #define ENABLE_cpccTESTING    1
#else
    #ifdef NDEBUG // in xcode, add NDEBUG in the preprocessor macros, under Release
        #define ENABLE_cpccTESTING    0
	#elif _WIN32
		#define ENABLE_cpccTESTING    0
    #endif
#endif

#ifndef ENABLE_cpccTESTING
    #error Not defined ENABLE_cpccTESTING. It must be 0 or 1.
#endif

#if (ENABLE_cpccTESTING==1)
    #define TestingVariableName aSelfTestObject
#else
    #define TestingVariableName
#endif

// #define MACRO_CONCAT_IMPL( x, y )    x##y
// #define MACRO_CONCAT( x, y )         MACRO_CONCAT_IMPL( x, y )

#define TEST_STR(value) #value
#define TEST_MAKESTRING(value) TEST_STR(value)


#define TEST_EXPECT(aCONDITION, aMESSAGE)		    \
  if (!(aCONDITION))							    \
    {                                               \
    aStream << _T("Test failed! (");                \
    aStream << TEST_MAKESTRING(aCONDITION);         \
    aStream << _T(")")  << std::endl;               \
    aStream << aMESSAGE << std::endl;               \
    }


#define TEST_ADDNOTE(aMESSAGE)   aStream << aMESSAGE << std::endl;


#define TEST_RUN(SelfTestUniqueName, aSTREAM)	    \
	namespace SelfTestUniqueName {			        \
        class cpccSelfTest          			    \
        {										    \
        public:									    \
            inline static void runTest(std::basic_ostream<TCHAR> &aStream);     \
            cpccSelfTest()						                                \
            {   const char* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);     \
                aSTREAM.get() << "/ Starting test:" << tmpNameA << std::endl;   \
                runTest(aSTREAM.get());                                         \
                aSTREAM.get() << "\\ Ending  test:" << tmpNameA << std::endl << std::endl; \
            };						                \
        } TestingVariableName;                      \
    }                                               \
                                                    \
    void SelfTestUniqueName::cpccSelfTest::runTest(std::basic_ostream<TCHAR> &aStream)


namespace cpccTesting
{
    
    
    
    /////////////////////////////////////
    // class util util
    /////////////////////////////////////
    
	class util
	{
	public:
        static std::basic_string<TCHAR> getTimeStamp(void);
	};

    
    
    /////////////////////////////////////
    // class util implementation
    /////////////////////////////////////
    
    inline std::basic_string<TCHAR> util::getTimeStamp(void)
    {
        std::time_t secondsSinceTheEpoch = std::time(nullptr);
		#pragma warning( suppress : 4996 )
        return std::asctime(std::localtime(&secondsSinceTheEpoch));
    }


    
    //=============================================================================
    
    
    /////////////////////////////////////
    // classes cOutputStreamAbstract
    /////////////////////////////////////
    
    class cOutputStreamAbstract
    {
    public:
        virtual std::basic_ostream<TCHAR> &get(void) = 0;
    };
    
    
    class cOutputStreamStdErr: public cOutputStreamAbstract
    {
    public:
        virtual std::basic_ostream<TCHAR> &get(void) override
        {
        #ifdef UNICODE
            return std::wcerr;
        #else
            return std::cerr;
        #endif
        }
    };
    
    
    class cOutputStreamDesktopFile: public cOutputStreamAbstract
    {
    private:
        std::basic_ofstream<TCHAR>  m_stream;
        std::basic_string<TCHAR>    m_fn;
        
    public:
        cOutputStreamDesktopFile()
        {
            m_fn = cpccUserFolders::getDesktop();
            m_fn.append(_T("/cpccTesting.txt"));
            
            m_stream.open(m_fn, 0);
            if (m_stream.good())
            {
                m_stream << util::getTimeStamp() << std::endl;
                m_stream.flush();
            }
        }
        
        virtual std::basic_ostream<TCHAR> &get(void) override
        {
            if (m_stream.good())
            {
                m_stream.flush();
                return m_stream;
            }
            
            #ifdef UNICODE
                return std::wcerr;
            #else
                return std::cerr;
            #endif
        }
        
        ~cOutputStreamDesktopFile()
        {
            get() << _T("Closing the file.") << std::endl;
        }
    };
    
    
    
    /////////////////////////////////////
    // class singleton
    /////////////////////////////////////
    
    class singleton
    {
    public:
        static cOutputStreamAbstract &getOutput(void)
        {
            static cOutputStreamDesktopFile inst_;
            return inst_;
        }
    };
    
    
    template <typename T>
    class cSingleton
    {

    public:
        static T& getInstance()
        {
            static T theinstance{};
            return theinstance;
        }

    protected:
        cSingleton() {}
        ~cSingleton() {}

    private:
        cSingleton(cSingleton const&);
        void operator=(cSingleton const&);
    };



    //=======================================================================
    
    struct sTestResult
    {
        bool pass;
        std::basic_string<TCHAR> message;
    };


    
    //=============================================================================
    
    
    class cTestRegister
    {
        public:
            bool runAllTests(std::basic_ostream<TCHAR> &os);
    };
    
    
} // end of namespace cpccTesting





#endif /* cpccTesting_h */
