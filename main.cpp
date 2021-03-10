
// !!!! depending on architecture diffrent #define should be implemented
// _X86_  for x32
// _AMD64_ for x64


#define _X86_ // x 32
//#define _AMD64_//  x64



#include <iostream>
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
        x = rand() % 200;
    }

    int get() { return x; };

    test_class(const test_class &other) = default;


    char *hash() const {

        return "dziala";
    }


};


int main() {

    { /// operators ++ --
        auto my_ptr = my::Allocator<double>::allocate(5);
        my_ptr[0] = 0; // entering some random values
        my_ptr[1] = 1;
        my_ptr[2] = 2;
        my_ptr[3] = 3;
        my_ptr[4] = 4;

        // testing operator++()
        assert (*my_ptr == 0);
        my_ptr++;
        assert (*my_ptr == 1);

        assert (*(my_ptr++) == 1);

        assert (*my_ptr == 2);
        printf("operator++()    [ ok ]\n");
        // testing operator--()

        my_ptr--;
        assert (*my_ptr == 1);

        assert (*(my_ptr--) == 1);

        assert (*my_ptr == 0);
        printf("operator--()    [ ok ]\n");

        // testing operator++(int)

        assert (*my_ptr == 0);
        ++my_ptr;
        assert (*my_ptr == 1);

        assert (*(++my_ptr) == 2);
        printf("operator++(int)    [ ok ]\n");

        // testing operator--(int)

        --my_ptr;
        assert (*my_ptr == 1);

        assert (*(--my_ptr) == 0);
        printf("operator--(int)    [ ok ]\n");
    }



    //  show_system_info();


    {

        my::huge_ptr<char> my_ptr = my::Allocator<char>::allocate(2);
        my::huge_ptr<char> my_ptr2 = my::Allocator<char>::allocate(2);

        my_ptr[0] = 'o';
        my_ptr[1] = 'k';

        my_ptr2 = my_ptr;
        assert(my_ptr == my_ptr2);

        my_ptr2++;
        assert(my_ptr != my_ptr2);
        printf("difference between two pointers  [ ok ]\n");
    }

    {
        my::huge_ptr<test_class> my_ptr = my::Allocator<test_class>::allocate(3);


        test_class x;
        *my_ptr = x;

        assert(my_ptr->hash() == (*my_ptr).hash());

        printf("operator ->         [ok] \n");
    }

    {

        // allocator is able to allocate huge amounts of data
        printf("allocating 200 MB\n");
        my::huge_ptr<char> huge_allocation = my::Allocator<char>::allocate(200 MB);

        for (size_t n = 199 MB; n < 200 MB; ++n) { // assigning value n to nth position

            huge_allocation[n] = n;
        }

        for (size_t n = 199 MB; n < 199 MB + 5; ++n) {
            std::cout << "value under n'th position  = " << " value = " << (int) huge_allocation[n] << "\n";
        }
    }

    {
        size_t limit = 900 MB;

        std::cout << "allocating " << limit << " bytes" << std::endl;

        my::huge_ptr<BYTE> huge_allocation = my::Allocator<BYTE>::allocate(limit);


        for (int i = 0; i < 5; i++) {

            huge_allocation[i * (limit / 5)] = i;
        }

        for (int i = 0; i < 5; i++) {
            std::cout << "value under i = " << i * (limit / 5)
                      << " value = " << (int) huge_allocation[i * (limit / 5)]
                      << "\n";
        }



        std::cout << "allocating another " << limit * sizeof(test_class)<< " bytes" << std::endl;

        my::huge_ptr<test_class> another_huge_allocation = my::Allocator<test_class>::allocate(limit);

        for (int i = 0; i < 5; i++) {
            another_huge_allocation[i * (limit / 5)] = test_class();

        }
        for (int i = 0; i < 5; i++) {
            std::cout << another_huge_allocation[i * (limit / 5)].get() << "\n";
        }

    }

    {
        printf("operator =  test\n");
        my::huge_ptr<int> copy_test;
        {

            my::huge_ptr<int> copy_test2 = my::Allocator<int>::allocate(10);
            for (int i = 0; i < 10; i++) copy_test2[i] = i;

            copy_test = copy_test2;

        }
        for (int i = 0; i < 5; i++) {

            std::cout << "copy_test[" << i << "] = " << *copy_test << std::endl;
            copy_test++;
        } // shifting offset
          // next


        for (int i = 0; i < 5; i++) {

            std::cout << "copy_test[" << i << "] = " << copy_test[i] << std::endl;

        }
printf("oprator =       [ ok ]\n");
    }


    system("pause");

    return 0;

}
