#include "huge_ptr.h"
namespace mk {

	// how much physicaal memory is left on the system
	static size_t get_memory_left()
	{
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		return status.ullAvailPhys;
	}

	// we need to get the allocation granularity
	// na sczescie wystarczy pobraac @SYSTEM_INFO
	static size_t get_graniualrity()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return  info.dwAllocationGranularity;
	}

	// allocaate huge amounts of memory through file maping and creatin views into file 
	// on disk
	huge_ptr allocate_huge(size_t alloc_size)
	{

		HANDLE file_handle =
			CreateFileA(
				"temp_file", // file name
				GENERIC_READ | GENERIC_WRITE, // define desired acces rights
				FILE_SHARE_WRITE | FILE_SHARE_READ, // can handle be shared NULL if not 
				NULL,       //  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
				CREATE_NEW, //  DWORD                 dwCreationDisposition,
				FILE_ATTRIBUTE_NORMAL, //  DWORD dwFlagsAndAttributes,
				NULL                   //  HANDLE                hTemplateFile
			);

		if (!file_handle)
		{
			printf("file created handle empty\n");
			exit(1);
		}

		HANDLE maped_handle = CreateFileMappingA(
			file_handle, // file handle from @CreateFileA
			NULL, //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			PAGE_READWRITE,  //DWORD                 flProtect,
			0, //DWORD                 dwMaximumSizeHigh,
			alloc_size, // how much of memory to map
			NULL // name for this handle if neded to share between processes
		);

		if (!maped_handle)
		{
			printf("file maped handle empty\n");
			exit(1);
		}

		LPVOID ptr = MapViewOfFile(
			maped_handle,	//HANDLE hFileMappingObject,
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
		temp.file_handle_ = file_handle;
		temp.maped_handle_ = maped_handle;
		temp.cur_ptr_ = (T*)ptr;
		temp.allocated_mem_ = alloc_size;
		//temp.page_size_ = page_size;
		temp.ofset_ = 0;

		return temp;

	}

	// creaates new file view from the desired position of size 
	// equal to size of one stored element and return reference
	T& huge_ptr::operator[](size_t position)
	{
		const size_t graniularity = get_graniualrity();
		const size_t allocation_block = position / graniularity;
		const size_t allocation_position = position % graniularity;

		UnmapViewOfFile(this->cur_ptr_);

		LPVOID ptr = MapViewOfFile(
			this->maped_handle_,	//HANDLE hFileMappingObject,
			FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
			0,	//DWORD  dwFileOffsetHigh,
			graniularity * allocation_block,	//DWORD  dwFileOffsetLow,
			(position % graniularity) + sizeof(T)	//SIZE_T dwNumberOfBytesToMap
		);

		if (!ptr)
		{
			printf("file view ptr empty\n");
			exit(1);
		}

		this->cur_ptr_ = (T*)ptr;

		return this->cur_ptr_[allocation_position];
	}
}

