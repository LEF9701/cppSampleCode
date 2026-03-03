// all_in_one.cpp
// g++ -std=c++20 -O2 -pthread all_in_one.cpp && ./a.out

#include <algorithm>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

// ============================================================
// 1) Concepts + a generic algorithm (your custom find)
// ============================================================

template <class It>
concept InputIter = requires(It it) {
    { ++it } -> std::same_as<It&>;
    { *it };
    { it != it } -> std::convertible_to<bool>;
};

template <InputIter It, class T>
It my_find(It first, It last, const T& value) {
    while (first != last && !(*first == value)) ++first;
    return first;
}

// ============================================================
// 2) A small RAII SmartPtr (unique-ownership, move-only)
//    (demonstrates const overloads of *, ->)
// ============================================================

template <class T>
class SmartPtr {
    T* ptr_ = nullptr;

public:
    SmartPtr() = default;
    explicit SmartPtr(T* p) noexcept : ptr_(p) {}

    // no copy (unique ownership)
    SmartPtr(const SmartPtr&) = delete;
    SmartPtr& operator=(const SmartPtr&) = delete;

    // move
    SmartPtr(SmartPtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}
    SmartPtr& operator=(SmartPtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = std::exchange(other.ptr_, nullptr);
        }
        return *this;
    }

    ~SmartPtr() { reset(); }

    void reset(T* p = nullptr) noexcept {
        delete ptr_;
        ptr_ = p;
    }

    T* get() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }

    T& operator*() noexcept { return *ptr_; }
    const T& operator*() const noexcept { return *ptr_; }

    T* operator->() noexcept { return ptr_; }
    const T* operator->() const noexcept { return ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }
};

// ============================================================
// 3) MyArray<T>: initializer_list ctor + deep copy assignment + <<
// ============================================================

template <class T>
class MyArray {
    T* arr_ = nullptr;
    std::size_t size_ = 0;

public:
    MyArray() = default;

    MyArray(std::initializer_list<T> list) : size_(list.size()) {
        arr_ = new T[size_];
        std::size_t i = 0;
        for (const auto& x : list) arr_[i++] = x;
    }

    // deep copy
    MyArray(const MyArray& other) : size_(other.size_) {
        arr_ = size_ ? new T[size_] : nullptr;
        for (std::size_t i = 0; i < size_; ++i) arr_[i] = other.arr_[i];
    }

    // move
    MyArray(MyArray&& other) noexcept : arr_(std::exchange(other.arr_, nullptr)),
                                        size_(std::exchange(other.size_, 0)) {}

    // copy assignment (deep copy)
    MyArray& operator=(const MyArray& other) {
        if (this == &other) return *this;

        T* newArr = other.size_ ? new T[other.size_] : nullptr;
        for (std::size_t i = 0; i < other.size_; ++i) newArr[i] = other.arr_[i];

        delete[] arr_;              // frees "old memory" owned by *this*
        arr_ = newArr;
        size_ = other.size_;
        return *this;
    }

