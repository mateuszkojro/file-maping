#define _AMD64_ 
#include <fileapi.h>
#include <iostream>
#include <windows.h>
#include <errhandlingapi.h>
#include <cstdlib>

unsigned long long get_memory_left()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailPhys;
}
void show_system_info() {
	SYSTEM_INFO info;

	GetSystemInfo(&info);

	std::cout << "Procesor info: " << info.dwAllocationGranularity << std::endl;
	std::cout << "Memory avail: " << get_memory_left() << std::endl;
}

//void* huge_allocate(size_t size) {
//
//}

int main() {

	HANDLE file_created =
		CreateFileA("test_file", //  LPCSTR                lpFileName,
			GENERIC_READ | GENERIC_WRITE, //  DWORD                 dwDesiredAccess,
			FILE_SHARE_WRITE | FILE_SHARE_READ, //  DWORD dwShareMode,
			NULL,       //  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			CREATE_NEW, //  DWORD                 dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL, //  DWORD dwFlagsAndAttributes,
			NULL                   //  HANDLE                hTemplateFile
		);
	std::cout << GetLastError() << std::endl;

	if (!file_created)
	{
		printf("file created handle empty\n");
		return 1;
	}

	HANDLE file_maped = CreateFileMappingA(
		file_created, //HANDLE                hFile,
		NULL, //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
		PAGE_READWRITE, //DWORD                 flProtect,
		0, //DWORD                 dwMaximumSizeHigh,
		sizeof(long long), //DWORD                 dwMaximumSizeLow,
		"file_mapping" //LPCSTR                lpName
	);
	std::cout << GetLastError() << std::endl;

	if (!file_maped)
	{
		printf("file maped handle empty\n");
		return 1;
	}

	LPVOID ptr = MapViewOfFile(
		file_maped,	//HANDLE hFileMappingObject,
		FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
		0,	//DWORD  dwFileOffsetHigh,
		0,	//DWORD  dwFileOffsetLow,
		sizeof(long long) - 1	//SIZE_T dwNumberOfBytesToMap
	);

	if (!ptr)
	{
		printf("file view ptr empty\n");
		return 1;
	}
	//const char* cos = (const char*)ptr;
	//cos = "koniec";
	//ptr =(LPVOID) ((const char*)"Hello world");

	//memset(ptr, 0xaf, sizeof(long long));

	int* cos = (int*)malloc(2 * sizeof(int));
	cos[0] = 1;
	cos[1] = 0;

	std::cout << GetLastError() << std::endl;
	std::cout << (int)*(const char*)ptr << std::endl;
}