#include<cstddef>
#include<iostream>


template<typename... Args>
class MyTuple;

template<>
class MyTuple<> {};

template<typename T, typename... Rest>
class MyTuple<T, Rest...> {
    T value;
    MyTuple<Rest...> rest;

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

// Base case: index 0 returns the current value
template<size_t I, typename Tuple>
struct TupleGet;

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

// Convenience function
template<size_t I, typename... Args>
auto& get(MyTuple<Args...>& t) {
    return TupleGet<I, MyTuple<Args...>>::get(t);
}



int main(){
    MyTuple<int, double, char> t(1, 3.14, 'a');
    auto id_0 = get<0>(t); // 1
    auto id_1 = get<1>(t); // 3.14
    auto id_2 = get<2>(t); // 'a'
    std::cout << "get<0>t : " << id_0 << std::endl;
    std::cout << "get<1>t : " << id_1 << std::endl;
    std::cout << "get<2>t : " << id_2 << std::endl;

    return 0;    
}
