#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <memory>

#include "alloc_traits.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits, typename... alloc_policies>
class allocator: public alloc_policies...
{
public:

    static_assert(sizeof...(alloc_policies) > 0,
                  "Allocator needs at least one allocation policy");

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::reference reference;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::const_reference const_reference;
    typedef typename alloc_traits::size_type size_type;
    typedef typename alloc_traits::difference_type difference_type;

    template <typename U>
    struct rebind
    {
        typedef allocator<
                          U,
                          typename alloc_traits::template rebind<U>,
                          typename alloc_policies::template rebind<U>...
                         > other;
    };

    template <typename U>
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;

    pointer address(reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    const_pointer address(const_reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
    {
        return allocate_helper<alloc_policies...>::allocate(*this, n, nullptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        return allocate_helper<alloc_policies...>::deallocate(*this, ptr, n);
    }

    size_type max_size() const noexcept
    {
        return alloc_traits::max_size();
    }

    template <typename U, typename... Args>
    void construct(rebind_pointer<U> ptr, Args&&... args)
    {
        alloc_traits::construct(ptr, std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(rebind_pointer<U> ptr)
    {
        alloc_traits::destroy(ptr);
    }

private:

    template <typename... policies>
    struct allocate_helper
    {
        static pointer allocate(allocator& alloc,
                                size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint);

        static void deallocate(allocator& alloc,
                               const pointer& ptr, size_type n);
    };

    template <typename policy>
    struct allocate_helper<policy>
    {
        static pointer allocate(allocator& alloc,
                                size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint)
        {
            return alloc.policy::allocate(n, ptr, hint);
        }

        static void deallocate(allocator& alloc,
                               const pointer& ptr, size_type n)
        {
            alloc.policy::deallocate(ptr, n);
        }
    };

    template <typename policy, typename... policies>
    struct allocate_helper<policy, policies...>
    {
        static pointer allocate(allocator& alloc,
                                size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint)
        {
            pointer new_ptr = alloc.policy::allocate(n, ptr, hint);
            return allocate_helper<policies...>::allocate(alloc, n, new_ptr, hint);
        }

        static void deallocate(allocator& alloc,
                               const pointer& ptr, size_type n)
        {
            alloc.policy::deallocate(ptr, n);
            allocate_helper<policies...>::deallocate(alloc, ptr, n);
        }
    };

};

}

#endif // ALLOCATOR_H