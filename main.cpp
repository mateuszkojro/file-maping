//#define _X86_
//#define _AMD64_

#include <iostream>
#include <windows.h>
#include <vector>

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
    int x;

    test_class() {
        x = rand()%200;
    }

    int get() { return x; };

    test_class(const test_class &other) = default;


    char *hash() const {

        return "dziala";
    }


};


int main() {


  //  show_system_info();
    {
        using T = int;

        auto my_ptr = mk::Allocator<T>::allocate(5 * sizeof(T));
        my_ptr[0] = -2; //
        my_ptr[1] = -1;
        my_ptr[2] = 0; // 0
        my_ptr[3] = 4; // 1
        my_ptr[4] = 2; // 2



        std::cout << "*ptr  =  " << my_ptr[0] << std::endl;
        std::cout << "ptr++\n";
                    ++my_ptr;
                    my_ptr++;


        std::cout << "*(my_ptr + 1 )  =  " << *my_ptr << std::endl;
        std::cout << "++ptr\t";





        mk::huge_ptr<double> new_ptr = mk::Allocator<double>::allocate(2);
        my_ptr[0] = 48;
        my_ptr[1] = 49;

        std::cout << "ptr\t" << my_ptr[0] << std::endl;
        std::cout << "ptr\t" << my_ptr[1] << std::endl;

        std::cout << "difference  between two pointers : ";

        std::cout << my_ptr - my_ptr << std::endl;


        mk::huge_ptr<test_class> mew_ptr = mk::Allocator<test_class>::allocate(3);


        test_class x;

        mew_ptr[0] = x;

        std::cout << "operator -> : " << mew_ptr->hash() << std::endl;


        {

            // allocator is able to allocate and have access to
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
    size_t limit = (_UI32_MAX / 4) - 16; // ok

    std::cout << "allocating 4GB of memory , "<<limit<<" bytes" << std::endl;

    mk::huge_ptr<int> even_more_huge_allocation = mk::Allocator<int>::allocate(limit);

    for (int i = 0; i < 10; i++) {

        even_more_huge_allocation[i*(limit/10)] = (int) i;
    }

    for (int i = 0; i < 10; i++) {
        std::cout << "i = " << i * (limit / 10) << " value = " << even_more_huge_allocation[i * (limit / 10)] << "\n";
    }

    std::cout   << "allocating 4GB of memory, for the second time, test_class weight: "
                << sizeof(test_class) << " bytes"
                << std::endl;

    mk::huge_ptr<test_class> the_same_more_huge_allocation = mk::Allocator<test_class>::allocate(limit);

    for (int i = 0; i < 10; i++) {
        the_same_more_huge_allocation[i * (limit / 10)] = test_class();
        //the_same_more_huge_allocation[i] = test_class();
    }
    for (int i = 0; i < 5; i++) {
        std::cout << the_same_more_huge_allocation[i * (limit / 10)].get() << "\n";
    }
    the_same_more_huge_allocation.get_offset() = 5;
    for (int i = 0; i < 5; i++) {
        std::cout << the_same_more_huge_allocation->get() << "\n";
        the_same_more_huge_allocation++;
    }





    mk::huge_ptr<int> copy_test;
    {

        mk::huge_ptr<int> copy_test2 = mk::Allocator<int>::allocate(44);
        for (int i = 0; i < 44; i++) copy_test2[i] = i;

        copy_test = copy_test2;

        // deleting copy_test2
    }
    for (int i = 0; i < 11; i++) {

        std::cout << "copy_test["<<i<<"] = " << *copy_test << std::endl;
        copy_test++;
    }
    copy_test.get_offset() = 0;

    for (int i = 11; i < 22; i++) {

        std::cout <<  "copy_test["<<i<<"] = " << copy_test[i] << std::endl;

    }






    //FILE_FLAG_RANDOM_ACCESS

    system("pause");

    return 0;

}
