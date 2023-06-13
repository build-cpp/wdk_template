#include "driver.hpp"
#include "log.hpp"

#define PREFIX "[MyDriver] "
#define PREFIX_LEN (sizeof(PREFIX) - 1)

// TODO: replace this with a proper logging system
void Log(_In_z_ _Printf_format_string_ const char* format, ...)
{
	// Format the string
	char msg[1024] = PREFIX;
	va_list vl;
	va_start(vl, format);
	const int n = _vsnprintf(msg + PREFIX_LEN, sizeof(msg) - PREFIX_LEN - 2, format, vl);
	va_end(vl);

	// Append CRLF
	auto index = PREFIX_LEN + n;
	msg[index++] = '\r';
	msg[index++] = '\n';
	msg[index++] = '\0';

#ifdef _DEBUG
	// Log to the debugger
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, msg);
#endif

	// Log to the file
	// TODO: use some symbolic link to not hardcode the driver letter
	UNICODE_STRING FileName;
	OBJECT_ATTRIBUTES objAttr;
	RtlInitUnicodeString(&FileName, L"\\DosDevices\\C:\\MyDriver.log");
	InitializeObjectAttributes(&objAttr, &FileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);
	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
	{
#ifdef _DEBUG
		DbgPrint(PREFIX "KeGetCurrentIrql != PASSIVE_LEVEL!\n");
#endif
		return;
	}

	HANDLE handle;
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS ntstatus = ZwCreateFile(&handle,
		FILE_APPEND_DATA,
		&objAttr, &ioStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);
	if (NT_SUCCESS(ntstatus))
	{
		size_t cb;
		ntstatus = RtlStringCbLengthA(msg, sizeof(msg), &cb);
		if (NT_SUCCESS(ntstatus))
			ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock, msg, (ULONG)cb, NULL, NULL);
		ZwClose(handle);
	}
}
