/*
 *  cpcc_SelfTest.h
 *  Cross Platform C++ Classes
 *
 *  Copyright 2013 StarMessage software.
 *  Free for opensource projects.
 *  Commercial license for closed source projects.
 *	http://www.StarMessageSoftware.com
 */


#ifndef cpcc_SelfTest_h
#define cpcc_SelfTest_h


/// Central switch to close all selfTests in your application
#define ENABLE_SELF_TESTS	1


#if (ENABLE_SELF_TESTS==1)
	#define SelftestVariableName aSelfTest
#else
	#define SelftestVariableName
#endif


#define SELFTEST_BEGIN(stname)		\
	namespace stname {				\
	class TmioanSelfTest			\
	{								\
	public:							\
		TmioanSelfTest()			\
		{ 




#define SELFTEST_END			\
		};						\
	} SelftestVariableName; }


#endif // cpcc_SelfTest_h