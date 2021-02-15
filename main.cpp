#define _AMD64_ 
#include <fileapi.h>
#include <iostream>
#include <windows.h>
#include <errhandlingapi.h>
#include <cstdlib>
#include "huge_ptr.h"
#include "Allocator.h"
#include <vector>


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

int main() {

	show_system_info();

	using T = char;

	auto my_ptr = mk::huge_ptr<T>::allocate_huge(3 * sizeof(T));

	my_ptr[0] = 48;//"ala ma kota";
	my_ptr[1] = 49;//"Kot ma aale";
	my_ptr[3] = 50;//"Kot ma aale";

	std::cout << "ptr++\t" << my_ptr[0]++ << std::endl;
	std::cout << "++ptr\t" << ++my_ptr[0] << std::endl;

	std::cout << "ptr--\t" << my_ptr[0]-- << std::endl;
	std::cout << "--ptr\t" << --my_ptr[0] << std::endl;
	
	std::cout << "*ptr\t" << *my_ptr << std::endl;

	mk::huge_ptr<double> new_ptr = mk::Allocator<double>::allocate(2);
	my_ptr[0] = 48;//"ala ma kota";
	my_ptr[1] = 49;//"Kot ma aale";

	std::cout << "ptr\t" << my_ptr[0] << std::endl;
	std::cout << "ptr\t" << my_ptr[1] << std::endl;

	int* a = new int;
	int* b = new int;

	*a = 2;
	*b = 2;

	std::cout
		<< a << std::endl
		<< b << std::endl
		<< a - b << std::endl;

	std::vector<double, mk::Allocator<double>> data;// = {1,2,3};

	////data.reserve(100);
	data.push_back(1);



	//system("pause");

}