#define _AMD64_ 
#include <fileapi.h>
#include <iostream>
#include <windows.h>
#include <errhandlingapi.h>
#include <cstdlib>
#include "huge_ptr.h"

void show_system_info() {
	SYSTEM_INFO info;

	GetSystemInfo(&info);

	std::cout << "gran: " << info.dwAllocationGranularity << std::endl;
	std::cout << "max: " << info.lpMaximumApplicationAddress << std::endl;
	std::cout << "min: " << info.lpMinimumApplicationAddress << std::endl;
	//std::cout << "Memory avail: " << get_memory_left() << std::endl;
}

//void* huge_allocate(size_t size) {
//
//}

int main() {

	//HANDLE file_created =
	//	CreateFileA("test_file", //  LPCSTR                lpFileName,
	//		GENERIC_READ | GENERIC_WRITE, //  DWORD                 dwDesiredAccess,
	//		FILE_SHARE_WRITE | FILE_SHARE_READ, //  DWORD dwShareMode,
	//		NULL,       //  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	//		CREATE_NEW, //  DWORD                 dwCreationDisposition,
	//		FILE_ATTRIBUTE_NORMAL, //  DWORD dwFlagsAndAttributes,
	//		NULL                   //  HANDLE                hTemplateFile
	//	);
	//std::cout << GetLastError() << std::endl;

	//if (!file_created)
	//{
	//	printf("file created handle empty\n");
	//	return 1;
	//}

	//HANDLE file_maped = CreateFileMappingA(
	//	file_created, //HANDLE                hFile,
	//	NULL, //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	//	PAGE_READWRITE, //DWORD                 flProtect,
	//	0, //DWORD                 dwMaximumSizeHigh,
	//	sizeof(long long), //DWORD                 dwMaximumSizeLow,
	//	"file_mapping" //LPCSTR                lpName
	//);
	//std::cout << GetLastError() << std::endl;

	//if (!file_maped)
	//{
	//	printf("file maped handle empty\n");
	//	return 1;
	//}

	//LPVOID ptr = MapViewOfFile(
	//	file_maped,	//HANDLE hFileMappingObject,
	//	FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
	//	0,	//DWORD  dwFileOffsetHigh,
	//	0,	//DWORD  dwFileOffsetLow,
	//	sizeof(long long) - 1	//SIZE_T dwNumberOfBytesToMap
	//);

	//if (!ptr)
	//{
	//	printf("file view ptr empty\n");
	//	return 1;
	//}
	////const char* cos = (const char*)ptr;
	////cos = "koniec";
	////ptr =(LPVOID) ((const char*)"Hello world");

	////memset(ptr, 0xaf, sizeof(long long));

	show_system_info();

	unsigned int big_num = 65000 * 3;

	auto my_ptr = mk::allocate_huge(big_num * sizeof(__int8));

	//*my_ptr[4096] = 'C';

	for (size_t i = 0; i < big_num - 10; i++) {
		std::clog << i << ",";
		if (i == 4096) {
			//*my_ptr[i] = i;
		}
		*my_ptr[i] = i;

	}
	const size_t graniularity = 65536;

	*my_ptr[0] = 'A';
	*my_ptr[10] = 'B';
	*my_ptr[100] = 'C';

	*my_ptr[graniularity + 0] = 'D';
	*my_ptr[graniularity + 10] = 'E';
	*my_ptr[graniularity + 100] = 'F';

	std::cout << "val: " << (int)*my_ptr[0] << std::endl;
	std::cout << "val: " << (int)*my_ptr[10] << std::endl;
	std::cout << "val: " << (int)*my_ptr[100] << std::endl;

	std::cout << "val: " << (int)*my_ptr[graniularity + 0] << std::endl;
	std::cout << "val: " << (int)*my_ptr[graniularity + 10] << std::endl;
	std::cout << "val: " << (int)*my_ptr[graniularity + 100] << std::endl;

	//for (size_t i = 0; i < big_num - 10; i++) {
	//	std::cout << "val: " << (int)*my_ptr[i] << std::endl;
	//}

	std::cout << "val: " << (int)*my_ptr[1] << std::endl;
	std::cout << "val: " << (int)*my_ptr[100] << std::endl;
	std::cout << "val: " << (int)*my_ptr[1000] << std::endl;
	std::cout << "val: " << (int)*my_ptr[60000] << std::endl;
}