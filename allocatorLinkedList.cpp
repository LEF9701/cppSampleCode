#include <iostream>
#include <memory>   // std::allocator
#include <utility>  // std::swap

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;

        Node(const T& value) : data(value), next(nullptr) {}
    };

    using Alloc = std::allocator<Node>;
    Alloc alloc;

    Node* head = nullptr;

    // helper: destroy entire list
    void clear() {
        Node* current = head;
        while (current) {
            Node* temp = current->next;

            std::allocator_traits<Alloc>::destroy(alloc, current);
            alloc.deallocate(current, 1);

            current = temp;
        }
        head = nullptr;
    }

public:
    LinkedList() = default;

    // Copy constructor (deep copy)
    LinkedList(const LinkedList& other) {
        Node* current = other.head;
        Node** tail = &head;

        while (current) {
            Node* newNode = alloc.allocate(1);
            std::allocator_traits<Alloc>::construct(
                alloc, newNode, current->data
            );

            *tail = newNode;
            tail = &((*tail)->next);

            current = current->next;
        }
    }

    // Copy assignment (copy-and-swap)
    LinkedList& operator=(LinkedList other) {
        std::swap(head, other.head);
        return *this;
    }

    // Destructor
    ~LinkedList() {
        clear();
    }

    void push_front(const T& value) {
        Node* newNode = alloc.allocate(1);

        std::allocator_traits<Alloc>::construct(
            alloc, newNode, value
        );

        newNode->next = head;
        head = newNode;
    }

    void display() const {
        Node* current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << "\n";
    }
};

int main() {
    LinkedList<int> list;

    list.push_front(10);
    list.push_front(20);
    list.push_front(30);

    list.display();

    LinkedList<int> copy = list;
    copy.display();

    return 0;
}
