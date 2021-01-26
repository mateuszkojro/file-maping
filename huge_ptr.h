#pragma once
#define _AMD64_
#include <fileapi.h>
#include <windows.h>
#include <iostream>

namespace mk {

	class huge_ptr
	{
	public:
		// dostep do koljenych adresow jak w zwyklym ptr
		__int8& operator[](size_t);
		// dereferenece the smart ptr
		__int8 operator*();
		// call a function on a ptr
		__int8* operator->();

		//reszta operatorow - casta na boola prownywanie itd.
		// tak jak np w unique_ptr
	
		friend huge_ptr allocate_huge(size_t);
	private:
		// the size on the disk
		size_t page_size_;
		// size currently in memory
		size_t allocated_mem_;
		// current ofset 
		size_t ofset_;
		// curent file view ptr
		__int8* cur_ptr_;
		HANDLE file_handle_;
		HANDLE maped_handle_;
	};

	huge_ptr allocate_huge(size_t);
	huge_ptr make_huge();

}


