//
// cpcc_SelfTest.h
//

#ifndef TmioanSelfTest_h
#define TmioanSelfTest_h

#define ENABLE_SELF_TESTS


#ifdef ENABLE_SELF_TESTS		
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


#endif 