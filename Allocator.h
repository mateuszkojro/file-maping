// klasa allocator
// ralizuje funkcje allokatora
// autor Piotr Drabik

#pragma once
#ifndef ALLOCATOR
#define ALLOCATOR

#include "huge_ptr.h"

namespace my {
	template <class Tp>
	class Allocator {
	public:
		static huge_ptr<Tp> allocate(size_t n)
		{

			return huge_ptr<Tp>::allocate_huge(n);
		}

		static void deallocate(huge_ptr<Tp>& p, size_t n)
		{
			p.free();
		}

		static void deallocate(huge_ptr<Tp>& p)
		{
			p.free();
		}
	private:
        Allocator() = default;
        Allocator(const Allocator&) = default;
        Allocator& operator=(const Allocator&) = default;
        ~Allocator() = default;
	};


}

#endif // ALLOCATOR