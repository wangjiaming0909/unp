/*************************************************************************
	> File Name: new_allocator.h
	> Author: 
	> Mail: 
	> Created Time: Thu 07 Sep 2017 07:35:31 PM CST
 ************************************************************************/

#ifndef _NEW_ALLOCATOR_H
#define _NEW_ALLOCATOR_H

using std::size_t;
using std::ptrdiff_t;

template <class _Tp>
class new_allocator{
public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef _Tp*        pointer;
    typedef const _Tp*  const_iterator;
    typedef _Tp&        reference;
    typedef const _Tp&  const_reference;
    typedef _Tp         value_type;

    template <typename _Tp1>
    struct rebind{
        typedef new_allocator<_Tp1> other;
    };

#if __cplusplus >= 201103L
    typedef std::true_type propagate_on_container_move_assignment;
#endif

    new_allocator() noexcept{}
    new_allocator(const new_allocator&) noexcept{}
    template <typename _Tp1>
    new_allocator(const new_allocator<_Tp>&) noexcept{}

    ~new_allocator() noexcept{}

    pointer address(reference __x)const noexcept{
        return std::__addressof(__x);
    }
    pointer allocator(size_type __n, const void* = 0){
        if(__n > this->max_size())
            std::__throw_bad_alloc();
        return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
    }

    void deallocaotr(pointer __o, size_type){
        ::operator delete(__p);
    }
    size_type max_size() const noexcept{
        return size_t(-1) / sizeof(_Tp);
    }

#if __cplusplus >= 201103L
    template <typename _Up, typename... _Args>
    void constructor(_Up*, _Args&&... __args){
        ::new ((void *) __p) _Up(std::forward<_Args>...);
    }
    template <typename _Up>
    void destroy(_Up* __p){
        __p->~Up();
    }
#endif
};

#endif
