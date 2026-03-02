template<typename... Args>
class MyTuple;

template<>
class MyTuple<> {};

template<typename T, typename... Rest>
class MyTuple<T, Rest...> {
    T value;
    MyTuple<Rest...> rest;

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
