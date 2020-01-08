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

/* 
    
    Saves a file on the desktop containing the test results.
    No dependencies on other libraries and frameworks.
    On MacOS sandboxed applications the desktop folder is redirected by the OS under the App folders.

*/


#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include "fs.cpccUserFolders.h"

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
    #else
        #define ENABLE_cpccTESTING    1
    #endif
#endif

#ifndef ENABLE_cpccTESTING
    #error Not defined ENABLE_cpccTESTING. It must be 0 or 1.
#endif

#define TEST_STR(value) #value
#define TEST_TSTR(value) _T( TEST_STR(value) )
#define TEST_MAKESTRING(value) TEST_TSTR(value)


// #define TEST_MACRO_CONCAT_IMPL( x, y )    x##y
// #define TEST_MACRO_CONCAT( x, y )         TEST_MACRO_CONCAT_IMPL( x, y )

// note: __COUNTER__ gets reset for each cpp file

#if (ENABLE_cpccTESTING==1)
    #define TestingVariableName aSelfTestObject
#else
    #define TestingVariableName
#endif


namespace cpccTesting
{

    //=============================================================================
    // class cTestRegister
    // used to avoid runing the same test again
    //=============================================================================

    class cTestRegister
    {
    public:
        static int  &getNErrors(void)
        {
            static int nErrors = 0;
            return nErrors;
        }

        static bool testHasAlreadyRan(const TCHAR* aTestName)
        {
            if (!aTestName)
                return false;

            static std::vector<std::basic_string<TCHAR>> list;
            if (std::find(list.begin(), list.end(), aTestName) != list.end())
                return true;

            list.push_back(aTestName);
            return false;
        }
    };
} // end of namespace (the same namespace continues below)



#define TEST_EXPECT(aCONDITION, aMESSAGE)		    \
  if (!(aCONDITION))							    \
    {                                               \
    aStream << _T("Test failed! (");                \
    aStream << TEST_MAKESTRING(aCONDITION);         \
    aStream << _T(")")  << std::endl;               \
    aStream << aMESSAGE << std::endl;               \
    cpccTesting::cTestRegister::getNErrors() = cpccTesting::cTestRegister::getNErrors()+1; \
    }


#define TEST_ADDNOTE(aMESSAGE)   aStream << aMESSAGE << std::endl;

// Notes: use a unnamed namespace so that the variable TestingVariableName is not redefined multiple times
// Alternative to the unnamed namespaces is the the 'static' declaration of the variable.
// Here, it helps to avoid the multiple symbols definition when the header is included (compiled) from many cpp files.
#define TEST_RUN(SelfTestUniqueName, aSTREAM)	    \
	namespace SelfTestUniqueName {			        \
        class cpccSelfTest          			    \
        {										    \
        public:									    \
            static void runTest(std::basic_ostream<TCHAR> &aStream);     \
            cpccSelfTest()						                                \
            {   const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);     \
                if (cpccTesting::cTestRegister::testHasAlreadyRan(tmpNameA))    \
                    return;                                                     \
                aSTREAM.get() << _T("/ Starting test:") << tmpNameA << std::endl;   \
                runTest(aSTREAM.get());                                         \
                aSTREAM.get() << _T("\\ Ending   test:") << tmpNameA << std::endl << std::endl; \
                int errors = cpccTesting::cTestRegister::getNErrors();          \
                if (errors > 0)                                                 \
                    aSTREAM.get() << errors << _T(" errors so far.") << std::endl;  \
            };						                                            \
        }; static cpccSelfTest  TestingVariableName;                            \
    }                                                                           \
                                                                                \
    inline void SelfTestUniqueName::cpccSelfTest::runTest(std::basic_ostream<TCHAR> &aStream)



namespace cpccTesting
{
    
    // ///////////////////////////////////
    // class util
    // ///////////////////////////////////
    
	class util
	{
	public:
        static std::basic_string<TCHAR> getTimeStamp(void)
        {
            std::time_t secondsSinceTheEpoch = std::time(nullptr);
            
            #ifdef UNICODE
                #pragma warning( suppress : 4996 )
                return _wasctime(std::localtime(&secondsSinceTheEpoch));
            #else
                #pragma warning( suppress : 4996 )
                return std::asctime(std::localtime(&secondsSinceTheEpoch));
            #endif
        }
	};

        
    
    // ///////////////////////////////////
    // classes cOutputStreamAbstract
    // ///////////////////////////////////
    
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
                m_stream << _T("Unit testing by the cpcc library.") << std::endl;
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
            int errors = cpccTesting::cTestRegister::getNErrors();
            
            if (errors>0)
                get() << _T("FOUND ") << errors << _T(" ERRORS.") << std::endl;
            else
                get() << _T("All tests were passed OK.") << std::endl;

            if (m_stream.good())
                m_stream.close();
        }
    };
    
    

    // ///////////////////////////////////
    // class singleton
    // ///////////////////////////////////
    
    class singleton
    {
    public:
        static cOutputStreamAbstract &getOutput(void)
        {
            static cOutputStreamDesktopFile inst_;
            return inst_;
        }
    };
    

    //=======================================================================
    
    /*
    struct sTestResult
    {
        bool pass;
        std::basic_string<TCHAR> message;
    };
    */

    
    
} // end of namespace cpccTesting





#endif /* cpccTesting_h */
