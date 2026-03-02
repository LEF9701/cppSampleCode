#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <numeric>
#include <string>

// Y-combinator helper for recursive lambdas
template<typename Func>
class YCombinator {
    Func func;
public:
    explicit YCombinator(Func&& f) : func(std::forward<Func>(f)) {}

    template<typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return func(*this, std::forward<Args>(args)...);
    }
};

int main() {

    std::vector<int> data = {5, 3, 8, 3, 9, 1, 5, 3};

    // 1. Complex stateful comparator lambda
    std::unordered_map<int, int> frequency;

    std::for_each(data.begin(), data.end(),
        [&frequency](int x) { frequency[x]++; });

    std::sort(data.begin(), data.end(),
        [&, bias = 2](int a, int b) mutable {
            // Sort by frequency (descending)
            if (frequency[a] != frequency[b])
                return frequency[a] > frequency[b];

            // If same frequency, sort by value with dynamic bias
            bias++; // state changes each comparison
            return (a + bias) < (b + bias);
        });

    std::cout << "Sorted by frequency + bias:\n";
    for (auto v : data)
        std::cout << v << " ";
    std::cout << "\n\n";

    // 2. Lambda returning lambda (functional pipeline)
    auto make_pipeline = [](auto... functions) {
        return [=](auto input) {
            return ( ... | [&](auto f) {
                input = f(input);
                return input;
            } );
        };
    };

    auto add10 = [](int x) { return x + 10; };
    auto square = [](int x) { return x * x; };
    auto halve = [](int x) { return x / 2; };

    auto pipeline = make_pipeline(add10, square, halve);

    std::cout << "Pipeline result: " << pipeline(5) << "\n\n";

    // 3. Recursive lambda with memoization (Fibonacci)
    std::unordered_map<int, long long> memo;

    auto fib = YCombinator(
        [&memo](auto self, int n) -> long long {
            if (n <= 1) return n;
            if (memo.contains(n)) return memo[n];

            memo[n] = self(n - 1) + self(n - 2);
            return memo[n];
        }
    );

    std::cout << "Fib(40): " << fib(40) << "\n\n";

    // 4. Perfect forwarding generic lambda
    auto logger = [](auto&&... args) {
        ((std::cout << std::forward<decltype(args)>(args) << " "), ...);
        std::cout << "\n";
    };

    logger("Complex", "lambda", 2026, 3.1415);

    return 0;
}
