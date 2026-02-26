#include<iostream>
#include <memory>
#include <stdexcept>

template<typename T>
class MyVector{
private:
    std::unique_ptr<T[]> data_;
    size_t size_{0};
    size_t capacity_{0};

    void reallocate(size_t new_capacity){
        auto new_data = std::make_unique<T[]>(new_capacity);
        for(size_t i = 0; i < size_; ++i){
            new_data[i] = std::move(data_[i]);
        }
        data_ = std::move(new_data);
        capacity_ = new_capacity;
    }

public:
    // Default constructor
    MyVector() = default;

    // Copy constructor (deep copy)
    MyVector(const MyVector& other)
        : data_(other.capacity_ ? std::make_unique<T[]>(other.capacity_) : nullptr),
          size_(other.size_),
          capacity_(other.capacity_)
    {
        for(size_t i = 0; i < size_; ++i){
            data_[i] = other.data_[i];
        }
    }

    // Copy assignment (deep copy)
    MyVector& operator=(const MyVector& other){
        if(this == &other) return *this;

        auto new_data = other.capacity_ ? std::make_unique<T[]>(other.capacity_) : nullptr;

        for(size_t i = 0; i < other.size_; ++i){
            new_data[i] = other.data_[i];
        }

        data_ = std::move(new_data);
        size_ = other.size_;
        capacity_ = other.capacity_;

        return *this;
    }

    // Move constructor
    MyVector(MyVector&& other) noexcept
        : data_(std::move(other.data_)),
          size_(other.size_),
          capacity_(other.capacity_)
    {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Move assignment
    MyVector& operator=(MyVector&& other) noexcept{
        if(this == &other) return *this;

        data_ = std::move(other.data_);
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }

    void push_back(const T& value){
        if(size_ == capacity_){
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            reallocate(new_capacity);
        }
        data_[size_++] = value;
    }

    void pop_back(){
        if(size_ == 0){
            throw std::out_of_range("Vector is empty");
        }
        --size_;
    }

    T& operator[](size_t index){
        if(index >= size_){
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const T& operator[](size_t index) const{
        if(index >= size_){
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    size_t size() const{
        return size_;
    }
};


int main(){
    MyVector<int> v;

    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    for(size_t i = 0; i < v.size(); i++){
        std::cout << v[i] << " ";
    }


    return 0;
}
