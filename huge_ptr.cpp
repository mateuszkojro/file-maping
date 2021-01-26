#include "huge_ptr.h"
namespace mk {
	unsigned long long get_memory_left()
	{
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		return status.ullAvailPhys;
	}

	huge_ptr allocate_huge(size_t alloc_size)
	{

		HANDLE file_created =
			CreateFileA("temp_file", //  LPCSTR                lpFileName,
				GENERIC_READ | GENERIC_WRITE, //  DWORD                 dwDesiredAccess,
				FILE_SHARE_WRITE | FILE_SHARE_READ, //  DWORD dwShareMode,
				NULL,       //  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
				CREATE_NEW, //  DWORD                 dwCreationDisposition,
				FILE_ATTRIBUTE_NORMAL, //  DWORD dwFlagsAndAttributes,
				NULL                   //  HANDLE                hTemplateFile
			);

		//std::cout << GetLastError() << std::endl;
		if (!file_created)
		{
			printf("file created handle empty\n");
			exit(1);
		}

		HANDLE file_maped = CreateFileMappingA(
			file_created, //HANDLE                hFile,
			NULL, //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			PAGE_READWRITE,  //DWORD                 flProtect,
			0, //DWORD                 dwMaximumSizeHigh,
			alloc_size, //DWORD                 dwMaximumSizeLow,
			NULL //LPCSTR                lpName
		);

		if (!file_maped)
		{
			printf("file maped handle empty\n");
			exit(1);
		}

		LPVOID ptr = MapViewOfFile(
			file_maped,	//HANDLE hFileMappingObject,
			FILE_MAP_ALL_ACCESS, //FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
			0,	//DWORD  dwFileOffsetHigh,
			0,	//DWORD  dwFileOffsetLow,
			1	//SIZE_T dwNumberOfBytesToMap
		);

		if (!ptr)
		{
			printf("file view ptr empty\n");
			exit(1);
		}


		huge_ptr temp;
		temp.file_handle_ = file_created;
		temp.maped_handle_ = file_maped;
		temp.cur_ptr_ = (__int8*)ptr;
		temp.allocated_mem_ = alloc_size;
		//temp.page_size_ = page_size;
		temp.ofset_ = 0;

		return temp;

	}
	__int8* huge_ptr::operator[](size_t position)
	{
		const size_t graniularity = 65536;
		size_t allocation_block = position / graniularity;
		size_t allocation_position = position % graniularity;

		//HANDLE file_maped = CreateFileMappingA(
		//	file_handle_, //HANDLE                hFile,
		//	NULL, //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
		//	PAGE_READWRITE, //DWORD                 flProtect,
		//	0, //DWORD                 dwMaximumSizeHigh,
		//	65536, //DWORD                 dwMaximumSizeLow,
		//	"file_mapping" //LPCSTR                lpName
		//);
		//
		
		UnmapViewOfFile(this->cur_ptr_);

		LPVOID ptr = MapViewOfFile(
			maped_handle_,	//HANDLE hFileMappingObject,
			FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
			0,	//DWORD  dwFileOffsetHigh,
			graniularity * allocation_block,	//DWORD  dwFileOffsetLow,
			(position % graniularity) + sizeof(__int8)	//SIZE_T dwNumberOfBytesToMap
		);


		//std::cout << GetLastError() << std::endl;
		if (!ptr)
		{
			printf("file view ptr empty\n");
			std::cout << GetLastError() << std::endl;
			exit(1);
		}

		this->cur_ptr_ = (__int8*)ptr;


		return (((__int8*)ptr) + (allocation_position));
	}
}

