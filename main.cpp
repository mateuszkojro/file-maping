#define _AMD64_ 
#include <fileapi.h>
#include <iostream>
#include <windows.h>
#include <errhandlingapi.h>
#include <cstdlib>
#include "huge_ptr.h"
#include "Allocator.h"
#include <vector>

#include <unordered_map>

#define _CONTAINER_DEBUG_LEVEL 0;

void show_system_info() {
	SYSTEM_INFO info;

	GetSystemInfo(&info);

	std::cout << "gran: " << info.dwAllocationGranularity << std::endl;
	std::cout << "max: " << info.lpMaximumApplicationAddress << std::endl;
	std::cout << "min: " << info.lpMinimumApplicationAddress << std::endl;
	//std::cout << "Memory avail: " << get_memory_left() << std::endl;
}

// --- FOR DESTRUCTOR ---
// CloseHandle(hFile) - can be used for closing handles
// std::remove("temp_file"); - usuwam plik tymczasowy
// UnmapViewOfFile(this->cur_ptr_); -- usuwam view


// --- TODO ---
// - wywalic nie potrzebne pola klasy
// - dereferencja ptr
// - ->() TF


struct Cos {
	int x;
	int y;
	int z;
};

int main() {

	show_system_info();

	using T = char;

	auto my_ptr = mk::huge_ptr<T>::allocate_huge(3 * sizeof(T));

	my_ptr[0] = 48;//"ala ma kota";
	my_ptr[1] = 49;//"Kot ma aale";
	my_ptr[3] = 50;//"Kot ma aale";

	std::cout << "ptr++\t" << my_ptr[0] << std::endl;
	assert(my_ptr[0] == 48);
	my_ptr[0]++;
	std::cout << "++ptr\t" << ++my_ptr[0] << std::endl;
	assert(my_ptr[0] == 50);

	std::cout << "ptr--\t" << my_ptr[0] << std::endl;
	assert(my_ptr[0] == 50);
	my_ptr[0]--;

	
	std::cout << "--ptr\t" << --my_ptr[0] << std::endl;
	assert(my_ptr[0] == 48);

	std::cout << "*ptr\t" << *my_ptr << std::endl;

	mk::huge_ptr<double> new_ptr = mk::Allocator<double>::allocate(2);
	my_ptr[0] = 48;//"ala ma kota";
	my_ptr[1] = 49;//"Kot ma aale";

	std::cout << "ptr\t" << my_ptr[0] << std::endl;
	std::cout << "ptr\t" << my_ptr[1] << std::endl;


	std::cout << my_ptr - my_ptr << std::endl;

	int* a = new int;
	int* b = new int;

	*a = 2;
	*b = 2;



	std::cout
		<< a << std::endl
		<< b << std::endl
		<< a - b << std::endl;
	system("pause");

	std::vector<int, mk::Allocator<int>> all;// = {1,2,3};

//std::unordered_map<int, int,std::hash<int>, std::equal_to<int>, mk::Allocator<int>> a;

//a[2] = 1;

	int i = 1;

	my_ptr = my_ptr + i;

	//auto ptr1 = a + 2;


	//std::cout <<   << std::endl;


	all.reserve(100);
	//double d = 1;

	for (int i = 0; i < 10; i++) {
		all.push_back(i);
	}


	for (int i = 0; i < 3; i++) {
		std::cout << "I: " << all[i] << std::endl;
	}

	//system("pause");

}