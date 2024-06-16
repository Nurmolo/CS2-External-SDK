#include <Windows.h>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>  
#include <sstream>   
#include <filesystem>
#include <fstream>
#include <iostream>
#include <fstream>
/* TODO
Add custom logs for user 2 use. EX : Build your own log func. customlog(0,0,0,0);
Maybe build info??
Network logs?????
*/

typedef void (*FuncPtr)();
#pragma warning(disable : 4996)
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m" 

using namespace std;
namespace logger
{
	std::string getcurrenttime();
	void logtofile(const std::string& message, const std::string& filename, bool overwrite,bool debug);
	void threadfunction(FuncPtr func, bool detach);
    void trackfunctionspeed(FuncPtr func);
	void error(string message,bool getlasterror);
	void warning(string message);
	void success(string message);
	void info(string message);
}