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

#pragma once

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
#include <chrono>
#include <vector>
#include <locale>
#include <codecvt>
#include <mutex>
#include <cstdlib>
#include "data.cpccWideCharSupport.h"
// #include "io.cpccFileSystemMini.h"


#ifdef    _WIN32
    #include <windows.h>
	// #include <Shlobj.h> // for SHGetFolderPath()
	// #pragma comment(lib,"Shell32.lib") // instruct visual studio to link the library
#endif

#if __APPLE__
    #include "TargetConditionals.h"
#endif


#if defined(_DEBUG) || defined(DEBUG) // Under xcode put DEBUG=1 in the debug configuration
    #define ENABLE_cpccTESTING    1
#else
    #ifdef NDEBUG // in xcode, add NDEBUG in the preprocessor macros, under Release
        #define ENABLE_cpccTESTING    0
	#elif _WIN32
		#define ENABLE_cpccTESTING    0
    #else
        #define ENABLE_cpccTESTING    0     // MacOS release
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
    #define INSTANTIATE_TEST_OBJECT     static cpccSelfTest  aSelfTestObject
    // #define TestingVariableName aSelfTestObject
#else
    
    #define INSTANTIATE_TEST_OBJECT
    // #define TestingVariableName
#endif


namespace cpccTesting
{

    class config
    {
    public:
        // where the file will be written
        static std::basic_string<TCHAR>  getReportFilename(void);
        
        // the name of the app. It will be written inside the report file
        static std::basic_string<TCHAR> getAppName(void);
        
    };

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
    /* unused class 
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

    */


    // =============================================================================
    // class sharedTestRegister
    // used to avoid runing the same test again
    // =============================================================================

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
            static std::vector<std::basic_string<TCHAR>> list;
            static std::mutex mu;

            if (!aTestName)
                return false;

            // mutex here
            std::lock_guard<std::mutex> lock(mu);

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
            
            TCHAR* result = 0;
             #ifdef UNICODE
                #pragma warning( suppress : 4996 )
                result = _wasctime(std::localtime(&secondsSinceTheEpoch));
             #else
                #pragma warning( suppress : 4996 )
                result = std::asctime(std::localtime(&secondsSinceTheEpoch));
             #endif
            
            if (!result) return _T("");
            return result;
        }
        
        
        static void shellOpenFile(const TCHAR *filename)
        {
         #if TARGET_OS_IPHONE != 1
            if (system(NULL)) // If command is a null pointer, the function only checks whether a command processor is available through this function, without invoking any command.
            {
               #ifdef _WIN32
                    std::basic_string<TCHAR> theCommand(_T("start "));
                #else
                    std::basic_string<TCHAR> theCommand("open -e ");
                #endif
               theCommand += filename;
                
                #ifdef UNICODE
                    _wsystem(theCommand.c_str());
                #else
                    system(theCommand.c_str());
                #endif
            }
         #endif
        }
        
    };

        

    // ///////////////////////////////////
    // classes cOutputStreamAbstract
    // ///////////////////////////////////

    class cOutputStreamAbstract
    {
    public:
        std::mutex  m_writeMutex;
        
        virtual ~cOutputStreamAbstract() { }
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
            // m_fn = cpccUserFolders::getDesktop();
            // m_fn.append(_T("/cpccTesting.txt"));
            m_fn = config::getReportFilename();
            
            m_stream.open(m_fn, 0);
            if (m_stream.good())
            {
                std::lock_guard<std::mutex> lock(m_writeMutex);
                std::locale my_utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
                std::locale loc = m_stream.imbue(my_utf8_locale); // loc is used just to suppress warning C26444

                m_stream << _T("Unit testing by the cpcc library.") << std::endl;
                // m_stream << _T("Host application:") <<  cpccFileSystemMini::getAppFullPathFilename() << std::endl;
                m_stream << _T("Host application:") <<  config::getAppName() << std::endl;
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
        
        virtual ~cOutputStreamDesktopFile();
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
        
        
        static std::mutex &waitForExecutionTimeSlot(void)
        {
            static std::mutex mu;
            return mu;
        }
    };


    inline
    cOutputStreamDesktopFile::~cOutputStreamDesktopFile()
    {
        {
            std::lock_guard<std::mutex> lock(m_writeMutex);
            
            get() << _T("Closing the file.") << std::endl;
            int errors = cpccTesting::sharedTestRegister::getNErrors();
            
            if (errors>0)
            {
                get() << _T("FOUND ") << errors << _T(" ERRORS.") << std::endl;
                util::shellOpenFile(m_fn.c_str());
            }
            else
                get() << _T("All tests passed OK.") << std::endl;
        }
        
        if (m_stream.good())
            m_stream.close();
    }


} // end of namespace (the same namespace continues below)

#define OUTPUT_STREAM           cpccTesting::sharedObjects::outputStream().get()


// For the incorrect warning, Looks like C26110 can't recognize a mutex via a reference.
// Here is a workarround: https://developercommunity.visualstudio.com/content/problem/670717/incorrect-lock-warnings-by-analyzer-c26110.html

