//#define _X86_
//#define _AMD64_

#include <iostream>
#include <windows.h>

#include <cstdlib>
#include "huge_ptr.h"
#include "Allocator.h"

#include <climits>


#define KB * 1024
#define MB * 1024 * 1024
#define GB * 1024 * 1024 * 1024
#define TB * 1024 * 1024 * 1024 * 1024


void show_system_info() {
    SYSTEM_INFO info;

    GetSystemInfo(&info);

    std::cout << "gran: " << info.dwAllocationGranularity << std::endl;
    std::cout << "max: " << info.lpMaximumApplicationAddress << std::endl;
    std::cout << "min: " << info.lpMinimumApplicationAddress << std::endl;

}


class test_class {
public:
    char x;
    char y;

    test_class() : x(1), y(0) {};

    test_class(char x, char y) : x(x), y(y) {};

    test_class(const test_class &other) = default;

    int hash() const {

        return this->x * 256 + this->y;
    }


};


int main() {


    show_system_info();
    {
        using T = char;

        auto my_ptr = mk::Allocator<T>::allocate(3 * sizeof(T));

        my_ptr[0] = 48;//"ala ma kitten";
        my_ptr[1] = 49;//"Kot ma ale";
        my_ptr[2] = 50;//"Kot ma ale";

        std::cout << "ptr++\t";
        std::cout << my_ptr[0] << std::endl;
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
        my_ptr[0] = 48;
        my_ptr[1] = 49;

        std::cout << "ptr\t" << my_ptr[0] << std::endl;
        std::cout << "ptr\t" << my_ptr[1] << std::endl;


        std::cout << my_ptr - my_ptr << std::endl;


        mk::huge_ptr<test_class> mew_ptr = mk::Allocator<test_class>::allocate(3);


        test_class x;

        mew_ptr[0] = x;

        std::cout << "meow: " << mew_ptr->hash() << std::endl;
        std::cout << "allocating 4GB of memory " << std::endl;


        {
            mk::huge_ptr<char> huge_allocation = mk::Allocator<char>::allocate(20 KB);
            for (int i = 0; i < 20 KB; i++) {

                huge_allocation[i] = i % 256;


            }

            for (int i = 10 KB; i < 10 KB + 10; i++) {
                std::cout << "i = " << i << " value = " << (int) huge_allocation[i] << "\n";


            }

            std::cout << huge_allocation[1024 * 4];


        }
    }
    size_t limit = ULONG_MAX - 1; // ok


    mk::huge_ptr<int> even_more_huge_allocation = mk::Allocator<int>::allocate(limit);

    for (int i = 0; i <16; i++) {
        even_more_huge_allocation[i*(limit/16)] = (int) i;


    }
    for (int i = 0; i <16; i++) {
        std::cout << "i = " << i*(limit/16) << " value = " << even_more_huge_allocation[i*(limit/16)] << "\n";


    }
    mk::huge_ptr<test_class> the_same_more_huge_allocation = mk::Allocator<test_class>::allocate(limit);

    for (int i = 0; i <16; i++) {
        the_same_more_huge_allocation[i*(limit/16)] = test_class(47,48);


    }
    for (int i = 0; i <16; i++) {
        std::cout << the_same_more_huge_allocation[i*(limit/16)].hash() << "\n";


    }
    //FILE_FLAG_RANDOM_ACCESS


    return 0;

}
