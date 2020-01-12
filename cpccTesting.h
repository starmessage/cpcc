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
#include <thread>
#include <vector>
#include "types.cpccWideCharSupport.h"

#include "fs.cpccUserFolders.h"

#ifdef    _WIN32
    #include <windows.h>
	// #include <Shlobj.h> // for SHGetFolderPath()
	// #pragma comment(lib,"Shell32.lib") // instruct visual studio to link the library
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

    /*
    struct sTestResult
    {
        bool pass;
        std::basic_string<TCHAR> message;
    };
    */


    //=============================================================================
    // class cThreadRegister
    // used to join all (if any) running threads
    //=============================================================================
    class cThreadRegister
    {
    public:
        typedef std::vector<std::thread *> tListOfThreads;
        
        static tListOfThreads &getListOfThreads(void)
        {
            static tListOfThreads list;
            return list;
        }
        
        
        static bool addThread(std::thread *aThread)
        {
            if (!aThread)
                return false;
            
            static std::mutex mu;
            std::lock_guard<std::mutex> lock(mu);
            getListOfThreads().push_back(aThread);
            return true;
        }
        
        static void joinAllThreads(std::basic_ostream<TCHAR> &errStream)
        {
            for (auto &th : getListOfThreads())
            {
                if (th)
                {
                    if (th->joinable())
                        th->join();
                    else
                        errStream << _T("#8261a: joinAllThreads found a non-joinable thread\n");
                }
                else
                    errStream << _T("#8261b: joinAllThreads found a null thread ptr\n");
            }
        }
    };

    //=============================================================================
    // class sharedTestRegister
    // used to avoid runing the same test again
    //=============================================================================

    class sharedTestRegister
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
        
        ~cOutputStreamDesktopFile();
    };

    // ///////////////////////////////////
    // class sharedObjects has singletons
    // of the needed objects
    // ///////////////////////////////////

    class sharedObjects
    {
    public:
        static cOutputStreamAbstract &outputStream(void)
        {
            static cOutputStreamDesktopFile inst_;
            return inst_;
        }
    };



    inline cOutputStreamDesktopFile::~cOutputStreamDesktopFile()
    {
        get() << _T("Closing the file.") << std::endl;
        int errors = cpccTesting::sharedTestRegister::getNErrors();
        
        if (errors>0)
            get() << _T("FOUND ") << errors << _T(" ERRORS.") << std::endl;
        else
            get() << _T("All tests passed OK.") << std::endl;

        if (m_stream.good())
            m_stream.close();
    }


    class cTestRunner
    {
    private:
        const std::basic_string<TCHAR> testName;
        std::basic_ostream<TCHAR> &reportStream;
        std::thread *threadPtr = NULL;
        
    public:
        cTestRunner(const TCHAR *aTestName, std::basic_ostream<TCHAR> &aReportStream, const bool runAsync):
            testName(aTestName?aTestName:_T("#5231: NULL test name")),
            reportStream(aReportStream)
        {
            if (cpccTesting::sharedTestRegister::testHasAlreadyRan(testName.c_str()))
                return;
            if (!runAsync)
                runFunctionWrapper();
            else
            {
                reportStream << _T("Will run async (not implemented)\n");
                // threadPtr = new std::thread(&cTestRunner::runFunctionWrapper, *this );
            }
        }
        
        ~cTestRunner()
        {
            if (threadPtr)
            {
                if (threadPtr->joinable())
                    threadPtr->join();
                else
                    reportStream << _T("#8261b: ~cTestRunner() found a non-joinable thread\n");
            }
        }
        
        // this will be defined by the macro, in a derived class
        void runTest(std::basic_ostream<TCHAR> &aStream);
        
        void runFunctionWrapper(void)
        {
            reportStream << _T("/ Starting test:") << testName << std::endl;
            runTest(reportStream);
            reportStream << _T("\\ Ending   test:") << testName << std::endl << std::endl;
            int errors = cpccTesting::sharedTestRegister::getNErrors();
            if (errors > 0)
                reportStream << errors << _T(" errors so far.") << std::endl;
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
    cpccTesting::sharedTestRegister::getNErrors() = cpccTesting::sharedTestRegister::getNErrors()+1; \
    }

#define OUTPUT_STREAM           cpccTesting::sharedObjects::outputStream().get()

#define TEST_ADDNOTE(aMESSAGE)   OUTPUT_STREAM << aMESSAGE << std::endl;

// Notes: use a unnamed namespace so that the variable TestingVariableName is not redefined multiple times
// Alternative to the unnamed namespaces is the the 'static' declaration of the variable.
// Here, it helps to avoid the multiple symbols definition when the header is included (compiled) from many cpp files.
#define TEST_RUN(SelfTestUniqueName)	            \
	namespace SelfTestUniqueName {			        \
        class cpccSelfTest          			    \
        {										    \
        public:									    \
            void runTest(std::basic_ostream<TCHAR> &aStream);     \
                                                                            \
            void runFunctionWrapper(void)                                   \
            {   const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);        \
                OUTPUT_STREAM << _T("/ Starting test:") << tmpNameA << std::endl;   \
                runTest(OUTPUT_STREAM);                                         \
                OUTPUT_STREAM << _T("\\ Ending   test:") << tmpNameA << std::endl << std::endl; \
                int errors = cpccTesting::sharedTestRegister::getNErrors();          \
                if (errors > 0)                                                 \
                    OUTPUT_STREAM << errors << _T(" errors so far.") << std::endl;  \
            }                                                                       \
                                                                                    \
            cpccSelfTest()						                                    \
            {   const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);        \
                if (cpccTesting::sharedTestRegister::testHasAlreadyRan(tmpNameA))   \
                    return;                                                         \
                runFunctionWrapper();                                               \
            }						                                                \
                                                                                    \
        }; static cpccSelfTest  TestingVariableName;                            \
    }                                                                           \
                                                                                \
    inline void SelfTestUniqueName::cpccSelfTest::runTest(std::basic_ostream<TCHAR> &aStream)


/* the async version of the test that runs it on a separate thread
   after a few msec to give time to the app to launch all modules */
#define TEST_RUN_ASYNC(SelfTestUniqueName, aSTREAM)        \
        TEST_RUN_IMPL(SelfTestUniqueName, aSTREAM, true)



#define TEST_RUN_WITHCLASS(SelfTestUniqueName, aSTREAM)        \
namespace SelfTestUniqueName {                    \
    static cpccTesting::cTestRunner  TestingVariableName( TEST_MAKESTRING(SelfTestUniqueName), aSTREAM.get(), false);   \
}                                                                           \
                                                                            \
inline void SelfTestUniqueName::cTestRunner::runTest(std::basic_ostream<TCHAR> &aStream)




#define TEST_RUN_xxxx(SelfTestUniqueName, aSTREAM)        \
    namespace SelfTestUniqueName {                    \
        class cTestRunnerImpl: protected cpccTesting::cTestRunner            \
        {                                            \
        public:                                        \
            cTestRunnerImpl(void): \
                cTestRunner( TEST_MAKESTRING(SelfTestUniqueName), aSTREAM.get(), false)    \
            { }                                         \
            void runTest(std::basic_ostream<TCHAR> &aStream) override;     \
        }; static cTestRunnerImpl  TestingVariableName;   \
    }                                                                           \
                                                                                \
    inline void SelfTestUniqueName::cTestRunnerImpl::runTest(std::basic_ostream<TCHAR> &aStream)




#endif /* cpccTesting_h */
