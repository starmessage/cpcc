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
#include <mutex>
#include <cstdlib>
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
    #define INSTANTIATE_TEST_OBJECT     static cpccSelfTest  aSelfTestObject
    // #define TestingVariableName aSelfTestObject
#else
    
    #define INSTANTIATE_TEST_OBJECT
    // #define TestingVariableName
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

    //=============================================================================
    // class testVariable
    // used to avoid runing the same test again
    //=============================================================================
    template <typename T>
    class testVariable
    {
    private:
        T value;
    public:
        testVariable(const T aDefaultValue): value(aDefaultValue) { }
        T get(void) { return value; }
        void set(const T aValue) { value = aValue; }
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

    // todo:this class is unused
    /*
    class cTestRunner
    {
    private:
        const std::basic_string<TCHAR> testName;
        std::basic_ostream<TCHAR> &reportStream;
        std::thread *threadPtr = NULL;
        
        // typedef pointer to a class member function: you need the class type
        // As of C++11, you could write this typedef as a more legible using statement:
        // using MyTypedef = int (MyClass::*)(int);
        
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
    */


} // end of namespace (the same namespace continues below)

#define OUTPUT_STREAM           cpccTesting::sharedObjects::outputStream().get()


#define TEST_EXPECT(aCONDITION, aMESSAGE)		    \
  if (!(aCONDITION))							    \
    {                                               \
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
#define TEST_RUN(SelfTestUniqueName)	            \
	namespace SelfTestUniqueName {			        \
        class cpccSelfTest          			    \
        {										    \
        public:									    \
            void runTest(void);                     \
                                                                            \
            void runFunctionWrapper(void)                                   \
            {   const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);        \
                if (cpccTesting::sharedTestRegister::testHasAlreadyRan(tmpNameA))   \
                    return;                                                         \
                OUTPUT_STREAM << _T("/ Starting test:") << tmpNameA << std::endl;   \
                runTest();                                         \
                OUTPUT_STREAM << _T("\\ Ending   test:") << tmpNameA << std::endl << std::endl; \
                int errors = cpccTesting::sharedTestRegister::getNErrors();          \
                if (errors > 0)                                                 \
                    OUTPUT_STREAM << errors << _T(" errors so far.") << std::endl;  \
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
#define TEST_RUN_ASYNC(SelfTestUniqueName)        \
        namespace SelfTestUniqueName {                    \
            class cpccSelfTest                          \
            {                                            \
            private:                                        \
                std::thread *threadPtr = NULL;                                    \
            public:                                        \
                void runTest(void);                     \
                                                                                \
                void runFunctionWrapper(void)                                   \
                {   const TCHAR* tmpNameA = TEST_MAKESTRING(SelfTestUniqueName);        \
                    if (cpccTesting::sharedTestRegister::testHasAlreadyRan(tmpNameA))   \
                        return;                                                         \
                    const int msec = 800 + (std::rand() % 1200);                 \
                    std::this_thread::sleep_for(std::chrono::milliseconds(msec));               \
                    OUTPUT_STREAM << _T("/ Starting test (in thread):") << tmpNameA << std::endl;   \
                    runTest();                                         \
                    OUTPUT_STREAM << _T("\\ Ending   test (in thread):") << tmpNameA << std::endl << std::endl; \
                    int errors = cpccTesting::sharedTestRegister::getNErrors();          \
                    if (errors > 0)                                                 \
                        OUTPUT_STREAM << errors << _T(" errors so far.") << std::endl;  \
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
            }; INSTANTIATE_TEST_OBJECT;                                        \
        }                                                                           \
                                                                                    \
        inline void SelfTestUniqueName::cpccSelfTest::runTest(void)



