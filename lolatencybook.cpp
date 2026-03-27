#include <iostream>
#include <array>

struct Order {
    int id;     // order ID
    int price;  // order price
    int qty;    // order quantity
};

class OrderBook {
private:
    static constexpr std::size_t MAX_ORDERS = 100; // fixed maximum number of orders
    std::array<Order, MAX_ORDERS> orders{};        // fixed-size storage, avoids dynamic allocation
    std::size_t count = 0;                         // current number of stored orders

public:
    bool addOrder(int id, int price, int qty) {
        if (count >= MAX_ORDERS) { // reject if buffer is already full
            return false;
        }

        orders[count++] = Order{id, price, qty}; // store new order, then increase count
        return true; // insertion succeeded
    }

    int totalQuantityAtPrice(int targetPrice) const {
        int total = 0; // accumulate total quantity for matching price

        for (std::size_t i = 0; i < count; ++i) { // only scan valid stored orders
            if (orders[i].price == targetPrice) { // check whether price matches target
                total += orders[i].qty;           // add matching order quantity
            }
        }

        return total; // return total quantity at that price
    }

    void show() const {
        for (std::size_t i = 0; i < count; ++i) { // print all stored orders
            std::cout << "id=" << orders[i].id
                      << " price=" << orders[i].price
                      << " qty=" << orders[i].qty << '\n';
        }
    }
};

int main() {
    OrderBook book; // create one order book object

    book.addOrder(1, 100, 50); // add order: id=1, price=100, qty=50
    book.addOrder(2, 101, 30); // add order: id=2, price=101, qty=30
    book.addOrder(3, 100, 20); // add order: id=3, price=100, qty=20

    book.show(); // display all orders

    std::cout << "Total qty at price 100 = "
              << book.totalQuantityAtPrice(100) << '\n'; // calculate total quantity at price 100

    return 0; // normal program termination
}

If you want, I can also make the comments:
more concise, more practical, or more exam-style.
