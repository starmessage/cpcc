
/*  *****************************************
 *  File:		algo.cpccWorkFlow_advancing.h
 *	Purpose:	Portable (cross-platform), light-weight, library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2016 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales (at) starmessage.info
 *	*****************************************
 */

#pragma once


class cpccWorkFlow_advancing
{
public:
	const int	m_WFsteps = 0;

protected:
	int			m_WFlastCompletedStep =0; // 1..n //

public:
	cpccWorkFlow_advancing(const int nSteps) : m_WFsteps(nSteps)
	{

	}

	virtual void doStep(const int stepNo)
	{
		// 1. call the previous step in case it was skipped
		if (m_WFlastCompletedStep < stepNo-1)
			if (stepNo>1)
				doStep(stepNo-1);

		// 2. do the step (the anscessor will do it)
		m_WFlastCompletedStep = stepNo;
	}

	
};