    // move assignment
    MyArray& operator=(MyArray&& other) noexcept {
        if (this != &other) {
            delete[] arr_;
            arr_ = std::exchange(other.arr_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    ~MyArray() { delete[] arr_; }

    std::size_t size() const noexcept { return size_; }

    T& operator[](std::size_t i) { return arr_[i]; }
    const T& operator[](std::size_t i) const { return arr_[i]; }

    friend std::ostream& operator<<(std::ostream& out, const MyArray& a) {
        for (std::size_t i = 0; i < a.size_; ++i) out << a.arr_[i] << (i + 1 == a.size_ ? "" : " ");
        return out;
    }
};

// ============================================================
// 4) Allocator-backed LinkedList<T> with iterator + Rule of 5
// ============================================================

template <class T>
class LinkedList {
    struct Node {
        T data;
        Node* next = nullptr;

        Node(const T& v, Node* n) : data(v), next(n) {}
        Node(T&& v, Node* n) : data(std::move(v)), next(n) {}
    };

    using Alloc = std::allocator<Node>;
    using Traits = std::allocator_traits<Alloc>;

    Alloc alloc_;
    Node* head_ = nullptr;
    std::size_t size_ = 0;

    void clear() noexcept {
        Node* cur = head_;
        while (cur) {
            Node* nxt = cur->next;
            Traits::destroy(alloc_, cur);
            Traits::deallocate(alloc_, cur, 1);
            cur = nxt;
        }
        head_ = nullptr;
        size_ = 0;
    }

    void copy_from(const LinkedList& other) {
        // preserve order by collecting then pushing_front reverse, or build tail.
        Node* tail = nullptr;
        for (Node* cur = other.head_; cur; cur = cur->next) {
            Node* p = Traits::allocate(alloc_, 1);
            try {
                Traits::construct(alloc_, p, cur->data, nullptr);
            } catch (...) {
                Traits::deallocate(alloc_, p, 1);
                throw;
            }
            if (!head_) head_ = p;
            else tail->next = p;
            tail = p;
            ++size_;
        }
    }

public:
    class iterator {
        Node* p_ = nullptr;
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using reference = T&;
        using pointer = T*;

        iterator() = default;
        explicit iterator(Node* p) : p_(p) {}

        reference operator*() const { return p_->data; }
        pointer operator->() const { return &p_->data; }

        iterator& operator++() { p_ = p_->next; return *this; }
        iterator operator++(int) { iterator tmp(*this); ++(*this); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) { return a.p_ == b.p_; }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }
    };

    class const_iterator {
        const Node* p_ = nullptr;
    public:
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using reference = const T&;
        using pointer = const T*;

        const_iterator() = default;
        explicit const_iterator(const Node* p) : p_(p) {}

        reference operator*() const { return p_->data; }
        pointer operator->() const { return &p_->data; }

        const_iterator& operator++() { p_ = p_->next; return *this; }
        const_iterator operator++(int) { const_iterator tmp(*this); ++(*this); return tmp; }

        friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.p_ == b.p_; }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) { return !(a == b); }
    };

    LinkedList() = default;

    // copy
    LinkedList(const LinkedList& other) { copy_from(other); }

    // move
    LinkedList(LinkedList&& other) noexcept
        : alloc_(std::move(other.alloc_)),
          head_(std::exchange(other.head_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}

    // copy assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;
        clear();
        copy_from(other);
        return *this;
    }

