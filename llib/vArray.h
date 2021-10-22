// vArray.h: 
//	
//			class varray <class T, class A = std::allocator<T>>   : a resemblance of STL vector<>
//				General Notes:
//					- container for elements of type T, ...
//					- 'size': # of active elements ::= [0, 'capacity') ; 'capacity' current storage size ::= [0, ... 
//					- all elements in [size, capacity) are initialized with T() (CURRENTLY FOR TESTING PURPOSES)
//					  NB: T() should, preferably, NOT need memory releases as it will not be taken care of
//					- ...             [0, size) are initialized after an operation with user defined value<T>
//                    POSSIBLE Optimization for SPEED: FORGET ABOUT T() initialization except explicitly requested
//					NB: In case the empty storage is no longer being initialized with T() functions like push_back() must be updated accordingly
//			    NB: - the family of functions unini_*() acts on NOT INITIALIZED elements, ie NO care is taken for releasing resources
//					- the ...                 over_*()  acts on CONSTRUCTED elements: ie resources ARE taken care of
//					- elements [_spa, _lim) initialized are currently initialized (and maintained that way) with T() for testing purposes mostly
//                  - Influenced funstions: all initialization, changing size, capacity, etc.
// 
//				Interface:
//					- default Constructor: create varray of size num and initialize its elements with a value, use allocator
//					  might also reserve additional storage space
//    ver 0.2       - initializer_list Constructor
//					- COPY operations
//					- MOVE operations
//					- size(): returns the size within the current storage capacity::=capacity()
//					- T* begin/end() family to support RFL
//					- reserve(new_capacity): reserve new storage capacity BUT does NOT Decrease the allocated space: use resize() ; shrink_to_fit() 
//					- resize(new_size, T filler): change the size (reallocate new storage capacity if needed), 
//							'new' elements are to be initialized with 'filler', redundant ones destroyed to T()
//					- push_back(): <as in vector> add new element (at the end). Increase capacity, if necessary as (empty() ? 8 : capacity() * 2)
//					- pop_back(): differs from <vector>: erases the last one, BUT returns again true if on empty. If an exception occured
//                                because of ~T() or initialization of not used storage, etc BUT AGAIN will just re-throw and return false
//                              AGAIN: possible optimization after Testing - skip the initialization of the NOT-USED storage
//					- back(): differs from vector<>: doesnot throw; returns T() if empty(), the last element's (MOVABLE) copy otherwise
//    ver 0.2		- front(): ...                                                              1st ...
//					- shrink_to_fit(): release the access storage
//					- [const] T& operator [i] const : throws out_of_range if (i >= size())
//    ver 0.2		- member types ::iterator -> Random Access Iterators (class iterator_RA<T> defined in "class_RAI.h")
//                                 ::const_iterator -> ...                type const_iterator_RA<T> ...
//					- friend operator <<(): output to ostream&. For testing purposes outputs all, ie incl. the not used storage
//    ver 0.3		- swap(& va1, & va2) ; va1.swap(& va2): swap : noexception is expected  
//    ver 0.3		- erase(pos), erase(from, to)
//    ver 0.3		- insert(): as in STL vector; additional insert_at(ind, from, to)
// 
//			struct VA_base: private member of varray: designed for use within varray:: only
//				General Notes:
//					- handle for the memory slots (base layer for memory related operation)
//					- no <T> related operations aiming for exception safety
//					- Only MOVE Operations defined
//
// 

#ifndef OWN_ARRAY_OWN_ARRAY_OWN
#define OWN_ARRAY_OWN_ARRAY_OWN

#include <iostream>
#include <iomanip>
#include <utility>
#include <memory>
#include <stdexcept>

using std::allocator ;
using std::cout ;
using std::endl ;
using std::move ;
using std::cerr ;
using std::exception ;

#define _TEST_VARRAY_TEST      // Triggers ON initialization of empty storage with T()

// struct VA_base

template <class T, class A = allocator<T>>
struct VA_base    {				// Acts as a memory handle for varray

		A	_all ;
		T*	_elems  ;
		T*	_lim  ;
		T*	_spa ;

