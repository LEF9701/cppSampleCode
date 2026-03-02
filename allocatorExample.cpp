// ============================================================
// EXAMPLE 1: std::allocator with a user-defined class
// Shows the full allocate → construct → destroy → deallocate lifecycle
// ============================================================

#include <iostream>
#include <memory>
#include <string>
#include <cstddef>
#include <cstdlib>
#include <new>

struct Student {
    std::string name;
    int grade;

    Student(const std::string& n, int g) : name(n), grade(g) {
        std::cout << "  [constructed: " << name << "]\n";
    }
    ~Student() {
        std::cout << "  [destroyed: " << name << "]\n";
    }
};

void example1_allocator_with_class() {
    std::cout << "=== Example 1: std::allocator with Student ===\n\n";

    std::allocator<Student> alloc;
    constexpr std::size_t n = 3;

    // Step 1: allocate raw memory (no constructors called)
    Student* p = alloc.allocate(n);
    std::cout << "Memory allocated for " << n << " Students\n";

    // Step 2: construct objects in-place
    std::cout << "Constructing:\n";
    std::allocator_traits<std::allocator<Student>>::construct(alloc, p + 0, "Alice", 95);
    std::allocator_traits<std::allocator<Student>>::construct(alloc, p + 1, "Bob", 82);
    std::allocator_traits<std::allocator<Student>>::construct(alloc, p + 2, "Carol", 91);

    // Step 3: use the objects
    std::cout << "\nStudents:\n";
    for (std::size_t i = 0; i < n; ++i) {
        std::cout << "  " << p[i].name << " - grade: " << p[i].grade << "\n";
    }

    // Step 4: destroy objects (destructors called, memory NOT freed)
    std::cout << "\nDestroying:\n";
    for (std::size_t i = 0; i < n; ++i) {
        std::allocator_traits<std::allocator<Student>>::destroy(alloc, p + i);
    }

    // Step 5: deallocate raw memory
    alloc.deallocate(p, n);
    std::cout << "Memory deallocated\n\n";
}

// ============================================================
// EXAMPLE 2: Minimal vector using an allocator
// Shows why containers use allocators (separate reserve from push_back)
// ============================================================

template <typename T, typename Alloc = std::allocator<T>>
class MiniVec {
    using AllocTraits = std::allocator_traits<Alloc>;

    Alloc alloc_;
    T* data_;
    std::size_t size_;
    std::size_t capacity_;

public:
    MiniVec() : data_(nullptr), size_(0), capacity_(0) {}

    ~MiniVec() {
        clear();
        if (data_) {
            alloc_.deallocate(data_, capacity_);
        }
    }

    // No copy/move for simplicity
    MiniVec(const MiniVec&) = delete;
    MiniVec& operator=(const MiniVec&) = delete;

