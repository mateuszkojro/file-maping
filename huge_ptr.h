#pragma once
#define _AMD64_
#include <fileapi.h>
#include <windows.h>
#include <iostream>
#include <assert.h>



// --- zaimplementowac jako alocator --
// https://en.cppreference.com/w/cpp/memory/allocator

typedef double T;
typedef long long int s_size_t;

namespace mk {

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
	
		friend huge_ptr allocate_huge(size_t);
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

	huge_ptr allocate_huge(size_t);
	huge_ptr make_huge();

}


