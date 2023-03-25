#include "Log.h"
#include <fstream>

//This opens the file.
Log::Log(std::string filename)
{
	logFile.open(filename);
}

//This is for performing the log.
void Log::log(std::string logMe)
{
	logFile << logMe << std::endl;
}