#pragma once
#define _AMD64_
#include <fileapi.h>
#include <windows.h>
#include <iostream>


// --- zaimplementowac jako alocator --
// https://en.cppreference.com/w/cpp/memory/allocator
// chyba jezeli dodoam jeszcze operacje na ptr minus to moze jakas szansa jest ze to zadziala



namespace mk {
	template <typename T>
	class huge_ptr : std::pointer_traits<T>
	{
	public:
		using type = T;
		using element_type = T;
		using value_type = T;
		using value_type = const  T;
		using pointer = *T
	public:
		// dostep do koljenych adresow jak w zwyklym ptr
		T& operator[](size_t);
		// dereferenece the smart ptr
		T& operator*() { return *cur_ptr_; }
		// call a function on a ptr
		T* operator->();

		T* operator+(size_t ofset) { return &operator[](ofset); }
		//T* operator-(size_t ofset) { return &operator[](-ofset_); }
		T* operator++() { return &operator[](1); }

		//reszta operatorow - casta na boola prownywanie itd.
		//reszta operatorow - casta na boola prownywanie itd.
		// tak jak np w unique_ptr
	
		operator T*() const {
			return cur_ptr_;
		}
		friend huge_ptr allocate_huge(size_t);
	private:
		// the size on the disk
		size_t page_size_;
		// size currently in memory
		size_t allocated_mem_;
		// current ofset 
		size_t ofset_;
		// curent file view ptr
		T* cur_ptr_;
		HANDLE file_handle_;
		HANDLE maped_handle_;
	};
	template<typename T>
	huge_ptr<T> allocate_huge(size_t);
	
	template<typename T>
	huge_ptr<T> make_huge();

	template <typename T>
	class alocator {
	public:
		using value_type = T;
		using pointer = huge_ptr<T>;
		using reference = T&;
		using type = huge_ptr<T>;
		pointer allocate(size_t size) {
			return allocate_huge<T>(size);
		}
		void deallocate(pointer adress, size_t size) {}
	};

}


