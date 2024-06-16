#include "sdk.h"
#include "../Driver/com.h"

BOOL sdk::IsElevated() {
    /* Credits to breached
    https://stackoverflow.com/questions/8046097/how-to-check-if-a-process-has-the-administrative-rights
    */
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}
DWORD sdk::get_process_id(const wchar_t* process_name) {
    DWORD process_id = 0;

    HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snap_shot == INVALID_HANDLE_VALUE)
        return process_id;

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(decltype(entry));

    if (Process32FirstW(snap_shot, &entry) == TRUE) {
        // Check if the first handle is the one we want.
        if (_wcsicmp(process_name, entry.szExeFile) == 0)
            process_id = entry.th32ProcessID;
        else {
            while (Process32NextW(snap_shot, &entry) == TRUE) {
                if (_wcsicmp(process_name, entry.szExeFile) == 0) {
                    process_id = entry.th32ProcessID;
                    break;
                }
            }
        }
    }

    CloseHandle(snap_shot);

    return process_id;
}
uintptr_t sdk::get_module_base(const DWORD pid, const wchar_t* module_name) {
    std::uintptr_t module_base = 0;

    // Snap-shot of process' modules (dlls).
    HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap_shot == INVALID_HANDLE_VALUE)
        return module_base;

    MODULEENTRY32W entry = {};
    entry.dwSize = sizeof(decltype(entry));

    if (Module32FirstW(snap_shot, &entry) == TRUE) {
        if (wcsstr(module_name, entry.szModule) != nullptr)
            module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
        else {
            while (Module32NextW(snap_shot, &entry) == TRUE) {
                if (wcsstr(module_name, entry.szModule) != nullptr) {
                    module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
                    break;
                }
            }
        }
    }
    CloseHandle(snap_shot);

    return module_base;
}
uintptr_t sdk::setupmodules()
{
    logger::info("Getting process base");
    sdk::procbase = sdk::get_module_base(sdk::pid, procname);
    if (sdk::clientdll == NULL)
    {
        sdk::error = 12;
        logger::error("Failed to get process base!", false);
        exit(1);
    }
    logger::info("Getting client.dll module base");
    sdk::clientdll = sdk::get_module_base(sdk::pid, sdk::clientdllname);
    if (sdk::clientdll == NULL)
    {
        sdk::error = 12;
        logger::error("Failed to get client.dll", false);
        exit(1);
    }
    // add modules as needed
}

void sdk::setupoffsets()
{
    sdk::clientdll = sdk::get_module_base(sdk::pid, L"client.dll");
    player::localplayer = memory::read_memory<uintptr_t>(sdk::driver, sdk::clientdll + client_dll::dwLocalPlayerPawn);
    player::health = memory::read_memory<uintptr_t>(sdk::driver, sdk::clientdll + player::localplayer + client_dll::C_BaseEntity::m_iHealth);
    player::teamnum = memory::read_memory<uintptr_t>(sdk::driver, sdk::clientdll + player::localplayer + client_dll::C_BaseEntity::m_iTeamNum);
    player::m_lifeState = memory::read_memory<uintptr_t>(sdk::driver, sdk::clientdll + player::localplayer + client_dll::C_BaseEntity::m_lifeState);
    logger::info("Got all memory address! Printing...");
    cout << "[" << logger::getcurrenttime() << "]" << "| " << "Local Player : 0x" << player::localplayer << endl;
    cout << "[" << logger::getcurrenttime() << "]" << "| " << "Local Player Health : 0x" << player::health << endl;
    cout << "[" << logger::getcurrenttime() << "]" << "| " << "Local Player Team Num : 0x" << player::teamnum << endl;
    cout << "[" << logger::getcurrenttime() << "]" << "| " << "Local Player Lifestate : 0x" << player::m_lifeState << endl;
}
void sdk::checkproc()
{
    while (true)
    {
         HWND temphwnd = FindWindowA("Notepad", NULL);
         if (temphwnd == NULL)
         {
             logger::error("Integerity Check failed cs2 is not running! [WINDOW CHECK]", false);
             Sleep(1000);
             sdk::error = 11;
             exit(1);
         }
         else
         {
             logger::success("Process passed integrity check [WINDOW CHECK]");
         }
       
        HANDLE temphandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, sdk::pid);
        if (temphandle == NULL)
        {
            logger::error("Integerity Check failed cs2 is not running! [HANDLE CHECK]", false);
            Sleep(1000);
            sdk::error = 11;
            exit(1);
        }
        else
        {
            logger::success("Process passed integrity check [HANDLE CHECK]");
        }
        CloseHandle(temphandle);
        EndDialog(temphwnd, 0);
        // extra stuff
        ZeroMemory(&temphandle, sizeof(temphandle));
        ZeroMemory(&temphwnd, sizeof(temphwnd));
        this_thread::sleep_for(std::chrono::seconds(1));
    }
  
}
void sdk::initdriver()
{
    logger::info("Creating a handle to driver");
    sdk::driver = CreateFileA("\\\\.\\pastahook", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (sdk::driver == INVALID_HANDLE_VALUE)
    {
        logger::error("Failed to connect driver!", true);
        sdk::error = 13;
        Sleep(1000);
        exit(1);
    }
    logger::success("Created a handle to driver!");
    logger::info("Attaching to process");
    memory::attach_to_process(sdk::driver, sdk::pid);
    // no error checking cause admin check limits the issues add yourself (;
    logger::success("Attached to process!");
}
void sdk::init()
{
   sdk::pid = sdk::get_process_id(sdk::procname);
   if (pid == NULL)
   {
       logger::error("Failed to find cs2.exe is running?", true);
       sdk::error = 10;
       Sleep(1000);
       exit(1);
   }  logger::success("Found cs2 succesfully!");
   
   logger::info("Starting integrity thread");
   /*
std::thread integrityThread(&sdk::checkproc, this); creates a thread that executes the member function checkproc with the current instance of sdk (this) as the object on which to operate.
integrityThread.detach(); detaches the thread, allowing it to run independently from the main thread.
*/
   std::thread integrityThread(&sdk::checkproc, this);
   logger::info("Thread detached");
   integrityThread.detach();
   
   logger::warning("These function have no checking if its setup or not meaning these can be setup many times over!");
   logger::info("Initlizing Driver");
   sdk::initdriver();
   logger::info("Setting up memory address");
   sdk::setupoffsets();
   cin.get();

}
