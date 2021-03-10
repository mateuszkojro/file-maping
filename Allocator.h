#pragma once

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
	template <class T, class U>
	bool operator==(const Allocator<T>&, const Allocator<U>&) { return true; }
	template <class T, class U>
	bool operator!=(const Allocator<T>&, const Allocator<U>&) { return false; }


}

