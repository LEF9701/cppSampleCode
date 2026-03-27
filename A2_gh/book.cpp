#include "Book.h"
#include<string>
#include<iostream>

Book::Book()
        :title("Unknown"), author("Unknown"), isbn("000-0-00-000000-0"), yearPublished(0), isAvailable(true){
            std::cout << "Default constructor called. Book created." << std::endl;
        }
Book::Book(const std::string& title, const std::string& author, const std::string& isbn, int year)
        :title(title), author(author), isbn(isbn), yearPublished(year),isAvailable(true){
            std::cout << "Parameterized constructor called. Book '" << getTitle() << "' created." <<std::endl;
        }
Book::Book(const Book& other)
        :title(other.title), author(other.author), isbn(other.isbn), yearPublished(other.yearPublished), isAvailable(other.isAvailable){
            std::cout << "Copy constructor called. Book '" << getTitle() << "' copied." <<std::endl;
        }
Book::~Book(){
    std::cout << "Destructor called. Book '" << getTitle() << "' destroyed." <<std::endl;
}
const std::string& Book::getTitle() const{return title;}
const std::string& Book::getAuthor() const{return author;}
const std::string& Book::getIsbn() const{return isbn;}
int Book::getYearPublished() const{return yearPublished;}
bool Book::isBookAvailable() const{return isAvailable;}
void Book::setTitle(const std::string& title_){title = title_;}
void Book::setAuthor(const std::string& author_){author = author_;}
void Book::setIsbn(const std::string& isbn_){isbn = isbn_;}
void Book::setYearPublished(int year){yearPublished = year;}
void Book::borrowBook(){
    isAvailable = false;
    std::cout << "Book '" << getTitle() << "' has been borrowed successfully." << std::endl;
}
void Book::returnBook(){
    isAvailable = true;
    std::cout << "Book '" << getTitle() << "' has been returned successfully." << std::endl;
}
void Book::displayInfo() const{
    std::cout << "=========================================" << std::endl;
    std::cout << "Book Information:" << std::endl;
    std::cout << "Title:\t\t" << getTitle() << std::endl;
    std::cout << "Author:\t\t" << getAuthor() << std::endl;
    std::cout << "ISBN:\t\t" << getIsbn() << std::endl;
    std::cout << "Year Published:\t" << getYearPublished() << std::endl;
    std::cout << "Availability:\t" << (isBookAvailable() ? "Available": "Unavailable") << std::endl;
    std::cout << "=========================================" << std::endl;
}
