#include <ntifs.h>
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

extern "C" {
    NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName,
                                        PDRIVER_INITIALIZE InitializationFunction);

    NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress,
                                             PEPROCESS TargetProcess, PVOID TargetAddress,
                                             SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode,
                                             PSIZE_T ReturnSize);


}

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

   
    struct Request {
        HANDLE process_id;

        PVOID target;
        PVOID buffer;

        SIZE_T size;
        SIZE_T return_size;
    };

    NTSTATUS create(PDEVICE_OBJECT device_object, PIRP irp) {
        UNREFERENCED_PARAMETER(device_object);

        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return irp->IoStatus.Status;
    }

    NTSTATUS close(PDEVICE_OBJECT device_object, PIRP irp) {
        UNREFERENCED_PARAMETER(device_object);

        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return irp->IoStatus.Status;
    }

    NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp) {
        UNREFERENCED_PARAMETER(device_object);


        NTSTATUS status = STATUS_UNSUCCESSFUL;

        PIO_STACK_LOCATION stack_irp = IoGetCurrentIrpStackLocation(irp);

        auto request = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer);

        if (stack_irp == nullptr || request == nullptr) {
            IoCompleteRequest(irp, IO_NO_INCREMENT);
            return status;
        }

        static PEPROCESS target_process = nullptr;

        const ULONG control_code = stack_irp->Parameters.DeviceIoControl.IoControlCode;
        switch (control_code) {
            case codes::attach:
                status = PsLookupProcessByProcessId(request->process_id, &target_process);
                break;

                // Read process memory implementation.
            case codes::read:
                if (target_process != nullptr)
                    status = MmCopyVirtualMemory(target_process, request->target,
                                                 PsGetCurrentProcess(), request->buffer,
                                                 request->size, KernelMode, &request->return_size);
                break;

            case codes::write:
                if (target_process != nullptr)
                    status = MmCopyVirtualMemory(PsGetCurrentProcess(), request->buffer,
                                                 target_process, request->target, request->size,
                                                 KernelMode, &request->return_size);
                break;

            default:
                break;
        }

        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(Request);

        IoCompleteRequest(irp, IO_NO_INCREMENT);

        return status;
    }

}  // namespace driver


NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
    UNREFERENCED_PARAMETER(registry_path);
    UNICODE_STRING device_name = {};
    RtlInitUnicodeString(&device_name, L"\\Device\\MICROSFTUFEISECUREBOOT3831748138");
    PDEVICE_OBJECT device_object = nullptr;
    NTSTATUS status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN,FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
    if (status != STATUS_SUCCESS) {
        return status;
    }
    UNICODE_STRING symbolic_link = {};
    RtlInitUnicodeString(&symbolic_link, L"\\DosDevices\\MICROSFTUFEISECUREBOOT3831748138");
    status = IoCreateSymbolicLink(&symbolic_link, &device_name);
    if (status != STATUS_SUCCESS) {
        
        return status;
    }
    SetFlag(device_object->Flags, DO_BUFFERED_IO);
    driver_object->MajorFunction[IRP_MJ_CREATE] = driver::create;
    driver_object->MajorFunction[IRP_MJ_CLOSE] = driver::close;
    driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::device_control;
    ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);
    return status;
}


NTSTATUS Drivermain() {
    UNICODE_STRING driver_name = {};
    RtlInitUnicodeString(&driver_name, L"\\Driver\\MICROSFTUFEISECUREBOOT3831748138");
    return IoCreateDriver(&driver_name, &driver_main);
}
