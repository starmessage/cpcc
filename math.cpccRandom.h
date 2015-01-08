
/*  *****************************************
 *  File:		math.cpccRandom.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, random numbers
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once

#include <time.h>
#include <stdlib.h>

class cpccRandom
{
private:
	static void		randomizeOnce(void)
	{
		static bool isRandomized=false;
		if (isRandomized)
			return;
		
		time_t randomSeed;
 		time( &randomSeed ); 
	
		srand((unsigned int)randomSeed);
		isRandomized=true;
	}
	
	static float random0to1(void)
	{
		randomizeOnce();
		return (float)rand() / ((float)(RAND_MAX)+1.0f) ;
	}

public:
	inline static int		random(const int From,const  int To)		{ return From + (int)((To-From)*random0to1()); }
	inline static long		random(const long From,const  long To)		{ return From + (long)((To-From)*random0to1()); }
	inline static double	random(const double From,const  double To)	{ return From + (To-From)*random0to1(); }
	inline static float		random(const float From,const float To)		{ return From + (To-From)*random0to1(); }

};