//
// cpccLog.h
//
 
#pragma once

/* testing for std::cout capture */

#include <iostream>
#include <fstream>

/*	I have to extend the ofstream class to write a logging stream service.
	The goal is to intercept each line, adding at the head of each one a 
	customized text (date/time, severity, and so on).
	http://stackoverflow.com/questions/13976262/customized-ofstream-output
	
*/


/*
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.
    std::istream::sentry se(is, true);
*/


class cpccLogStream: public std::ofstream
{
private:
	

public: 
	char *m_tag;

	cpccLogStream() 	{ }
	
	/*
	template <typename T>
	std::ofstream& operator<<(ofstream &s, const T& data_)
	{
		s << "Info>\t" << << data_;
		return *this;
	}
	*/
};


//	http://www.cplusplus.com/reference/ios/ios/rdbuf/

class ScopedRedirect
{	// 	http://stackoverflow.com/questions/4753153/redirecting-stdcout

private:
    ScopedRedirect(const ScopedRedirect&);
    ScopedRedirect& operator=(const ScopedRedirect&);

    std::ostream & 		mOriginal;
    std::streambuf * 	mOldBuffer;
	
public:
    ScopedRedirect(std::ostream & inOriginal, std::ostream & inRedirect) :
        mOriginal(inOriginal),
        mOldBuffer(inOriginal.rdbuf(inRedirect.rdbuf()))
    { }

    ~ScopedRedirect()
    {
        mOriginal.rdbuf(mOldBuffer);
    }    

};




class cpccLog
{
private:
	//std::ostream & 		mCout;
    std::streambuf * 	mCoutBuffer;
	cpccLogStream		fileForCout;
	//std::ofstream		fileForCout;

public: 
	cpccLog(char *aFilename) // : mCout(std::cout)
	{
		//fileForCout.m_tag = "Info> ";
		//fileForCout.open(aFilename, std::ios_base::out | std::ios_base::ate | std::ios_base::app | std::ios_base::trunc);
		fileForCout.open(aFilename);
		mCoutBuffer = std::cout.rdbuf(fileForCout.rdbuf());
	}

	~cpccLog()
	{
		std::cout.rdbuf(mCoutBuffer);
	}
	
	
};


// http://www.physics.utah.edu/~detar/lessons/c++/canned_classes/node8.html
// http://forums.xkcd.com/viewtopic.php?f=11&t=57696
// http://www.gamedev.net/topic/622694-custom-subclass-of-stdostream-for-custom-printing/
// http://stackoverflow.com/questions/772355/how-to-inherit-from-stdostream


/* example
int main()
{
    std::cout << "Before redirect." << std::endl;
    std::ofstream filestream("redirected.txt");
    {
        ScopedRedirect redirect(std::cout, filestream);
        std::cout << "During redirect." << std::endl;

        // yay:
        filestream << "also to the file, right?...yes" << std::endl;
        filestream << "i am not on the screen" << std::endl;
    }
    std::cout << "After redirect." << std::endl;

    return 0;
}
*/

