#include<iostream>
#include<vector>
#include<list>
#include<deque>
#include<algorithm>

/*
Vector is:
A container of contiguous memory.
But:
Iterators are an abstraction layer over memory.
STL is designed so algorithms work on:
	•	pointers
	•	vector iterators
	•	list iterators
	•	custom iterators
All through the same interface.
*/

template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value){
    while(first != last && *first != value)
        ++first;
    return first;
}

int main(){
    int ia[] = {0,1,2,3,4,5,6};
    constexpr auto arraySize = std::size(ia);

    std::vector<int> ivect(ia, ia + arraySize); //[first, last) -> &ia[0] to &ia[7]
    
    std::vector<int>::iterator it1 = ::find(ivect.begin(), ivect.end(), 4); // global namespace
    if(it1 == ivect.end()) // [first, last) convention makes algorithms consistent.
        std::cout << "4 not found." << std::endl;
    else
        std::cout << "4 found. value: " << *it1 << std::endl;

    std::list<int> ilist(ia, ia + arraySize);
    std::list<int>::iterator it2 = ::find(ilist.begin(), ilist.end(), 6);
    if(it2 == ilist.end()) // [first, last) convention makes algorithms consistent.
        std::cout << "6 not found." << std::endl;
    else
        std::cout << "6 found. value: " << *it2 << std::endl;

    std::deque<int> ideque(ia, ia + arraySize);
    std::deque<int>::iterator it3 = ::find(ideque.begin(), ideque.end(), 8);
    if(it3 == ideque.end()) // [first, last) convention makes algorithms consistent.
        std::cout << "8 not found." << std::endl;
    else
        std::cout << "8 found. value: " << *it3 << std::endl;

    return 0;

    /*
    begin() and end() return iterator objects, not guaranteed raw pointers.
    Formally std::vector<int>::iterator is NOT int*.
    In most standard library implementations (libstdc++, libc++, MSVC) usually just a raw pointer internally
    Because: 
    •vector stores elements contiguously
	•A pointer is already a perfect random-access iterator
    So typically: std::vector<int>::iterator == int*
    But The C++ standard does NOT guarantee that.

    C++ guarantees:

    &v[0] == v.data()
    v.data()   // returns T*

    int* p = v.data();
    int* p = &v[0];

   
    The standard says:
    vector<T>::iterator
    But it does NOT say:
    iterator == T*

    Compare With Other Containers

    vector: begin() usually returns pointer
    array: begin() always pointer
    list: node iterator
    map: tree iterator
    deque complex segmented iterator

    So,
    auto it = v.begin();
    int* p = it;   // not guaranteed portable

    How to get pointer safely?
    int* p = v.data();              // ok even if empty (returns nullptr or non-derefable)
    if (!v.empty()) {
        int* p2 = &v[0];            // ok
        int* p3 = &*v.begin();      // ok
    }

    */

    
    /*
    std::vector has this constructor:
    template<class InputIt>
    vector(InputIt first, InputIt last);

    other constructor:
    1. Default Constructor
        std::vector<int> v
        creates an empty vector

        vector();

    2. Size Constructor
        std::vector<int> v(5);
        creates 5 default-initialized(for int they become 0) elements.

        vector(size_type count);

    3. Size + Value Constructor
        std::vector<int> v(4, 42)
        creates 4 elements, all initialized to 42

        vector(size_type count, const T& value);

    4. Range Constructor
        std::vector<int> v(ia, ia + 7)

        template<class InputIt>
        vector(InputIt first, InputIt last);
    
    5. Copy Constructor
        std::vector<int> v2(v1);
        Performs deep copy

        vector(const vector& other);

    6. Move Constructor
        std::vector<int> v2(std::move(v1));
        Transfers ownership of memory.

        vector(vector&& other) noexpect;
    
    7. Initializer List Constructor
        std::vector<int> v = {1,2,3,4};

        vector(std::initializer_list<T> init);

    8. Allocator-Aware Constructors
        vector(const Allocator& alloc);
        vector(size_type count, const Allocator& alloc);
        vector(size_type count, const T& value, const Allocator& alloc);
        vector(InputIt first, InputIt last, const Allocator& alloc);
        vector(const vector& other, const Allocator& alloc);
        vector(vector&& other, const Allocator& alloc);

    9. C++23 From-Range Constructor
        std::vector<int> v(std::from_range, some_range);
        integrates with ranges lib

        vector(std::from_range_t, R&& rg);
    */


}
