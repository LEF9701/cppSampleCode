#include<cstddef>
#include<iostream>


// primary template declaration
template<typename... Args>
class MyTuple;

// base case
template<>
class MyTuple<> {};

// partial specialisation
template<typename T, typename... Rest>
class MyTuple<T, Rest...> {
    T value;
    MyTuple<Rest...> rest;

    // TupleGet requires access to private members of MyTuple<T, Rest...>
    // for this MyTuple<...> specialization, all TupleGet<I, Something> specializations are friends (so they can access private members)
    template<size_t, typename> friend struct TupleGet;

public:
    MyTuple(T v, Rest... r)
        : value(v), rest(r...) {}
};

/*
if you do: MyTuple<int, double, char> t(1, 3.14, 'A');

it becomes:

MyTuple<int, double, char>
    contains:
        int
        MyTuple<double, char>
            contains:
                double
                MyTuple<char>
                    contains:
                        char
                        MyTuple<>

*/

template<size_t I, typename Tuple>
struct TupleGet;

// Base case: index 0 returns the current value
template<typename T, typename... Rest>
struct TupleGet<0, MyTuple<T, Rest...>> {
    static T& get(MyTuple<T, Rest...>& t) {
        return t.value;
    }
};

// Recursive case: peel off one element
template<size_t I, typename T, typename... Rest>
struct TupleGet<I, MyTuple<T, Rest...>> {
    static auto& get(MyTuple<T, Rest...>& t) {
        return TupleGet<I - 1, MyTuple<Rest...>>::get(t.rest);
    }
};

/*
So get<2>(t) expands like:
TupleGet<2, MyTuple<int,double,char>>::get(t)
-> calls TupleGet<1, MyTuple<double,char>>::get(t.rest)
-> calls TupleGet<0, MyTuple<char>>::get(t.rest.rest)
-> returns char& from that node’s value
*/

// Convenience function(just a wrapper)
template<size_t I, typename... Args>
auto& get(MyTuple<Args...>& t) {
    return TupleGet<I, MyTuple<Args...>>::get(t);
}



int main(){
    MyTuple<int, double, char> t(1, 3.14, 'a');
    decltype(auto) id_0 = get<0>(t); // 1
    decltype(auto) id_1 = get<1>(t); // 3.14
    decltype(auto) id_2 = get<2>(t); // 'a'
    std::cout << "get<0>t : " << id_0 << std::endl;
    std::cout << "get<1>t : " << id_1 << std::endl;
    std::cout << "get<2>t : " << id_2 << std::endl;

    return 0;    
}
