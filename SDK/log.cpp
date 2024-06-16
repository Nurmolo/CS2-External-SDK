#include "log.h"

/*OUTPUT*/
void output(string output, int type)
{
    std::time_t currentTime;
    std::time(&currentTime);
    std::tm* localTime = std::localtime(&currentTime);

    char buffer[20];
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);
    std::sprintf(buffer + 8, ".%03lld", ms.count());
    int outputtype = type;
    string message = output;
    switch (outputtype)
    {
    case 1:
        cout << GREEN << "[" << buffer << "]" << RESET << "| " << message << RESET << endl;
        ZeroMemory(&buffer, 20);
        break;
    case 2:
        cout << RED << "[" << buffer << "]" << RESET << "| " << message << RESET << endl;
        ZeroMemory(&buffer, 20);
        break;
    case 3:
        cout << YELLOW << "[" << buffer << "]" << RESET << "| " << message << RESET << endl;
        ZeroMemory(&buffer, 20);
        break;
    case 4:
        cout << "[" << buffer << "]" << RESET << "| " << message << RESET << endl;
        break;
    default:
        cout << YELLOW << "!!! INVALID PARAMS !!!" << RESET << endl;
        break;
    }
}

void logger::info(string message)
{
    string data = message;
    output(data, 4);
}
/* Warning */
void logger::warning(string message)
{
    string data = message;
    data + " | NURMOLO";
    output(data, 3);
}
/* Error */
void logger::error(string message, bool getlasterror)
{
    std::string data = message;
    data + " | NURMOLO";
    if (getlasterror)
    {
        DWORD error = GetLastError();
        data += " Error : " + std::to_string(error);
    }
    output(data, 2);
}
/* Success */
void logger::success(string message)
{
    string data = message;
    data + " | NURMOLO";
    output(data, 1);
}

/* END OUTPUT*/


/* UTILS */

/* Get time */
std::string logger::getcurrenttime()
{
    std::time_t currentTime;
    std::time(&currentTime);
    std::tm* localTime = std::localtime(&currentTime);

    char buffer[20];
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);
    std::sprintf(buffer + 8, ".%03lld", static_cast<long long int>(ms.count()));

    return std::string(buffer);
}

/* Log 2 file */
void logger::logtofile(const std::string& message, const std::string& filename, bool overwrite, bool debug)
{
    std::ofstream outfile;
    if (overwrite)
    {
        outfile.open(filename);
    }
    else
    {
        outfile.open(filename, std::ios::app);
    }
    if (outfile.is_open())
    {
        std::string currentTime = logger::getcurrenttime();
        outfile << currentTime << " | " << message << std::endl;
        if (debug)
        {
            logger::success("Logged data to file successfully");
        }
        outfile.close();
    }
    else
    {
        logger::error("Failed to open file", true);
    }
}

/* Thread func */
void logger::threadfunction(FuncPtr func, bool detach)
{
    thread t1(func);
    t1.join();
    if (detach == true)
    {
        t1.detach();
    }
}
/* Track func speed */
void logger::trackfunctionspeed(FuncPtr func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    chrono::duration<double> timetook = end - start;

    std::cout << GREEN << "[" << logger::getcurrenttime << "]" << RESET << "| "
        << func << " took : " << timetook.count() << " seconds" << RESET << std::endl;
}
