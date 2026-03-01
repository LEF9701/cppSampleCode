#include <iostream>
#include <vector>
#include <stdexcept>
#include <compare>   // <=>

// A small vector-like class to demonstrate lots of operator overloads.
class Vec {
    std::vector<int> data_;

public:
    // constructors
    Vec() = default;
    Vec(std::initializer_list<int> init) : data_(init) {}
    explicit Vec(std::size_t n, int value = 0) : data_(n, value) {}

    // size helper
    std::size_t size() const { return data_.size(); }

    // operator[] (non-const + const)
    int& operator[](std::size_t i) {
        // [] normally does NOT bounds-check in std::vector, but we can choose to:
        if (i >= data_.size()) throw std::out_of_range("Vec::operator[] out of range");
        return data_[i];
    }
    const int& operator[](std::size_t i) const {
        if (i >= data_.size()) throw std::out_of_range("Vec::operator[] out of range");
        return data_[i];
    }

    // operator() example: treat Vec like a callable that returns sum in [l, r)
    int operator()(std::size_t l, std::size_t r) const {
        if (l > r || r > data_.size()) throw std::out_of_range("Vec::operator() bad range");
        int s = 0;
        for (std::size_t i = l; i < r; ++i) s += data_[i];
        return s;
    }

    // unary minus: -v
    Vec operator-() const {
        Vec out(*this);
        for (auto& x : out.data_) x = -x;
        return out;
    }

    // prefix ++ : ++v (increment all, return *this)
    Vec& operator++() {
        for (auto& x : data_) ++x;
        return *this;
    }

    // postfix ++ : v++ (return old copy, then increment)
    Vec operator++(int) {
        Vec old(*this);
        ++(*this);
        return old;
    }

    // compound assignments with another Vec (element-wise)
    Vec& operator+=(const Vec& rhs) {
        if (size() != rhs.size()) throw std::invalid_argument("Vec += size mismatch");
        for (std::size_t i = 0; i < size(); ++i) data_[i] += rhs.data_[i];
        return *this;
    }
    Vec& operator-=(const Vec& rhs) {
        if (size() != rhs.size()) throw std::invalid_argument("Vec -= size mismatch");
        for (std::size_t i = 0; i < size(); ++i) data_[i] -= rhs.data_[i];
        return *this;
    }

    // compound assignments with a scalar
    Vec& operator*=(int k) {
        for (auto& x : data_) x *= k;
        return *this;
    }
    Vec& operator/=(int k) {
        if (k == 0) throw std::invalid_argument("Vec /= division by zero");
        for (auto& x : data_) x /= k;
        return *this;
    }

    // comparisons: == and <=> (lexicographic via std::vector)
    bool operator==(const Vec& other) const = default;
    auto operator<=>(const Vec& other) const = default;

    // explicit conversion to bool: e.g. if(v) ...
    explicit operator bool() const {
        // "true" if any element is non-zero
        for (int x : data_) if (x != 0) return true;
        return false;
    }

    // copy/move assignment shown explicitly (compiler-generated ones are fine too)
    Vec& operator=(const Vec&) = default;
    Vec& operator=(Vec&&) noexcept = default;

    // friends: non-member operators
    friend Vec operator+(Vec lhs, const Vec& rhs) { // note: pass lhs by value to reuse +=
        lhs += rhs;
        return lhs;
    }
    friend Vec operator-(Vec lhs, const Vec& rhs) {
        lhs -= rhs;
        return lhs;
    }

    // Vec * scalar and scalar * Vec
    friend Vec operator*(Vec v, int k) {
        v *= k;
        return v;
    }
    friend Vec operator*(int k, Vec v) {
        v *= k;
        return v;
    }

    // Vec / scalar
    friend Vec operator/(Vec v, int k) {
        v /= k;
        return v;
    }

    // stream output: print like [1, 2, 3]
    friend std::ostream& operator<<(std::ostream& os, const Vec& v) {
        os << "[";
        for (std::size_t i = 0; i < v.size(); ++i) {
            os << v.data_[i];
            if (i + 1 < v.size()) os << ", ";
        }
        os << "]";
        return os;
    }

    // stream input: simple format: first read n, then n ints
    // Example input: 3 10 20 30  => Vec becomes [10, 20, 30]
    friend std::istream& operator>>(std::istream& is, Vec& v) {
        std::size_t n;
        if (!(is >> n)) return is;
        v.data_.assign(n, 0);
        for (std::size_t i = 0; i < n; ++i) {
            is >> v.data_[i];
        }
        return is;
    }
};

// A separate functor example: operator() on an object
struct Scaler {
    int factor;

    explicit Scaler(int f) : factor(f) {}

    // call operator: scale a Vec and return new Vec
    Vec operator()(Vec v) const {
        v *= factor;
        return v;
    }

    // overload operator() again: scale a single int
    int operator()(int x) const {
        return x * factor;
    }
};

int main() {
    Vec a{1, 2, 3};
    Vec b{10, 20, 30};

    // operator<<
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";

    // operator+ / operator- / operator* / operator/
    Vec c = a + b;
    std::cout << "a + b = " << c << "\n";

    Vec d = b - a;
    std::cout << "b - a = " << d << "\n";

    Vec e = a * 5;
    std::cout << "a * 5 = " << e << "\n";

    Vec f = 2 * a;
    std::cout << "2 * a = " << f << "\n";

    Vec g = b / 10;
    std::cout << "b / 10 = " << g << "\n";

    // operator+=, -=, *=, /=
    a += Vec{1, 1, 1};
    std::cout << "a after += [1,1,1] => " << a << "\n";

    b *= 2;
    std::cout << "b after *= 2 => " << b << "\n";

    // unary minus
    std::cout << "-a = " << (-a) << "\n";

    // operator[] (read/write)
    std::cout << "a[1] = " << a[1] << "\n";
    a[1] = 999;
    std::cout << "after a[1]=999, a = " << a << "\n";

    // operator() on Vec: sum range [l, r)
    std::cout << "a(0, 3) sum = " << a(0, 3) << "\n";

    // ++ prefix/postfix
    Vec before = a++;
    std::cout << "before a++ => " << before << ", after => " << a << "\n";
    ++a;
    std::cout << "after ++a => " << a << "\n";

    // comparisons: == and <=>
    Vec x{1, 2, 3};
    Vec y{1, 2, 4};
    std::cout << "x = " << x << ", y = " << y << "\n";
    std::cout << "x == y? " << (x == y ? "true" : "false") << "\n";
    std::cout << "x <  y? " << ((x < y) ? "true" : "false") << "\n"; // uses <=> automatically

    // explicit operator bool()
    Vec zero{0, 0, 0};
    if (zero) std::cout << "zero is true\n";
    else      std::cout << "zero is false (all zeros)\n";

    // functor: operator() in Scaler
    Scaler s3(3);
    std::cout << "s3(7) = " << s3(7) << "\n";
    std::cout << "s3(x) where x=" << x << " => " << s3(x) << "\n";

    // operator>> demo (optional): uncomment to test input
    // Vec in;
    // std::cout << "Enter: n then n ints (e.g. '3 9 8 7'): ";
    // std::cin >> in;
    // std::cout << "You entered: " << in << "\n";

    return 0;
}
