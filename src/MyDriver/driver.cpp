#include "driver.hpp"
#include "log.hpp"

static UNICODE_STRING Win32Device;

_Function_class_(DRIVER_UNLOAD)
_IRQL_requires_(PASSIVE_LEVEL)
_IRQL_requires_same_
static void DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    Log("[MyDriver] DriverUnload");
    IoDeleteSymbolicLink(&Win32Device);
    IoDeleteDevice(DriverObject->DeviceObject);
}

_Function_class_(DRIVER_DISPATCH)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
static NTSTATUS DriverCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

_Function_class_(DRIVER_DISPATCH)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
static NTSTATUS DriverDefaultHandler(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_NOT_SUPPORTED;
}

_Function_class_(DRIVER_INITIALIZE)
_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    // Set callback functions
    DriverObject->DriverUnload = DriverUnload;
    for (unsigned int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
        DriverObject->MajorFunction[i] = DriverDefaultHandler;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;

    // Create a device
    UNICODE_STRING DeviceName;
    RtlInitUnicodeString(&DeviceName, L"\\Device\\MyDriver");
    RtlInitUnicodeString(&Win32Device, L"\\DosDevices\\MyDriver");
    PDEVICE_OBJECT DeviceObject = nullptr;
    auto status = IoCreateDevice(DriverObject,
        0,
        &DeviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &DeviceObject);
    if (!NT_SUCCESS(status))
    {
        Log("[MyDriver] IoCreateDevice Error...\r\n");
        return status;
    }
    if (!DeviceObject)
    {
        Log("[MyDriver] Unexpected I/O Error...\r\n");
        return STATUS_UNEXPECTED_IO_ERROR;
    }

    Log("[MyDriver] DriverEntry\n");

    return STATUS_SUCCESS;
}