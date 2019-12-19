/*  *****************************************
 *  File:		cpccTesting.mm
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2019 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:   https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include "cpccTesting.h"
#include <Foundation/Foundation.h>


namespace cpccTesting
{
    std::string util::getDesktopFolder(std::string *err)
    {
        // There's no guarantee that the user's desktop is at $HOME/Desktop
        // The proper way to get it is via NSSearchPathForDirectoriesInDomains
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDesktopDirectory, NSUserDomainMask, YES );
        NSString* theDesktopPath = [paths objectAtIndex:0];
        if (theDesktopPath)
            return  [theDesktopPath UTF8String];
        if (err)
            *err = "Error #5723 in getDesktopFolder()";
        return "";
    }
    
    
}; // end of namespace cpccTesting