		explicit VA_base(A a = A(), typename A::size_type size = 0) : _all{a}, _elems{a.allocate(size)}, 
																	  _lim { _elems + size }, _spa { _elems } {}
																	// { cout << endl << "___ VA_base for " << size << " at (" << size_t(_elems) << ')'; }
		virtual  ~VA_base() noexcept { 
					// cout << endl << "___  ~ VA_base ..." ;
					_all.deallocate(_elems, _lim - _elems); _elems = _lim = _spa = nullptr ; 
				 }

		VA_base(const VA_base& vb) = delete ;
		VA_base& operator =(const VA_base& vb) = delete ;

		VA_base  (VA_base&& vb) noexcept ;				// Move C.
		VA_base& operator =(VA_base&& vb) noexcept ;	// Move A.
}; // struct VA_base

// ---------- class VA_base definitions:

template <class T, class A> inline
VA_base<T, A>::VA_base(VA_base&& vb) noexcept : _all { vb._all }, _elems { vb._elems }, _lim { vb._lim }, _spa { vb._spa }
{
														// cout << endl << "___ VA_base && ..." ;
	vb._elems = vb._lim = vb._spa = nullptr ;
} // VA_base::VA_base(&&)

template <class T, class A> inline VA_base<T, A> &
VA_base<T, A>::operator =(VA_base&& vb) noexcept
{
														// cout << endl << "___ VA_base = && ..." ;
	if (this != &vb) {
		// -> vb aka *this is to be destroyed at the end of the scope
		std::swap(_all, vb._all), std::swap(_elems, vb._elems), std::swap(_lim, vb._lim), std::swap(_spa, vb._spa) ;
	}
	return(*this) ;
} // VA_base::operator=(&&) 

// eoclass VA_base

#include "class_RAI.h"			// Defines RandomAccessIterator class

// ------------ class varray
template <class T, class A = allocator<T>>
class varray {
	public:
		using	iterator = typename iterator_RA<T> ;
		using	const_iterator = typename iterator_RA<const T> ;

	private:
		using  _PtrT = T * ;
		using  _RefT = T & ;
		template <typename Iter> using Diff_iter_type = typename std::iterator_traits<Iter>::difference_type ;

		struct VA_base<T,A>		m_base ;

		void destroy_elements(_PtrT from, _PtrT to) ;
		bool _insert_space(size_t ind, size_t num_ins) noexcept ; // Inserts space of size num_ins at [ind]: LEAVES the Object in ILLEGAL STATE
	
	public:
		explicit varray (size_t num = 0, const T& val = T(), size_t cap = 0, A a = A()) ;	// default C.
		explicit varray (std::initializer_list<T>, A a = A()) ;								// initializer_list C.
		virtual ~varray () ;

		varray(const varray& va) ;               // Copy C.
		varray& operator = (const varray& va) ;  // Copy A.
		void swap(varray<T,A>& va) noexcept ;

		varray(varray&& va) noexcept : m_base {move(va.m_base)} { /*cout << endl << "___ varray && ..." ;*/ }	// Move C.
		varray& operator =(varray&& va) noexcept ;																// Move A.

		size_t size() const { return(m_base._spa - m_base._elems) ; }
		size_t capacity() const { return(m_base._lim - m_base._elems) ; }
		bool   empty() const { return(size() == 0) ; }

		bool reserve(size_t cap) noexcept ;  					// increase the capacity
		bool resize(size_t size, const T& val = T()) noexcept ; // Change the size, initialize with val, if needed, upto _lim
		void clear() { resize(0) ; } 
		void shrink_to_fit() ;

		bool erase(iterator pos) noexcept		{ return(erase(pos, pos + 1)) ; }
		bool erase(iterator from, iterator to) noexcept ;

		iterator	insert(iterator pos, const T& val = T()) ;
		iterator	insert(iterator pos, T&& val = T()) ;
		iterator	insert(iterator pos, size_t num, const T& val = T()) ;
		template <class InputIterator> bool insert_at(size_t ind, InputIterator from, InputIterator to) ;
		template <class InputIterator> bool insert   (iterator pos, InputIterator from, InputIterator to) ;

		
		bool push_back(const T& val) ;
		bool push_back(T&& val) ;

		T	 back() const noexcept ;
		T	 front() const noexcept ;
		bool pop_back() ;