#define TEST_EXPECT(aCONDITION, aMESSAGE)		    \
  if (!(aCONDITION))							    \
    {                                               \
    auto& refToMutex = cpccTesting::sharedObjects::outputStream().m_writeMutex; \
    std::lock_guard<std::mutex> lock(refToMutex);           \
    OUTPUT_STREAM << _T("Test failed! (");                \
    OUTPUT_STREAM << TEST_MAKESTRING(aCONDITION);         \
    OUTPUT_STREAM << _T(")")  << std::endl;               \
    OUTPUT_STREAM << aMESSAGE << std::endl;               \
    cpccTesting::sharedTestRegister::getNErrors() = cpccTesting::sharedTestRegister::getNErrors()+1; \
    }


#define TEST_EXPECT_EQUAL(aVal1, aVal2, aMESSAGE)       \
if ((aVal1) != (aVal2))                                 \
    {                                                    \
    OUTPUT_STREAM << _T("Test failed! (");                \
    OUTPUT_STREAM << TEST_MAKESTRING(aVal1);              \
    OUTPUT_STREAM << _T(",");                               \
    OUTPUT_STREAM << TEST_MAKESTRING(aVal2);              \
    OUTPUT_STREAM << _T(")")  << std::endl;               \
    OUTPUT_STREAM << aMESSAGE << std::endl;               \
    OUTPUT_STREAM << aVal1;                               \
    OUTPUT_STREAM << _T(" NOT EQUAL TO ");                 \
    OUTPUT_STREAM << aVal2 << std::endl;                   \
    cpccTesting::sharedTestRegister::getNErrors() = cpccTesting::sharedTestRegister::getNErrors()+1; \
    }



#define TEST_ADDNOTE(aMESSAGE)   OUTPUT_STREAM << aMESSAGE << std::endl;

// Notes: use a unnamed namespace so that the variable TestingVariableName is not redefined multiple times
// Alternative to the unnamed namespaces is the the 'static' declaration of the variable.
// Here, it helps to avoid the multiple symbols definition when the header is included (compiled) from many cpp files.
#define TEST_RUN(SelfTestUniqueName)                \
	namespace SelfTestUniqueName {	                 \
        class cpccSelfTest          			    \
        {										    \
        public:									    \
            void runTest(void);                     \
                                                                            \
            void runFunctionWrapper(void)                                   \
            {   auto& refToMutex = cpccTesting::sharedObjects::waitForExecutionTimeSlot(); \
                std::lock_guard<std::mutex> lock(refToMutex);           \
                const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);        \
                if (cpccTesting::sharedTestRegister::testHasAlreadyRan(tmpNameA))   \
                    return;                                                         \
                OUTPUT_STREAM << _T("/ Starting test:") << tmpNameA << std::endl;   \
                runTest();                                         \
                OUTPUT_STREAM << _T("\\ Ending   test:") << tmpNameA << std::endl << std::endl; \
                int errors = cpccTesting::sharedTestRegister::getNErrors();          \
                if (errors > 0)                                                 \
                    OUTPUT_STREAM << errors << _T(" error(s) so far.") << std::endl;  \
            }                                                                       \
                                                                                    \
            cpccSelfTest()						                                    \
            {   runFunctionWrapper();                                              \
            }						                                                \
                                                                                    \
        }; INSTANTIATE_TEST_OBJECT;                                                 \
    }                                                                           \
                                                                                \
    inline void SelfTestUniqueName::cpccSelfTest::runTest(void)


/* the async version of the test that runs it on a separate thread
   after a few msec to give time to the app to launch all modules */
#define TEST_RUN_ASYNC(SelfTestUniqueName)                                      \
        namespace SelfTestUniqueName {                                          \
            class cpccSelfTest                                                  \
            {                                                                   \
            private:                                                            \
                std::thread *threadPtr = NULL;                                  \
            public:                                                             \
                void runTest(void);                                             \
                                                                                \
                void runFunctionWrapper(void)                                   \
                {   const int msec = 800 + (std::rand() % 1200);                \
                    std::this_thread::sleep_for(std::chrono::milliseconds(msec));               \
                    auto& refToMutex = cpccTesting::sharedObjects::waitForExecutionTimeSlot(); \
                    std::lock_guard<std::mutex> lock(refToMutex);           \
                    const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);        \
                    if (cpccTesting::sharedTestRegister::testHasAlreadyRan(tmpNameA))   \
                        return;                                                         \
                    OUTPUT_STREAM << _T("/ Starting Async test (in thread):") << tmpNameA << std::endl;   \
                    runTest();                                         \
                    OUTPUT_STREAM << _T("\\ Ending   Async test (in thread):") << tmpNameA << std::endl << std::endl; \
                    int errors = cpccTesting::sharedTestRegister::getNErrors();          \
                    if (errors > 0)                                                 \
                        OUTPUT_STREAM << errors << _T(" error(s) so far.") << std::endl;  \
                }                                                                       \
                                                                                        \
                cpccSelfTest()                                                            \
                {  threadPtr = new std::thread(&cpccSelfTest::runFunctionWrapper, *this );   \
                }                                                                        \
                                                                                        \
                ~cpccSelfTest()                                                            \
                {    if (threadPtr)                                                         \
                    {                                                                   \
                    if (threadPtr->joinable())                                          \
                        threadPtr->join();                                              \
                    else                                                                \
                        OUTPUT_STREAM << _T("#8261f: ~cTestRunner() found a non-joinable thread\n");    \
                    }                                                                   \
                }                                                                        \
                                                                                        \
            }; INSTANTIATE_TEST_OBJECT;                                                 \
        }                                                                               \
                                                                                        \
            inline void SelfTestUniqueName::cpccSelfTest::runTest(void)                 
        


