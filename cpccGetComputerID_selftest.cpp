
/*  *****************************************
 *  File:		cpccGetComputerID_selftest.cpp
 *	Purpose:	Portable (cross-platform), light-weight, 
 *				ComputerID for license registration systems
 *				Include this file in your project if you want to run a selftest of the class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#include "io.cpccLog.h"		// this uses cpccTimeCounter.h and must be included before <windows.h> 
#include "cpccGetComputerID.h"
#include "cpcc_selfTest.h"

#ifdef _WIN32
	#define dumpFile "c:\\tmp\\getComputerID-test.txt"
#elif  __APPLE__
	#define dumpFile "~/desktop/getComputerID-test.txt"
#endif	

SELFTEST_BEGIN(cpccGetComputerID_SelfTest)
	
	    logObjectLife logThis("cpccGetComputerID::selfTest()");
        
        cpccGetComputerID<33> mid;
        infoLog().addf("current executable (self):%s",mid.getSelfFilename().c_str());
        
        time_t adate = mid.getSelfModifyDate();
		#pragma warning( disable : 4996 )
        std::tm * ptm = std::localtime(&adate);
        char buffer[42];
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        infoLog().addf("modification date of current executable (self):%s",buffer);
		
		//////////////////////////

		infoLog().addf("Characteristic OS file:%s", mid.getCharacteristicOSFile().c_str());
		adate = mid.getOSModifyDate();
		#pragma warning( disable : 4996 )
        ptm = std::localtime(&adate);
        // Format: Mo, 15.06.2009 20:20:00
        std::strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
        infoLog().addf("modification date of OS folder:%s",buffer);
    
		time_t _tm = time(NULL);
		struct tm * curtime = localtime(&_tm);
		cpcc_string txt ("V2: ");
        cpcc_string compName = cpccOS::getComputerName();
        txt.append(compName);
		txt.append(_T("\tHash:")); 
		txt.append(mid.getHash(compName));
		txt.append(_T("\t"));
		txt.append(asctime(curtime)); // asctime contains a CRLF
		//txt.append(_T("\n"));

		cpccFileSystemMiniEx fs;
		assert( fs.appendTextFile(dumpFile, txt) && "#9572: cpccGetComputerID_SelfTest: Failed to write to dumpFile");
	
SELFTEST_END
    

    
