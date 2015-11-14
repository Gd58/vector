#ifndef __IOSTREAM__
#define __IOSTREAM__
#include<iostream>
#endif
#ifndef _GD_VECTOR__
#define __GD_VECTOR__
#ifndef __G_FUNC__
#include "g_func.h"
#endif
#ifndef __ALLOCATOR__
#include "allocator.h"
#endif


template<class T,class Allo>
/*
	here because iterator of the vector is same like pointer so we donnot need to reconstrcut a iterator

*/
//the iterator of the vector is like common pointer
class Vector{ 
public:
	typedef typename T value_type;
	typedef typename value_type& reference;
	typedef typename value_type* iterator;
	typedef typename const value_type* const_iterator;
	typedef typename size_t size_type;
	typedef typename ptrdiff_t difference_type;
	typedef typename value_type* pointer;

protected:
	typedef typename __allocator<T,Allo> allocator_t;
	iterator begin;//represent the begin of the vector object
	iterator finish;//represent the iterator of the last data
	iterator end;//represent the iterator of the last data of the vector
	
	
	
public:
	Vector():begin(0), finish(0), end(0){};
	Vector(size_type n, const value_type value){ fill_initialized(n, value); }
	Vector(int n, const value_type value)		{ fill_initialized(n, value); }
	Vector(long n, const value_type value)		{ fill_initialized(n, value); }
	explicit Vector(size_type n)				{ fill_initialized(n, value_type()); }
	~Vector(){ destory(begin, finish); deallocator(); }
	iterator begin_(){ return begin;}

	iterator end_(){ return finish; }
	
	reference front(){ return *begin_(); }
	reference back(){ return *(end_() - 1); }
	/* can return the last iterator*/
	void __fill(iterator begin, iterator end, const value_type& x){
		while (begin != end){
			*begin = x;
			++begin;
		}
	}
	iterator __copy(const iterator& begin,const iterator& end, iterator start){
		
		iterator _begin = begin;
		iterator _end = end;
		if (_begin != _end){
			while (_begin != _end){
				*start++ = *_begin++;
				
			}
		}
		return start;
	}
	/*return the start of it*/
	iterator __copy_backward(const iterator begin, const iterator end, iterator result){
		iterator _begin = begin;
		iterator _end = end;
		if (_begin != _end){
			while (_end != _begin){
				*result-- = *_end--;
			}
		}
		return result;
	}
	void pop_back(){ --finish; destory(finish); }//note finish is a pointer so it called destory(T*);
	void deallocator(){
		if (begin){
			allocator_t::deallocate(begin, (size_t)(end - begin));
		}
	}
	iterator allocate_and_fill(size_type n, const T& x){
		iterator result = (int*)allocator_t::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}
	void fill_initialized(size_type n, const T& x){
		begin = allocate_and_fill(n, x);
		finish = begin + n;
		end = finish;
	}

	//this return the size of the max capacity of the contain 
	size_type capacity(){ return end - begin; }
	//there will be returning the size of the used contain
	size_type size(){ return size_type(finish - begin); }
	void resize(size_type sz,const value_type& value){
		if (sz < size()){
			erase(begin_() + sz, end_());
		}
		else{
			insert(end_(),sz - size(),value);
		}
	}
	void resize(size_type sz){
		return resize(sz, T());
	}
	
	void clear(){ destory(begin_(), end_()); }
	reference operator[](size_type value){
		return *(begin_() + value);
	}
	template <class Element_type,class Alloc>
	Vector(const Vector<Element_type,Alloc>& lhs){
		object_copy(lhs);
	}

	template<class Element_type,class Alloc>
	Vector<T, Allo>& operator=(const Vector<Element_type,Alloc>& lhs){
		object_copy(lhs);
	}
	template<class Element_type,class Alloc>
	void object_copy(const typename std::enable_if<std::is_same<Element_type, T>::value
		&&std::is_same<Alloc, Allo>::value, Vector<Element_type, Alloc> >::type& lhs){
		size_t total_sizes = lhs.size();
		if (total_sizes > capacity()){
			destory(begin, finish); deallocator();
			begin = allocator_t::allocate(2 * total_sizes);
			finish = begin + total_sizes;
			end = begin + 2 * total_sizes;
			
			
		}
		__copy(lhs.begin_(), lhs.end_(), begin);

	}
	void push_back(const value_type& value){
		if (finish != end){
			construct(finish, value);
			++finish;
		}
		else{
			insert_aux(end_(), value);
		}
	}
	iterator insert_aux(iterator position, value_type value){
		if (finish != end){
			construct(finish, value);
			++finish;
			value_type x = value;
			__copy_backward(position, finish - 2, finish - 1);
			*position = value;
		}
		else{
			size_t total_sz = (size() == 0 ? 1 : 2 * size());
			iterator newstart = (int*)allocator_t::allocate(total_sz);
			iterator newfinish = newstart;
			newfinish = uninitialized_copy_x(begin, finish, newstart);
			construct(newfinish, value);
			++newfinish;
			destory(begin, finish);
			deallocator();
			begin = newstart;
			finish = newfinish;
			end = begin + total_sz;
		}
		return position;
	}
	iterator insert(iterator position, value_type value){
		return insert_aux(position, value);
	}
	
	iterator erase(iterator position){
		if (position + 1 != end){
			__copy(position + 1,finish,position);
			--finish;
			destory(finish);

		}
		return position;
	}
	iterator erase(iterator new_start, iterator new_end){
		iterator it = __copy(new_end, finish, new_start);
		destory(it, finish);
		finish = finish - (new_end - new_start);
		return new_start;
	}
	void insert(iterator position, size_type n, const T& x){
		iterator new_position = nullptr;
		if (n != 0){
			/*Here ullstration enough*/
			size_type element = position - begin;
			new_position = position;
			if (size_type(end - finish) < n){
				size_t len = size();
				size_t total_sz = 2 * (capacity() > n ? capacity() : n);
				iterator new_start =(int*)allocator_t::allocate(total_sz);
				iterator new_finish = uninitialized_copy_x(begin, finish, new_start);
				new_position = new_start + element;
				destory(begin, finish);
				deallocator();
				begin = new_start;
				finish = new_start + len;
				end = new_start + total_sz;
			}
			__copy_backward(new_position-1, finish, finish + n);
			finish += n;
			__fill(new_position, new_position + n, x);
			/*Here ullstation not enough*/
		}
	}
	
};


#endif