#include <vector>
#include <iostream>
#include <cassert>

/*
This avoids temporaries by building an AST of expressions and evaluating once.
CRTP, operator overloading, lifetimes, and you can easily create dangling references if you get fancy.
*/
template<class E>
struct Expr {
    double operator[](size_t i) const { return static_cast<E const&>(*this)[i]; }
    size_t size() const { return static_cast<E const&>(*this).size(); }
};

struct Vec : Expr<Vec> {
    std::vector<double> a;
    explicit Vec(size_t n) : a(n) {}
    double operator[](size_t i) const { return a[i]; }
    double& operator[](size_t i) { return a[i]; }
    size_t size() const { return a.size(); }
};

template<class L, class R>
struct Add : Expr<Add<L,R>> {
    L const& l; R const& r;
    Add(L const& l, R const& r) : l(l), r(r) {}
    double operator[](size_t i) const { return l[i] + r[i]; }
    size_t size() const { return l.size(); }
};

template<class L, class R>
Add<L,R> operator+(Expr<L> const& l, Expr<R> const& r) {
    return Add<L,R>(static_cast<L const&>(l), static_cast<R const&>(r));
}

template<class E>
void assign(Vec& dst, Expr<E> const& e) {
    auto const& ex = static_cast<E const&>(e);
    assert(dst.size() == ex.size());
    for (size_t i = 0; i < dst.size(); ++i) dst[i] = ex[i];
}

int main() {
    Vec a(5), b(5), c(5);
    for (int i=0;i<5;i++){ a[i]=i; b[i]=10*i; }

    assign(c, a + b + a); // builds expression tree, evaluates once
    for (int i=0;i<5;i++) std::cout << c[i] << " ";
}
