#pragma once
#define _AMD64_
#include <fileapi.h>
#include <windows.h>
#include <iostream>
#include <assert.h>



// --- zaimplementowac jako alocator --
// https://en.cppreference.com/w/cpp/memory/allocator

//typedef double T;
typedef long long int s_size_t;

namespace mk {
	template<class T>
	class huge_ptr
	{
	public:

		huge_ptr();
		huge_ptr(const huge_ptr&);
		huge_ptr(const huge_ptr&, s_size_t ofset);

		// dostep do koljenych adresow jak w zwyklym ptr
		T& operator[](size_t);
		// dereferenece the smart ptr
		T& operator*();
		// call a function on a ptr
		T* operator->();

		const huge_ptr& operator=(const huge_ptr& other);

		huge_ptr operator++();
		huge_ptr operator++(int);
		huge_ptr operator--();
		huge_ptr operator--(int);

		//reszta operatorow - casta na boola prownywanie itd.
		// tak jak np w unique_ptr

		static huge_ptr allocate_huge(size_t);
	private:
		// the size on the disk
		size_t page_size_;
		// size currently in memory
		size_t allocated_mem_;


		// how far are we from the begining of the file
		size_t ofset_;



		// curent file view ptr
		T* cur_ptr_;
		HANDLE file_handle_;
		HANDLE maped_handle_;
	};

	//template<class K>
	//huge_ptr<K> allocate_huge(size_t);
	//huge_ptr make_huge();

}


// -------------------------------------
// implementations
// --------------------------------------


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
	template<class T>
	huge_ptr<T> huge_ptr<T>::allocate_huge(size_t alloc_size)
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


		huge_ptr<T> temp;
		temp.file_handle_ = file_handle;
		temp.maped_handle_ = maped_handle;
		temp.cur_ptr_ = (T*)ptr;
		temp.allocated_mem_ = alloc_size;
		//temp.page_size_ = page_size;
		temp.ofset_ = 0;

		return temp;

	}

	template<class T>
	huge_ptr<T>::huge_ptr() {
		cur_ptr_ = nullptr;
		ofset_ = 0;
		assert(0);
	}
	template<class T>
	huge_ptr<T>::huge_ptr(const huge_ptr<T>& other) {
		cur_ptr_ = other.cur_ptr_;
		ofset_ = other.ofset_;
		file_handle_ = other.file_handle_;
		maped_handle_ = other.maped_handle_;
	}
	template<class T>
	const huge_ptr<T>& huge_ptr<T>::operator=(const  huge_ptr<T>& other) {
		if (this == &other) return *this;
		cur_ptr_ = other.cur_ptr_;
		ofset_ = other.ofset_;
		file_handle_ = other.file_handle_;
		maped_handle_ = other.maped_handle_;
		return *this;
	}
	template<class T>
	huge_ptr<T>::huge_ptr<T>(const  huge_ptr<T>& other, s_size_t  ofset)
	{
		cur_ptr_ = other.cur_ptr_;
		ofset_ = other.ofset_ + ofset;
		file_handle_ = other.file_handle_;
		maped_handle_ = other.maped_handle_;
	}

	// creaates new file view from the desired position of size 
	// equal to size of one stored element and return reference
	template<class T>
	T& huge_ptr<T>::operator[](size_t position)
	{
		const size_t calculated_position = position + ofset_;
		const size_t graniularity = get_graniualrity();
		const size_t allocation_block = calculated_position / graniularity;
		const size_t allocation_position = calculated_position % graniularity;

		//UnmapViewOfFile(this->cur_ptr_);

		LPVOID ptr = MapViewOfFile(
			this->maped_handle_,	//HANDLE hFileMappingObject,
			FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
			0,	//DWORD  dwFileOffsetHigh,
			graniularity * allocation_block,	//DWORD  dwFileOffsetLow,
			(calculated_position % graniularity) + sizeof(T)	//SIZE_T dwNumberOfBytesToMap
		);

		if (!ptr)
		{
			printf("file view ptr empty\n");
			exit(1);
		}

		cur_ptr_ = (T*)ptr;

		return this->cur_ptr_[calculated_position];
	}

	template<class T>
	T& huge_ptr<T>::operator* () {
		return operator[](0);
	}

	template<class T>
	huge_ptr<T>  huge_ptr<T>::operator++()
	{
		return  huge_ptr<T>(*this, ofset_ + 1);
	}

	template<class T>
	huge_ptr<T>  huge_ptr<T>::operator++(int)
	{
		huge_ptr<T> temp = *this;
		operator++();
		return temp;
	}

	template<class T>
	huge_ptr<T>  huge_ptr<T>::operator--()
	{
		return  huge_ptr<T>(*this, ofset_ - 1);
	}

	template<class T>
	huge_ptr<T>  huge_ptr<T>::operator--(int)
	{
		huge_ptr<T> temp = *this;
		operator--();
		return temp;
	}

}









