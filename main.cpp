#define _AMD64_ 
#include <fileapi.h>
#include <iostream>
#include <windows.h>
#include <errhandlingapi.h>
#include <cstdlib>
#include "huge_ptr.h"
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

int main() {

	show_system_info();

	auto my_ptr = mk::allocate_huge(2 * sizeof(T));

	my_ptr[0] = 1.02;//"ala ma kota";
	my_ptr[1] = 0.0002;//"Kot ma aale";

	//my_ptr[0] = "ala ma kota";
	//my_ptr[1] = "Kot ma aale";

	std::cout << "val: " << my_ptr[0] << std::endl;
	std::cout << "val: " << my_ptr[1] << std::endl;

	system("pause");

}