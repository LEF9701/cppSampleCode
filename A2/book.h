#ifndef BOOK_H
#define BOOK_H

#include<string>
#include<iostream>

class Book{
    private:
        std::string title;
        std::string author;
        std::string isbn;
        int yearPublished;
        bool isAvailable;

    public:
        Book();
        Book(std::string title, std::string author, std::string isbn, int year);
        Book(const Book& other);
        ~Book();
        std::string getTitle();
        std::string getAuthor();
        std::string getIsbn();
        int getYearPublished();
        bool isBookAvailable();
        void setTitle(std::string title_);
        void setAuthor(std::string author_);
        void setIsbn(std::string isbn_);
        void setYearPublished(int year);
        void borrowBook();
        void returnBook();
        void displayInfo();
};

#endif
