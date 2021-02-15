#pragma once

#include "huge_ptr.h"

namespace mk {
    template <class Tp>
    struct Allocator {
        using value_type = Tp;
        using pointer = huge_ptr<Tp>;
        using const_pointer = huge_ptr<Tp>;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;




        typedef Tp value_type;
        typedef huge_ptr<Tp> pointer;
        Allocator() = default;
        template <class T> Allocator(const Allocator<T>&) {}

        static huge_ptr<Tp> allocate(std::size_t n)
        {
            n *= sizeof(Tp);
            std::cout << "allocating " << n << " bytes\n";
            return huge_ptr<Tp>::allocate_huge(n);
        }
        
        static void deallocate(huge_ptr<Tp> p, std::size_t n)
        {
            std::cout << "deallocating " << n * sizeof * p << " bytes\n";
            //::operator delete(p);
        }

        static void deallocate(huge_ptr<Tp> p)
        {
            std::cout << "deallocating \n";
            //::operator delete(p);
        }

    };
    template <class T, class U>
    bool operator==(const Allocator<T>&, const Allocator<U>&) { return true; }
    template <class T, class U>
    bool operator!=(const Allocator<T>&, const Allocator<U>&) { return false; }


}

