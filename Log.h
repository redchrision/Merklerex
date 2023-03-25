#pragma once
#include <string>
#include <fstream>

// This class logs to a file. The logger should load in csv format, because it will be machine - readable.
// The log class has a single function, log, which takes as input a string rather than more strucured data because 
// it will be used to log multiple different kids of data and it's easier this way. 
// It has a constructor that takes a filename of the log file which it should write. 

class Log
{
public:
	Log(std::string filename);
	void log(std::string logMe);

private:
	std::ofstream logFile;
};