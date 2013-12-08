//
// cpccColor.cpp
//


#ifdef __APPLE__
    #if !defined __OBJC__
        #error File cpccColor must be compiled as obc++ under MAC. Did you include the cpccColor.cpp instead of cpccColor.mm ?
    #endif
#endif


#define NOMINMAX

#include "cpccColor.h"
#include "cpcc_SelfTest.h"
#include <assert.h>
#include <cmath>





#define practicallyEqual(x,y)		(std::abs(x-y)<0.00001f)

template<typename T>
void cpccColorT<T>::selfTest()
{
	cpccColorT<float>		cf1, cf2, cf3;
	cpccColorT<cpccUint8_t>	c1, c2, c3;

	assert( cf1.r==0.0f && cf1.g==0.0f && cf1.b==0.0f && cf1.a==1.0f && _T("#8571a: cpccColor"));
	assert( c1.r==0 && c1.g==0 && c1.b==0 && c1.a==255 && _T("#8571b: cpccColor"));

	cf1.r=0.8f; cf1.g=0.6f; cf1.b=0.4f; cf1.a=0.5f;
	cf1.amplifyComponents(0.5f, 0.5f, 0.5f);
			
	assert( practicallyEqual(cf1.r , 0.4f) && 
			practicallyEqual(cf1.g , 0.3f) && 
			practicallyEqual(cf1.b , 0.2f) && 
			practicallyEqual(cf1.a , 0.5f) && 
			_T("#8571c: cpccColor"));

	cf2 = cf1 *3.0f;
	assert( practicallyEqual(cf2.r, 1.0f) && 
			practicallyEqual(cf2.g, 0.9f) && 
			practicallyEqual(cf2.b, 0.6f) && 
			practicallyEqual(cf2.a, 0.5f) && 
			_T("#8571d: cpccColor"));
    
#ifdef __APPLE__
    cpccColor t1(30,60,120);
    NSColor *t2 = t1.asNSColor();
    
    cpccColor t3;
    t3.fromNSColor(t2);
    
    assert( (t3==t1) );
    // std::cout << "selftest of cpccColor: t3: R" << (int)t3.r << " G" << (int)t3.g << " B" << (int)t3.b << "\n";
#endif

}


SELFTEST_BEGIN(cpccColorT_SelfTest)
	if (cpccColorT<float>::config_RunSelfTest)
		cpccColorT<float>::selfTest();
SELFTEST_END