		_RefT		operator [] (const size_t ind) ;

		iterator		begin() const { return(static_cast<iterator>(m_base._elems)) ; }
		iterator		end() const { return(static_cast<iterator>(m_base._spa)) ; }
		iterator		endlim() const { return(static_cast<iterator>(m_base._lim)) ; }
		const_iterator cbegin() const { return(static_cast<const_iterator>(m_base._elems)) ; }
		const_iterator	cend() const { return(static_cast<const_iterator>(m_base._spa)) ; }
		const_iterator	cendlim() const { return(static_cast<const_iterator>(m_base._lim)) ; }

		template <class T, class A> friend void swap(varray<T,A>& v1, varray<T,A>& v2) noexcept ; 
		template <class T, class A> friend std::ostream& operator << (std::ostream& os, const varray& ar) ;
}; // class varray

// Support function templates uini_*() and over_*()

template <class InpIt, class T> void
unini_cpy(InpIt beg, InpIt end, T* ooo)	// Expected to provide Strong guarantee; (&* is for iterators);
{				             							// does NOT call a Destructor for the target elements
	
	auto q = ooo ;										// InpIt p = beg ; for ERROR handling and Testing: "cout << i << ':'" as well
	try {
		for (; beg != end ; ++beg, ++ooo) {
														// if (beg - p == 4)   throw (CM_error(string("__ testing CM_error"))) ;
														// cout << (beg - p) << ':',
			new (static_cast<void *>(&*ooo)) T (*beg) ; 
		}
	} catch (...) {
		cerr << endl << "__ exception thrown -> cleaning: " ;
		for (size_t i = 0 ; q != ooo ; ++q, i++)	((q))->~T(), cout << i << ':' ; // skip (&(*q)): used just with T*
		throw ;
	}
} // unini_cpy()

template <class InpIt, class T> void
unini_move(InpIt beg, InpIt end, T* ooo)	// does NOT call a Destructor for the target elements
{
														// cout << endl << "__ moving: " ;
														//InpIt p = beg ; auto q = ooo ; for ERROR handling and Testing: "cout << i << ':'" as well
	try {
		for (; beg != end ; ++beg, ++ooo) {
														// if (beg - p == 4)   throw (CM_error(string("__ testing CM_error"))) ;
														// cout << (beg - p) << ':',
			new (static_cast<void *>(&*ooo)) T (move(*beg)) ;
			beg->~T() ;   // Just in case ...
		}
	} catch (...) {
		cerr << endl << "__ exception thrown during moving: can not clean" ;
		throw ;
	}
} // unini_move()

template <class OutIt, class T> void
over_chng(OutIt beg, OutIt end, const T& val)				// Calls a destructor for the target elements
{
														// cout << endl << "__ changing: " ;
														// OutIt p = beg ;    for ERROR handling and Testing: "cout << i << ':'" as well
	try {
		for (; beg != end ; ++beg) {
														// if (beg - p == 4)   throw (CM_error(string("__ testing CM_error"))) ;
			beg->~T(),
														// cout << (beg - p) << ':',
				new (static_cast<void *>(&*beg)) T (val) ;
		}
	} catch (...) {
		cerr << endl << "__ exception thrown during changing: can not clean" ;
		throw ;
	}
} // over_chng()

template <typename Out, typename T> void
over_mval(Out beg, T&& val)					// Calls a destructor for the target elements
{											// cout << endl << "__ changing with &&: " ;													
	try {
		beg->~T() ;
		new (static_cast<void *>(&*beg)) T (move(val)) ;
	} catch (...) {
		cerr << endl << "__ exception thrown during moving value: can not clean" ;
		throw ;
	}
} // over_mval()

// ---------- class varray

template <class T, class A> inline void
varray<T, A>::destroy_elements(_PtrT from, _PtrT to)
{															// cout << "___ varray : destroying elements" ;												
	for (; from < to ; ++from)				(from)->~T() ;	// (&*from)->~T() : NO NEED just simple (T *) iterators used
} // varray::destroy_elements() 