    // move assignment
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            alloc_ = std::move(other.alloc_);
            head_ = std::exchange(other.head_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    ~LinkedList() { clear(); }

    void push_front(const T& v) {
        Node* p = Traits::allocate(alloc_, 1);
        Traits::construct(alloc_, p, v, head_);
        head_ = p;
        ++size_;
    }

    void push_front(T&& v) {
        Node* p = Traits::allocate(alloc_, 1);
        Traits::construct(alloc_, p, std::move(v), head_);
        head_ = p;
        ++size_;
    }

    std::size_t size() const noexcept { return size_; }

    iterator begin() noexcept { return iterator(head_); }
    iterator end() noexcept { return iterator(nullptr); }
    const_iterator begin() const noexcept { return const_iterator(head_); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept { return const_iterator(head_); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
};

// ============================================================
// 5) Variadic MyTuple + compile-time get<I>
// ============================================================

template <class... Ts>
class MyTuple;

template <>
class MyTuple<> {};

template <class T, class... Rest>
class MyTuple<T, Rest...> {
    T value_;
    MyTuple<Rest...> rest_;

    template <std::size_t I, class Tup>
    friend struct TupleGet;

public:
    MyTuple() = default;
    MyTuple(const T& v, const Rest&... r) : value_(v), rest_(r...) {}
    MyTuple(T&& v, Rest&&... r) : value_(std::move(v)), rest_(std::forward<Rest>(r)...) {}
};

template <std::size_t I, class Tup>
struct TupleGet;

template <class T, class... Rest>
struct TupleGet<0, MyTuple<T, Rest...>> {
    static T& get(MyTuple<T, Rest...>& t) { return t.value_; }
    static const T& get(const MyTuple<T, Rest...>& t) { return t.value_; }
};

template <std::size_t I, class T, class... Rest>
struct TupleGet<I, MyTuple<T, Rest...>> {
    static auto& get(MyTuple<T, Rest...>& t) {
        return TupleGet<I - 1, MyTuple<Rest...>>::get(t.rest_);
    }
    static const auto& get(const MyTuple<T, Rest...>& t) {
        return TupleGet<I - 1, MyTuple<Rest...>>::get(t.rest_);
    }
};

template <std::size_t I, class... Ts>
decltype(auto) get(MyTuple<Ts...>& t) { return TupleGet<I, MyTuple<Ts...>>::get(t); }

template <std::size_t I, class... Ts>
decltype(auto) get(const MyTuple<Ts...>& t) { return TupleGet<I, MyTuple<Ts...>>::get(t); }

// ============================================================
// 6) Operator overloading showcase: [], (), ++, +, <<
// ============================================================

class Counter {
    int x_ = 0;

public:
    Counter() = default;
    explicit Counter(int x) : x_(x) {}

    // prefix ++
    Counter& operator++() { ++x_; return *this; }

    // postfix ++
    Counter operator++(int) { Counter tmp(*this); ++(*this); return tmp; }

    friend Counter operator+(const Counter& a, const Counter& b) { return Counter(a.x_ + b.x_); }

    int operator()() const { return x_; }         // function call operator
    int& operator[](int /*dummy*/) { return x_; } // silly but demonstrates syntax

    friend std::ostream& operator<<(std::ostream& os, const Counter& c) { return os << c.x_; }
};

// ============================================================
// 7) Basic concurrency: threads + mutex + atomic
// ============================================================

struct BankAccount {
    mutable std::mutex m;
    long long balance = 0;

    void deposit(long long amt) {
        std::lock_guard<std::mutex> lock(m);
        balance += amt;
    }

    bool withdraw(long long amt) {
        std::lock_guard<std::mutex> lock(m);
        if (balance < amt) return false;
        balance -= amt;
        return true;
    }

    long long get() const {
        std::lock_guard<std::mutex> lock(m);
        return balance;
    }
};

int main() {
    std::cout << "== 1) my_find + iterators ==\n";
    std::vector<int> v{9, 8, 7, 6, 5};
    auto it = my_find(v.begin(), v.end(), 6);
    std::cout << "found 6? " << (it != v.end() ? "yes" : "no") << "\n\n";

    std::cout << "== 2) SmartPtr (move-only) ==\n";
    SmartPtr<std::string> sp(new std::string("hello"));
    const SmartPtr<std::string>& csp = sp;
    std::cout << "*sp=" << *sp << "\n";
    std::cout << "csp->size()=" << csp->size() << "\n\n";

    std::cout << "== 3) MyArray initializer_list + deep copy ==\n";
    MyArray<int> a{1, 2, 3, 4};
    MyArray<int> b{10, 20};
    b = a; // deep copy assignment
    std::cout << "a: " << a << "\n";
    std::cout << "b(after copy=): " << b << "\n\n";

    std::cout << "== 4) LinkedList with allocator + iterator + Rule of 5 ==\n";
    LinkedList<std::string> lst;
    lst.push_front("C");
    lst.push_front("B");
    lst.push_front("A");

    for (auto& s : lst) std::cout << s << " ";
    std::cout << "\nsize=" << lst.size() << "\n";

    LinkedList<std::string> lst2 = lst; // copy
    LinkedList<std::string> lst3 = std::move(lst2); // move
    std::cout << "lst3: ";
    for (const auto& s : lst3) std::cout << s << " ";
    std::cout << "\n\n";

    std::cout << "== 5) Variadic MyTuple + get<I> ==\n";
    MyTuple<int, double, char> t(1, 3.14, 'A');
    std::cout << "get<0>=" << get<0>(t) << ", get<1>=" << get<1>(t) << ", get<2>=" << get<2>(t) << "\n\n";

    std::cout << "== 6) Operator overloads ==\n";
    Counter c1(3), c2(7);
    Counter c3 = c1 + c2;
    std::cout << "c3=" << c3 << "\n";
    std::cout << "c3()=" << c3() << "\n";
    std::cout << "++c3=" << ++c3 << "\n";
    std::cout << "c3++=" << c3++ << " (after: " << c3 << ")\n\n";

    std::cout << "== 7) Concurrency (mutex + atomic) ==\n";
    BankAccount acct;
    std::atomic<long long> totalDeposited{0};

    auto worker = [&]() {
        for (int i = 0; i < 100000; ++i) {
            acct.deposit(1);
            totalDeposited.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread th1(worker), th2(worker), th3(worker);
    th1.join(); th2.join(); th3.join();

    std::cout << "totalDeposited=" << totalDeposited.load() << "\n";
    std::cout << "acct.balance=" << acct.get() << "\n";

    // small withdrawal demo
    bool ok = acct.withdraw(250000);
    std::cout << "withdraw 250000 ok? " << (ok ? "yes" : "no") << ", balance now=" << acct.get() << "\n";

    return 0;
}
