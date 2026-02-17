#include <iostream>
#include <vector>

// Base class
class A {
public:
    virtual void Info() = 0;
    virtual ~A() {     // IMPORTANT: virtual destructor
        std::cout << "A destructor\n";
    }
};

// Derived class B
class B : public A {
public:
    void Info() override {
        std::cout << "I am class B\n";
    }

    ~B() {
        std::cout << "B destructor\n";
    }
};

// Derived class C
class C : public A {
public:
    void Info() override {
        std::cout << "I am class C\n";
    }

    ~C() {
        std::cout << "C destructor\n";
    }
};

// Function that uses polymorphism
void makeSound(A* animal) {
    animal->Info();   // dynamic binding happens here
}

int main() {

    // --- Test single function call ---
    A* a1 = new B();
    A* a2 = new C();

    makeSound(a1);
    makeSound(a2);

    delete a1;
    delete a2;

    std::cout << "-----------------\n";

    // --- Test vector of base pointers ---
    std::vector<A*> objects;

    objects.push_back(new B());
    objects.push_back(new C());
    objects.push_back(new B());

    for (A* obj : objects) {
        obj->Info();   // polymorphism
    }

    // cleanup
    for (A* obj : objects) {
        delete obj;
    }

    return 0;
}
