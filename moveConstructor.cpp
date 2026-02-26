#include <iostream>
#include <vector>
#include <utility>


//	•	Move = steal pointer → O(1)
//	•	Copy = deep copy vector → O(n)


class MyClass {
public:
    std::vector<int> data;

    MyClass(std::vector<int> d)
        : data(std::move(d)) {
        std::cout << "Moved into MyClass\n";
    }
};

class BigData {
private:
    std::vector<int> data_;

public:
    // Constructor
    BigData(std::vector<int> data)
        : data_(std::move(data)) {}

    // Move constructor
    BigData(BigData&& other) noexcept
        : data_(std::move(other.data_)) {
        std::cout << "Move constructor called\n";
    }   
    // If move constructor is NOT noexcept,
    // std::vector may fallback to copying during reallocation.
    // bcz std::vector prefers move only if move is guaranteed not to throw.


    // Copy constructor (for comparison)
    BigData(const BigData& other)
        : data_(other.data_) {
        std::cout << "Copy constructor called\n";
    }

    // Advanced API Design — overload for performance
    void setData(const std::vector<int>& v) {
        std::cout << "Copy setData\n";
        data_ = v;
    }

    void setData(std::vector<int>&& v) {
        std::cout << "Move setData\n";
        data_ = std::move(v);
    }

    size_t size() const {
        return data_.size();
    }
};

int main() {
    std::cout << "===== Example 1: std::move into constructor =====\n";

    std::vector<int> v = {1, 2, 3};

    std::cout << "Original v size (before): " << v.size() << "\n";

    MyClass obj(std::move(v));

    std::cout << "Original v size (after): " << v.size() << "\n";


    std::cout << "\n===== Example 2: Move constructor vs Copy =====\n";

    BigData b1({10, 20, 30});

    BigData b2 = b1;              // copy
    BigData b3 = std::move(b1);   // move


    std::cout << "\n===== Example 3: Overloaded setData =====\n";

    BigData b4({});

    std::vector<int> bigVec = {100, 200, 300};

    b4.setData(bigVec);            // copy version
    b4.setData(std::move(bigVec)); // move version

    std::cout << "bigVec size after move: " << bigVec.size() << "\n";

    return 0;
}
