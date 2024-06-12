#pragma once
/*
   ____ ____ ____    ____  ____  _  __  ____    _    ____  _____ 
  / ___/ ___|___ \  / ___||  _ \| |/ / | __ )  / \  / ___|| ____|
 | |   \___ \ __) | \___ \| | | | ' /  |  _ \ / _ \ \___ \|  _|  
 | |___ ___) / __/   ___) | |_| | . \  | |_) / ___ \ ___) | |___ 
  \____|____/_____| |____/|____/|_|\_\ |____/_/   \_\____/|_____|
                                                                 

      $$$$$$\                                                           $$\           
  $$$ ___$$$\                                                         $$ |          
 $$ _/   \_$$\  $$$$$$$\  $$\   $$\  $$$$$$\  $$$$$$\$$$$\   $$$$$$\  $$ | $$$$$$\  
$$ / $$$$$\ $$\ $$  __$$\ $$ |  $$ |$$  __$$\ $$  _$$  _$$\ $$  __$$\ $$ |$$  __$$\ 
$$ |$$  $$ |$$ |$$ |  $$ |$$ |  $$ |$$ |  \__|$$ / $$ / $$ |$$ /  $$ |$$ |$$ /  $$ |
$$ |$$ /$$ |$$ |$$ |  $$ |$$ |  $$ |$$ |      $$ | $$ | $$ |$$ |  $$ |$$ |$$ |  $$ |
$$ |\$$$$$$$$  |$$ |  $$ |\$$$$$$  |$$ |      $$ | $$ | $$ |\$$$$$$  |$$ |\$$$$$$  |
\$$\ \________/ \__|  \__| \______/ \__|      \__| \__| \__| \______/ \__| \______/ 
 \$$$\   $$$\                                                                       
  \_$$$$$$  _|                                                                      
    \______/                                                                                                                                       


    ??? ALL CREDITS GOTO @.damc @nurmolo on discord #no pasting ???
*/
// driver name MICROSFTUFEISECUREBOOT3831748138
namespace driver {
    namespace codes {
        // Used to setup the driver.
        constexpr ULONG attach =
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80001, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        // Read process memory.
        constexpr ULONG read =
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80002, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        // Read process memory.
        constexpr ULONG write =
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    }  // namespace codes

    // Shared between user mode & kernel mode.
    struct Request {
        HANDLE process_id;

        PVOID target;
        PVOID buffer;

        SIZE_T size;
        SIZE_T return_size;
    };

    bool attach_to_process(HANDLE driver_handle, const DWORD pid) {
        Request r;
        r.process_id = reinterpret_cast<HANDLE>(pid);

        return DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr,
            nullptr);
    }

    template <class T>
    T read_memory(HANDLE driver_handle, const std::uintptr_t addr) {
        T temp = {};

        Request r;
        r.target = reinterpret_cast<PVOID>(addr);
        r.buffer = &temp;
        r.size = sizeof(T);

        DeviceIoControl(driver_handle, codes::read, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);

        return temp;
    }
    template <typename T>
    T get_module_base(const std::wstring& module_name) {
        T base_address = nullptr;
        ULONG bytes_returned = 0;

        // Your implementation code here...

        return base_address; // Ensure a return value is always provided
    }

    template <class T>
    void write_memory(HANDLE driver_handle, const std::uintptr_t addr, const T& value) {
        Request r;
        r.target = reinterpret_cast<PVOID>(addr);
        r.buffer = (PVOID)&value;
        r.size = sizeof(T);

        DeviceIoControl(driver_handle, codes::write, &r, sizeof(r), &r, sizeof(r), nullptr,
            nullptr);
    }
}  // namespace driver
