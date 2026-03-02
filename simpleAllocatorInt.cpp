#include <iostream>
#include <memory>

/*
Why Not Just Use new/delete?

Because:

new/delete: Combined memory + construction, Hardcoded, Not container-friendly
allocator Separate, Customizable, STL-compatible
*/


int main() {
    std::allocator<int> alloc;

    // allocate memory for 3 ints
    int* p = alloc.allocate(3);

    // construct values
    for (int i = 0; i < 3; ++i) {
        std::allocator_traits<std::allocator<int>>::construct(alloc, p + i, i * 10);
    }

    // use them
    for (int i = 0; i < 3; ++i) {
        std::cout << p[i] << " ";
    }

    std::cout << "\n";

    // destroy them
    for (int i = 0; i < 3; ++i) {
        std::allocator_traits<std::allocator<int>>::destroy(alloc, p + i);
    }

    // deallocate memory
    alloc.deallocate(p, 3);
}
