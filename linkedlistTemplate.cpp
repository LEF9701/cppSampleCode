#include <iostream>
#include <memory>

template<typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;

        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::unique_ptr<Node> head_;
    Node* tail_ = nullptr;   // raw pointer (non-owning)
    size_t size_ = 0;

public:
    SinglyLinkedList() = default;

    // Disable copy (because unique_ptr is non-copyable)
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    // Allow move
    SinglyLinkedList(SinglyLinkedList&&) noexcept = default;
    SinglyLinkedList& operator=(SinglyLinkedList&&) noexcept = default;

    void push_front(const T& value) {
        auto new_node = std::make_unique<Node>(value);
        
        if (!head_) {
            tail_ = new_node.get();
        } else {
            new_node->next = std::move(head_);
        }

        head_ = std::move(new_node);
        ++size_;
    }

    void push_back(const T& value) {
        auto new_node = std::make_unique<Node>(value);
        Node* new_tail = new_node.get();

        if (!head_) {
            head_ = std::move(new_node);
            tail_ = new_tail;
        } else {
            tail_->next = std::move(new_node);
            tail_ = new_tail;
        }

        ++size_;
    }

    void pop_front() {
        if (!head_) return;

        head_ = std::move(head_->next);

        if (!head_) {
            tail_ = nullptr;
        }

        --size_;
    }

    void print() const {
        Node* current = head_.get();
        while (current) {
            std::cout << current->data << " -> ";
            current = current->next.get();
        }
        std::cout << "nullptr\n";
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }
};

int main() {
    SinglyLinkedList<int> list;

    list.push_back(10);
    list.push_back(20);
    list.push_front(5);

    list.print();   // 5 -> 10 -> 20 -> nullptr

    list.pop_front();
    list.print();   // 10 -> 20 -> nullptr

    std::cout << "Size: " << list.size() << std::endl;
}
