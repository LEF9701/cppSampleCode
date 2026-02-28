#include<iostream>
#include<print>
// require C++23
// g++ -std=c++23 <filename>.cpp -o <filename>

class A{
    private:
        std::unique_ptr<int> p;

    public:
        // Parameterised constructor
        A(int value) : p(std::make_unique<int>(value)) {}

        // Manual deep-copy constructor
        A(const A& other) {
            if (other.p) {
                p = std::make_unique<int>(*other.p);
            }
        }
        // Manual copy assignment operator
        A& operator=(const A& other) {
        if (this != &other) {
            if (other.p)
                p = std::make_unique<int>(*other.p);
            else
                p.reset();
        }
            return *this;
        }

        int getValue()const{ return *p; }
        const int* getAddress() const { return p.get(); }

};
/*
interesting fact:
compiler will automatically generate: A(const A&) = delete;
because class A have std::unique_ptr<T>
note that it’s NOT about shallow copy, unique_ptr is non-copyable.
std::shared_ptr<T> is copyable so compiler would generate copy constructor

compiler will automatically generate:
1. A() noexcept : p(nullptr) {} // constructor
2. ~A(); // destructor
3. A(const A&) = delete; // copy constructor
4. A& operator=(const A&) = delete; // copy assignment operator
5. A(A&&) noexcept; // move constructor
6. A& operator=(A&&) noexcept; // move assignment operator

if you do nothing(rule of 0), compiler will automatically generate
1. move semantics
2. no-copyable behavior
*/

int main(){
    A a1(10);
    std::println("The value of p is: {}", a1.getValue());
    std::println("The address of p is: {}", static_cast<const void*>(a1.getAddress()));
    A a2 = a1;
    std::println("The value of p is: {}", a2.getValue());  
    std::println("The address of p is: {}", static_cast<const void*>(a2.getAddress()));

    return 0;

}
