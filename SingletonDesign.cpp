#include <iostream>

class Singleton {
private:
    Singleton() { std::cout << "Constructor\n"; }

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
        static Singleton instance;  // created once
        return instance;
    }

    void sayHello() {
        std::cout << "Hello from Singleton\n";
    }
};

int main() {
    Singleton& s1 = Singleton::getInstance();
    Singleton& s2 = Singleton::getInstance();

    s1.sayHello();
}
