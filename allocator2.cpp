#include <iostream>
#include <memory>
#include <vector>

// =======================================
// Custom Logging Allocator
// =======================================

template<typename T>
class LoggingAllocator {
public:
    using value_type = T;

    // Required default constructor
    LoggingAllocator() noexcept {}

    // Required converting constructor
    template<typename U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    // Allocate raw memory
    T* allocate(std::size_t n) {
        std::cout << "[ALLOCATE] "
                  << n << " object(s) of size "
                  << sizeof(T)
                  << " (total bytes: "
                  << n * sizeof(T)
                  << ")\n";

        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    // Deallocate raw memory
    void deallocate(T* p, std::size_t n) noexcept {
        std::cout << "[DEALLOCATE] "
                  << n << " object(s)\n";

        ::operator delete(p);
    }
};

// Required equality operators
template<typename T, typename U>
bool operator==(const LoggingAllocator<T>&,
                const LoggingAllocator<U>&) {
    return true;
}

template<typename T, typename U>
bool operator!=(const LoggingAllocator<T>&,
                const LoggingAllocator<U>&) {
    return false;
}

// =======================================
// Test Class to Observe Construction
// =======================================

class Test {
public:
    int value;

    Test(int v) : value(v) {
        std::cout << "Constructing Test(" << value << ")\n";
    }

    ~Test() {
        std::cout << "Destroying Test(" << value << ")\n";
    }
};

// =======================================
// Main
// =======================================

int main() {

    std::cout << "=== Creating vector ===\n";

    std::vector<Test, LoggingAllocator<Test>> vec;

    std::cout << "\n=== Pushing elements ===\n";

    vec.emplace_back(10);
    vec.emplace_back(20);
    vec.emplace_back(30);

    std::cout << "\n=== Clearing vector ===\n";

    vec.clear();   // destroys objects

    std::cout << "\n=== Exiting main ===\n";

    return 0;
}
