#include<iostream>
#include<vector>

template<typename T>
class MyVector{
    private:
        T* data_;
        size_t size_;
        size_t capacity_;

        void reallocate(size_t new_capacity){
            T* new_data = new T[new_capacity];
            for(size_t i = 0; i < size_; i++){
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }

    public:
        MyVector(): data_(nullptr), size_(0), capacity_(0) {}
        ~MyVector(){ delete[] data_; }

        void push_back(const T& value){
            if(size_ == capacity_){
                size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
                reallocate(new_capacity);
            }
            data_[size_] = value;
            ++size_;
        }

        void pop_back(){
            if(size_ == 0){ 
                throw std::out_of_range("Vector is empty");
            }
            --size_;
        }

        T& operator[](size_t index) const{
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