template <class T, class A>
varray<T, A>::~varray()
{															// cout << endl << "___  ~ varray - " ;
	destroy_elements(m_base._elems, m_base._spa) ;			// Clean [0 : size()
	
#ifdef _TEST_VARRAY_TEST
	destroy_elements(m_base._spa, m_base._lim) ;            // Clean the initialized and not used storage as for TESTING they were set to T()
#endif
	m_base._spa = m_base._elems ;
} // varray:: ~

template <class T, class A>
varray<T, A>::varray(size_t num, const T& val, size_t cap, A a) : m_base(a, (cap < num ? (cap = num + 8) : cap))
{
												// cout << endl << "___ varray (" << num << ", " ;
												// ??? 'val' ???
												// cout << "<DTy= " << typeid(Data_type<decltype(m_base._elems)>).name() << ">" ;
												// cout << ")/cap(): " << cap ;
	try {
		std::uninitialized_fill(m_base._elems, m_base._elems + num, val) ;  // See VS docs
#ifdef _TEST_VARRAY_TEST
		std::uninitialized_fill(m_base._elems + num, m_base._lim, T()) ;	// Initialize rest of the storage with the <empty>: For testing mostly
#endif
	} catch (...) { cerr << endl << "___ varray (): exception thrown -> rethrown" ; throw ; }
	m_base._spa = m_base._elems + num ;									    // _lim is just where it has to be: next spot after the end
} // varray:: ()

template <class T, class A>
varray<T, A>::varray(std::initializer_list<T> ilist, A a) : m_base(a, ilist.size())
{
												// cout << endl << "___ varray {}/cap(): " << ilist.size() ;
	unini_cpy(ilist.begin(), (ilist.end()), m_base._elems) ;     // Does NOT call a destructor for the target
	m_base._spa = m_base._elems + ilist.size() ; // _lim is already set 
} // varray (initializer_list)

template <class T, class A>
varray<T, A>::varray(const varray& var) : m_base(var.m_base._all, var.capacity())
{
												// cout << endl << "___ varray & ..." ;
	m_base._spa = m_base._elems + var.size() ;
	unini_cpy(var.m_base._elems, var.m_base._lim, m_base._elems) ; // Copy all of them - up to the limit: fully identical
} // varray (&)


template <class T, class A> varray<T, A>&
varray<T, A>:: operator =(const varray& var)
{
												// cout << endl << "___ varray = & ..." ;
	if (this != &var) {
		varray<T, A>  temp(var) ;      // 'var' should provide destruction of the old *this      
		m_base = move(temp.m_base) ;   // As per operator =() for m_base it is a swap, actually
	}
	return(*this) ;
} // varray operator =(&)

template <class T, class A> varray<T, A> &
varray<T, A>::operator =(varray&& var) noexcept
{
												// cout << endl << "___ varray = && ..." ;
	if (this != &var) {
		clear() ;					  // Caring about unproper use of std::move() somewhere
		m_base = move(var.m_base) ;   // As per operator =() for m_base it is a swap, actually
	}
	return(*this) ;
} // varray operator =(&&)


template <class T, class A> bool
varray<T, A>::reserve(size_t new_cap) noexcept
{
	if (new_cap <= capacity())               return(true) ; // >>>: do not decrease allocated space

	// PLAN A (very exception-safe):
	//  - create a new base and COPY elements from the old
	//  - if no exception thrown - destroy the old one
	// PLAN B (less exception safe): >>>: MOVE Operations shoud NOT throw
	//  - create a new base and if OK MOVE elements from the old
	//  - if no exception thrown - destroy the old one. Otherwise keep what's left
	// -> GO for PLAN B

	try {
		VA_base<T, A>		tb(m_base._all, new_cap) ;
		tb._spa = tb._elems + size() ;							// Adjust to match this->size(); Then (for TESTING) initialize to _lim
#ifdef _TEST_VARRAY_TEST
		std::uninitialized_fill(tb._spa, tb._lim, T()) ;		// In case of an eception, Nothing has been damaged so far
#endif
		unini_move(m_base._elems, m_base._spa, tb._elems) ;		// Now, it is what it is
		// Only need to swap (The allocator has to be the same); 'tb' must be destroyed at EOS
		std::swap(m_base._elems, tb._elems) , std::swap(m_base._spa, tb._spa) , std::swap(m_base._lim, tb._lim) ;
	} catch (...) {
		cerr << endl << "___ (varray): reserve() -> exception -> ..." ;
		return(false) ;
	}
	return(true) ;
} // varray reserve()


