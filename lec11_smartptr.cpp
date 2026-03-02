#include<iostream>

/*C++ has no portable way to know whether a pointer points to heap vs stack vs global.
So any design that lets users hand you a T* and you delete it later is inherently unsafe
unless you restrict how the pointer is obtained, or you don’t delete it,
or you use std::unique_ptr.*/

template<class T>
class SmartPtr{
    private:
        T *ptr;
    
    public:
        // default constructor
        explicit SmartPtr(T *p = nullptr){ ptr = p; }

        // destructor
        ~SmartPtr(){ delete ptr; }
        
        // overloading *
        T& operator*(){ return *ptr; }
        const T& operator*() const { return *ptr; } // without the const version, you cant dereference a const SmartPtr
        
        // overloading ->
        T* operator->(){ return ptr; }
        const T* operator->() const { return ptr; } // without the const version, you cant dereference a const SmartPtr

        // copy constructor(deep copy)
        SmartPtr(const SmartPtr& other){
            if(other.ptr)
                ptr = new T(*other.ptr);
            else
                ptr = nullptr;
        }
        
        // copy assignment
        SmartPtr& operator=(const SmartPtr& other){
            if(this != &other){
                delete ptr;
                ptr = other.ptr ? new T(*other.ptr) : nullptr;
            }
            return *this;
        }

        // move constructor
        SmartPtr(SmartPtr&& other) noexcept : ptr(other.ptr) {
            other.ptr = nullptr;
        }

        // move assignment
        SmartPtr& operator=(SmartPtr&& other) noexcept {
            if (this != &other) {
                delete ptr;
                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }
};

class Test {
public:
    int value;

    Test(int v) : value(v) {
        std::cout << "Construct " << value << "\n";
    }

    Test(const Test& other) : value(other.value) {
        std::cout << "Copy Construct " << value << "\n";
    }

    ~Test() {
        std::cout << "Destruct " << value << "\n";
    }
};

int main()
{
    std::cout << "=== Basic Test ===\n";
    SmartPtr<int> p1(new int(10));
    std::cout << *p1 << "\n";

    std::cout << "\n=== Deep Copy Test ===\n";
    SmartPtr<int> p2 = p1;     // copy constructor
    *p2 = 99;
    std::cout << "p1: " << *p1 << "\n";
    std::cout << "p2: " << *p2 << "\n";

    std::cout << "\n=== Copy Assignment Test ===\n";
    SmartPtr<int> p3(new int(50));
    p3 = p1;
    std::cout << "p3: " << *p3 << "\n";

    std::cout << "\n=== Self Assignment Test ===\n";
    p3 = p3;
    std::cout << "p3 after self-assign: " << *p3 << "\n";

    std::cout << "\n=== Move Constructor Test ===\n";
    SmartPtr<int> p4 = std::move(p1);
    if (!p1.operator->())
        std::cout << "p1 is null after move\n";
    std::cout << "p4: " << *p4 << "\n";

    std::cout << "\n=== Move Assignment Test ===\n";
    SmartPtr<int> p5(new int(777));
    p5 = std::move(p4);
    std::cout << "p5: " << *p5 << "\n";

    std::cout << "\n=== Object Lifetime Test ===\n";
    {
        SmartPtr<Test> t1(new Test(5));
        SmartPtr<Test> t2 = t1;   // deep copy
    } // should show two destructors

    std::cout << "\n=== End of main ===\n";

    return 0;
}
