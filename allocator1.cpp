#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <new>

// A small type to prove construction/destruction happens
struct Widget {
    int id;
    std::string name;

    Widget(int i, std::string n) : id(i), name(std::move(n)) {
        std::cout << "Widget ctor: " << id << ", " << name << "\n";
    }
    ~Widget() {
        std::cout << "Widget dtor: " << id << ", " << name << "\n";
    }
};

// A custom allocator that logs allocate/deallocate.
// This follows the minimum requirements for allocator-like types used by std containers.
template <class T>
struct LoggingAllocator {
    using value_type = T;

    LoggingAllocator() noexcept = default;
    template <class U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        std::cout << "[alloc]   " << n << " * " << sizeof(T)
                  << " bytes = " << (n * sizeof(T)) << " bytes\n";
        if (n > static_cast<std::size_t>(-1) / sizeof(T))
            throw std::bad_alloc();

        // Use global operator new for raw storage
        void* p = ::operator new(n * sizeof(T));
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t n) noexcept {
        std::cout << "[dealloc] " << n << " * " << sizeof(T)
                  << " bytes = " << (n * sizeof(T)) << " bytes\n";
        ::operator delete(p);
    }

    template <class U>
    bool operator==(const LoggingAllocator<U>&) const noexcept { return true; }
    template <class U>
    bool operator!=(const LoggingAllocator<U>&) const noexcept { return false; }
};

int main() {
    using Alloc = LoggingAllocator<Widget>;
    Alloc alloc;

    // 1) Allocate raw, uninitialized memory for N Widgets
    std::size_t N = 3;
    Widget* buf = alloc.allocate(N);

    // We'll track how many we successfully constructed, for exception safety
    std::size_t constructed = 0;

    try {
        // 2) Construct objects in-place using std::allocator_traits
        //    (preferred over calling placement-new directly)
        std::allocator_traits<Alloc>::construct(alloc, &buf[0], 1, "alpha");
        ++constructed;

        std::allocator_traits<Alloc>::construct(alloc, &buf[1], 2, "beta");
        ++constructed;

        std::allocator_traits<Alloc>::construct(alloc, &buf[2], 3, "gamma");
        ++constructed;

        // 3) Use the objects
        std::cout << "\nUsing objects:\n";
        for (std::size_t i = 0; i < N; ++i) {
            std::cout << "buf[" << i << "] = { id=" << buf[i].id
                      << ", name=" << buf[i].name << " }\n";
        }

        std::cout << "\nDone using objects.\n\n";
    } catch (...) {
        // If construction fails partway through, destroy what we built
        for (std::size_t i = constructed; i > 0; --i) {
            std::allocator_traits<Alloc>::destroy(alloc, &buf[i - 1]);
        }
        alloc.deallocate(buf, N);
        throw; // rethrow
    }

    // 4) Destroy objects in reverse construction order
    for (std::size_t i = N; i > 0; --i) {
        std::allocator_traits<Alloc>::destroy(alloc, &buf[i - 1]);
    }

    // 5) Deallocate raw memory
    alloc.deallocate(buf, N);

    std::cout << "\nAll cleaned up.\n";
    return 0;
}