template <class T, class A> bool
varray<T, A>::resize (size_t ns, const T& val) noexcept
{
					size_t	arr_size = size() ;

	if (ns == arr_size)				return(true) ;  // Nothing to do
	if (!reserve(ns))				return(false) ; // Will attempt smthg only if (ns > capacity)

	// Now _lim and _elems are, hopefully, where they should be
	try {
		if (ns < arr_size) {									// need to destroy some and
#ifdef _TEST_VARRAY_TEST
			over_chng(m_base._elems + ns, m_base._spa, T()) ;	// if TESTING: set them to T() - the rest are expected to be T()
#else
			destroy_elements(m_base._elems + ns, m_base._spa) ;
#endif
			m_base._spa = m_base._elems + ns ;                
		} else if (ns > size()) {
			std::uninitialized_fill(m_base._spa, m_base._elems + ns, val) , m_base._spa = m_base._elems + ns ;
		}
	} catch (...) {
		cerr << endl << "___ (varray): resize() -> exception -> ..." ;
		return(false) ;
	}

	return(true) ;
} // varray resize()

template <class T, class A> void
varray<T, A>::shrink_to_fit()
{
	VA_base<T,A>	tb(m_base._all, size()) ;
	try {
		unini_move(m_base._elems, m_base._spa, tb._elems) ;  // should take care of destruction of source elements
		tb._spa = tb._elems + size() ;
		// -> tb aka *this is to be destroyed at the end of the scope
		std::swap(m_base._elems, tb._elems), std::swap(m_base._lim, tb._lim), std::swap(m_base._spa, tb._spa) ;
	} catch (...) {
		cerr << endl << "___ (varray): shrink_to_fit -> exception -> ..." ;
	}
} // varray shrink_to_fit()

template <class T, class A> bool
varray<T, A>::erase(iterator from, iterator to) noexcept		// T's MOVE operations must comply with the standard
{		
					auto lim = end() ;
	// Do NOT allow parameters outside limits
	if (from >= to || from < begin() || from >= end() || to > end() || to <= begin())      return(false) ;
	
	for ( ; to < lim ; ++to, ++from)      {					// *from = move(*to) ;  // MOVE operations must NOT throw. So does resize()
		(&(*from))->~T() ,
		new (static_cast<void *>(&*from)) T(move(*to)) ;	// as: *from = move(*to) will strongly depend on T operations
	}
	return(resize(size() - (lim - from))) ;   // Should clean all remaining, if any, and do the necessary initialization
} // varray erase()

template <class T, class A> bool
varray<T, A>::_insert_space(size_t ind, size_t num_ins)  noexcept // move() should NOT throw; LEAVES the Object in ILLEGAL STATE
{
	size_t   nel = size() ;  // assumed that 'ind' is OK: in [0, size()] and 'num_ins' > 0
															// cout << endl << endl << "___  (insert space): " << num_ins 
															// << " elements at [" << ind << "] of {used " << nel << " of " << capacity() << "}" ;
	if (nel + num_ins > capacity())		{ 
		if (!reserve(nel + num_ins))   return(false) ; 
															// cout << endl << "___ capacity changed to: " << capacity() ;
	}

	auto   st = m_base._spa ;     
	auto   last = st + num_ins ;

#ifdef _TEST_VARRAY_TEST
	// We are to: either move elements to empty storage or, appending. So, Destroy the initialized elements in the not-used storage
	destroy_elements(st, last) ;      // [0 : _lim)
															// cout << endl << "___ " << num_ins << " elements in the empty storage cleared" ;
#endif
	try {
		if (ind < nel) {		// Free space for num_ins elements starting at ind, ie move backwards (nel-ind)s from [nel-1] to [nel+num_ins-1]
			--st, --last ;		// To positions
															// cout << endl << "___ moving backwards: " << (nel - ind) << " from " 
															//      << (st - begin()) << " to " << (last - begin()) ;

			for (auto i = nel - ind ; i > 0 ; i--, --st, --last) {    // Reverse move to free the space
				new (static_cast<void *>(&*last)) T (move(*st)) ;
				st->~T() ; // Just in case and simple (T *) iterators:  (&*st)->~T() ;
			}
		} else { /*cout << endl << "___ to append: " << num_ins << " at " << nel ;*/ }    // Append: ie insert into the free storage
	} catch (std::exception& e) { 
		cerr << endl << "___ (varray)->_insert_space: STL exception: " << e.what() 
				<< " of Type: " << typeid(e).name() ;
		return(false) ;
	} catch (...) { cerr << endl << "___ (varray)->_insert_space: unknown exception" ; return(false) ; }
	return(true) ;
} // varray _isert_space()

