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
        Book(const std::string& title, const std::string& author, const std::string& isbn, int year);
        Book(const Book& other);
        ~Book();
        const std::string& getTitle() const;
        const std::string& getAuthor() const;
        const std::string& getIsbn() const;
        int getYearPublished() const;
        bool isBookAvailable() const;
        void setTitle(const std::string& title_);
        void setAuthor(const std::string& author_);
        void setIsbn(const std::string& isbn_);
        void setYearPublished(int year);
        void borrowBook();
        void returnBook();
        void displayInfo() const;
};

#endif