    void reserve(std::size_t new_cap) {
        if (new_cap <= capacity_) return;

        // allocate new block
        T* new_data = alloc_.allocate(new_cap);

        // move existing elements to new block
        for (std::size_t i = 0; i < size_; ++i) {
            AllocTraits::construct(alloc_, new_data + i, std::move(data_[i]));
            AllocTraits::destroy(alloc_, data_ + i);
        }

        // free old block
        if (data_) {
            alloc_.deallocate(data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_cap;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        // construct ONE object in pre-allocated memory
        AllocTraits::construct(alloc_, data_ + size_, value);
        ++size_;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        AllocTraits::construct(alloc_, data_ + size_, std::forward<Args>(args)...);
        ++size_;
    }

    void clear() {
        for (std::size_t i = 0; i < size_; ++i) {
            AllocTraits::destroy(alloc_, data_ + i);
        }
        size_ = 0;
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }
};

void example2_mini_vector() {
    std::cout << "=== Example 2: MiniVec using allocator ===\n\n";

    MiniVec<Student> students;

    std::cout << "Reserving space for 4 students (no construction yet):\n";
    students.reserve(4);
    std::cout << "  capacity=" << students.capacity()
              << ", size=" << students.size() << "\n\n";

    std::cout << "Emplacing students one at a time:\n";
    students.emplace_back("Dave", 88);
    students.emplace_back("Eve", 97);
    students.emplace_back("Frank", 73);

    std::cout << "\nContents:\n";
    for (std::size_t i = 0; i < students.size(); ++i) {
        std::cout << "  " << students[i].name << " - " << students[i].grade << "\n";
    }
    std::cout << "  capacity=" << students.capacity()
              << ", size=" << students.size() << "\n";

    std::cout << "\nAdding 2 more (triggers reallocation):\n";
    students.emplace_back("Grace", 90);
    students.emplace_back("Hank", 85);
    std::cout << "  capacity=" << students.capacity()
              << ", size=" << students.size() << "\n";

    std::cout << "\nDestroying MiniVec:\n";
    // destructor calls destroy on each element, then deallocates
}

// ============================================================
// EXAMPLE 3: Custom arena (pool) allocator
// Pre-allocates a fixed block; allocations just bump a pointer.
// Great for temporary, short-lived allocations (no individual free).
// ============================================================

template <typename T>
class ArenaAllocator {
    char* arena_;       // raw memory block
    std::size_t offset_;
    std::size_t arena_size_;

public:
    using value_type = T;

    explicit ArenaAllocator(std::size_t max_objects = 1024)
        : offset_(0), arena_size_(max_objects * sizeof(T))
    {
        arena_ = static_cast<char*>(std::malloc(arena_size_));
        if (!arena_) throw std::bad_alloc();
        std::cout << "  [arena: allocated " << arena_size_ << " bytes]\n";
    }

    ~ArenaAllocator() {
        std::free(arena_);
        std::cout << "  [arena: freed entire block]\n";
    }

    // Required for allocator_traits
    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>&) = delete;

    T* allocate(std::size_t n) {
        std::size_t bytes = n * sizeof(T);

        // Align to alignof(T)
        std::size_t align = alignof(T);
        offset_ = (offset_ + align - 1) & ~(align - 1);

        if (offset_ + bytes > arena_size_) {
            throw std::bad_alloc();
        }

        T* result = reinterpret_cast<T*>(arena_ + offset_);
        offset_ += bytes;
        std::cout << "  [arena: bumped " << bytes << " bytes, "
                  << "used=" << offset_ << "/" << arena_size_ << "]\n";
        return result;
    }

    void deallocate(T*, std::size_t) noexcept {
        // Arena allocator: individual deallocation is a no-op.
        // Everything is freed when the arena is destroyed.
        std::cout << "  [arena: deallocate is a no-op]\n";
    }

    void reset() {
        offset_ = 0;
        std::cout << "  [arena: reset to 0]\n";
    }

    std::size_t used() const { return offset_; }
};

void example3_arena_allocator() {
    std::cout << "=== Example 3: Arena (pool) allocator ===\n\n";

    ArenaAllocator<int> arena(64);  // space for 64 ints

    // Allocate several chunks from the arena
    int* a = arena.allocate(5);
    int* b = arena.allocate(3);

    // Construct values manually
    for (int i = 0; i < 5; ++i) a[i] = i * 100;
    for (int i = 0; i < 3; ++i) b[i] = i + 42;

    std::cout << "\nArray a: ";
    for (int i = 0; i < 5; ++i) std::cout << a[i] << " ";

    std::cout << "\nArray b: ";
    for (int i = 0; i < 3; ++i) std::cout << b[i] << " ";

    std::cout << "\n\n";

    // deallocate is a no-op — arena frees everything at once
    arena.deallocate(a, 5);
    arena.deallocate(b, 3);

    // reset lets you reuse the arena
    std::cout << "\nResetting arena:\n";
    arena.reset();

    int* c = arena.allocate(10);
    for (int i = 0; i < 10; ++i) c[i] = i;

    std::cout << "Array c: ";
    for (int i = 0; i < 10; ++i) std::cout << c[i] << " ";
    std::cout << "\n\n";

    arena.deallocate(c, 10);
    std::cout << "\nArena going out of scope:\n";
}

// ============================================================
// main
// ============================================================

int main() {
    example1_allocator_with_class();
    std::cout << std::string(50, '-') << "\n\n";

    example2_mini_vector();
    std::cout << std::string(50, '-') << "\n\n";

    example3_arena_allocator();

    return 0;
}