template <class T, class A> iterator_RA<T>
varray<T, A>::insert(iterator pos, const T& val)
{
	return(insert(pos, 1, val)) ;
} // varray insert( T& )

template <class T, class A> iterator_RA<T>
varray<T, A>::insert(iterator pos, T&& val)
{
	if (pos < begin() || pos > end())   return(end() + 1) ;	 // indicate failure
	Diff_iter_type<iterator>   ind = pos - begin() ;
	if (_insert_space(ind, 1))		{							// cout << endl << "___ inserting &&: at position " << ind ;
		auto   st = begin() + ind ;
		try {
			new (static_cast<void *>(&*st))  T(move( val)) ; // All object have to be destroyed by _insert_space
		} catch (...) { cerr << endl << "___ (varray)-> insert (&&) exception in new (move): (re-thrown)" ; throw ; }
		m_base._spa++ ;
		return(st) ;
	}
	return(end() + 1) ;    // indicate failure
} // varray insert(&&)

template <class T, class A> iterator_RA<T>
varray<T, A>::insert(iterator pos, size_t num_ins, const T& val)
{
	if (pos < begin() || pos > end() || num_ins == 0)   return(end() + 1) ;    // indicate failure

	Diff_iter_type<iterator>   ind = pos - begin() ;
	if (_insert_space(ind, num_ins))   {					// cout << endl << "___ filling: " << num_ins << " from position " << ind << " onwards" ;
		m_base._spa += num_ins ;   // there is the array with a hole in it
		try {
			std::uninitialized_fill(m_base._elems + ind, m_base._elems + ind + num_ins, val) ;  // All object have to be destroyed by _insert_space
		} catch (...) { cerr << endl << "___ (varray)-> exception in uninitialized_fill(): (re-thrown)" ; throw ; }
		return(begin() + ind) ;
	}
	return(end() + 1) ;    // indicate failure
} // varray insert(count)


template <class T, class A>
template <class InputIterator> bool
varray<T, A>::insert_at(size_t ind, InputIterator from, InputIterator to)
{
	size_t   nel = size() ;
	if (ind > nel || from >= to)           return(false) ;

	Diff_iter_type<InputIterator>   num_ins = (to - from) ;
	if (_insert_space(ind, num_ins)) {						// Copy the input into the inserted space that starts at [ind]
															// cout << "___ copying: " << num_ins << " from position " << ind << " onwards" ;
		try {
			unini_cpy(from, to, m_base._elems + ind) ; // go to simple instead of begin() + ind) ;
		} catch (...) { cerr << endl << "___ (varray)-> exception in unini_cpy(): (re-thrown)" ; throw ; }
		m_base._spa += num_ins ;
		return(true) ;
	}
	return(false) ;
} // varray insert_at()

template <class T, class A>
template <class InputIterator> bool
varray<T, A>::insert(iterator pos,  InputIterator from, InputIterator to)
{
	return(insert_at(pos - begin(), from, to)) ;
} // varray insert()

template <class T, class A> bool
varray<T,A>::push_back(const T& val)
{
					size_t			arr_size = size() ;

	if (arr_size == capacity())		{ if (!reserve(arr_size ? arr_size * 2 : 8))    return(false) ; }

	try {
	over_chng(m_base._spa, m_base._spa + 1, val) ;
	} catch (...)   {  
		cerr << endl << "___ (varray): push_back(&) -> exception -> ..." ;
		// The only discrepancy is that *_spa might not be {}
		return(false) ;
	}  
	(m_base._spa) ++ ;
	return(true) ;
} // varray push_back(&)

template <class T, class A> bool
varray<T, A>::push_back(T&& val)
{
	size_t			arr_size = size() ;

	if (arr_size == capacity()) { if (!reserve(arr_size ? arr_size * 2 : 8))    return(false) ; }

	try {
		over_mval(m_base._spa, move(val)) ;
	} catch (...) {
		cerr << endl << "___ (varray): push_back(&&) -> exception -> ..." ;
		// The only discrepancy is that *_spa might not be {}
		return(false) ;
	}
	(m_base._spa)++ ;
	return(true) ;
} // varray push_back(&&)

template <class T, class A> bool
varray<T, A>::pop_back()
{
	if (empty())    return(false) ;    // Logic is: did not remove/change anything
	try {
#ifdef _TEST_VARRAY_TEST
		over_mval(m_base._spa - 1, move(T ())) ;
#else
		(m_base._spa - 1)->~T() ;
#endif
	} catch (...) {
		cerr << endl << "__ (varray): pop_back() -> exception -> ..." ;
		throw ;
	}
	(m_base._spa)-- ;
	return(true) ;
} // varray pop_back()

template <class T, class A> T
varray<T, A>::back() const noexcept
{
	return(empty() ? T() : *(m_base._spa - 1)) ;	
} // varray back()

template <class T, class A> T
varray<T, A>::front() const noexcept
{
	return(empty() ? T () : *(m_base._elems)) ;
} // varray front()


template <class T, class A> inline T&
varray<T,A>::operator [](const size_t i)
{
	if (i < size())      return(*(m_base._elems + i)) ;
	throw			std::out_of_range("___ (varray): subscript out of range") ;
} // varray operator[]()


template <class T, class A> inline void
varray<T, A>::swap(varray& v2) noexcept
{
	VA_base<T, A> * p2 = &(v2.m_base) ;
	std::swap(m_base._all, p2->_all), std::swap(m_base._elems, p2->_elems),
	std::swap(m_base._spa, p2->_spa), std::swap(m_base._lim, p2->_lim) ;
} // varray swap()

// FRIENDS follow
template <class T, class A> inline void
swap(varray<T, A>& v1, varray<T, A>& v2) noexcept
{
	v1.swap(v2) ;
} // friend varray swap()

constexpr int ElementsPerRow { 13 } ;    // For the output: ostream:
constexpr int SpacePerElement { 4 } ;    // ... # of ' '

template <typename Iter>
using Data_type = typename std::iterator_traits<Iter>::value_type ;

template <class T, class A> std::ostream&
operator << (std::ostream& os, const varray<T, A> & ar)
{
	os << "varray<" << typeid(Data_type<decltype(cbegin(ar))>).name() << "> is:" 
		<< endl << "{used " << ar.size() << " of " << ar.capacity() << '}' << '\n' ;
	if (ar.size() == 0) { os << "<EMPTY>" ; }

	size_t i = 0 ;
	for (auto p = cbegin(ar) ; p != ar.cend() ; ++p) {     // These are expected to be initialized: no checks
		if (i++ % ElementsPerRow == 0)    os << '\n' ;
		if (isVT_pointer(p))		{ auto ptr = *p ; os << std::setw(SpacePerElement) << *ptr ; }
		else                                          os << std::setw(SpacePerElement) << (*p) ;
		os << ':' << std::setw(2) << i ;
	}
#ifdef _TEST_VARRAY_TEST
	os << endl ; for (size_t j = 0 ; j < i % ElementsPerRow ; j++)  os << std::setw(SpacePerElement + 3) << "----" ;
	for (auto p = cend(ar) ; p != ar.cendlim() ; ++p) {
		if (i++ % ElementsPerRow == 0)    os << '\n' ;
		if (*p == T())          os << std::setw(SpacePerElement) << ' ' ;  // to match pointer = nullptr, etc
		else                    os << std::setw(SpacePerElement) << *p ;
		os << ':' << std::setw(2) << i ;
	}
#endif
	os << endl << "-------------------------------" ;
	return(os) ;
} // friend varray<T,A>::operator << ()

// eoclass varray

#undef _TEST_VARRAY_TEST

#endif // OWN_ARRAY_OWN_ARRAY_OWN