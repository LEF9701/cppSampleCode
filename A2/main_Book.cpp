#include <iostream>
#include "Book.h"

using namespace std;

// Function to demonstrate object passing by value (triggers copy constructor)
void displayBookByValue(Book book) {
    cout << "\n--- Inside displayBookByValue function ---" << endl;
    book.displayInfo();
    cout << "--- Exiting displayBookByValue function ---\n" << endl;
}

int main() {
    cout << "================================================" << endl;
    cout << "       LIBRARY BOOK MANAGEMENT SYSTEM TEST      " << endl;
    cout << "================================================\n" << endl;
    
    
    // Test 1: Default Constructor
    cout << "--- Test 1: Default Constructor ---" << endl;
    Book book1;
    book1.displayInfo();
    cout << endl;
    
    // Test 2: Parameterized Constructor
    cout << "--- Test 2: Parameterized Constructor ---" << endl;
    Book book2("1984", "George Orwell", "978-0-452-28423-4", 1949);
    book2.displayInfo();
    cout << endl;
    
    // Test 3: Setter Functions
    cout << "--- Test 3: Setter Functions ---" << endl;
    book1.setTitle("To Kill a Mockingbird");
    book1.setAuthor("Harper Lee");
    book1.setIsbn("978-0-06-112008-4");
    book1.setYearPublished(1960);
    cout << "After updating book1:" << endl;
    book1.displayInfo();
    cout << endl;
    
    // Test 4: Copy Constructor
    cout << "--- Test 4: Copy Constructor ---" << endl;
    Book book3(book2);
    book3.displayInfo();
    cout << endl;
    
    // Test 5: Copy Constructor via Function Parameter
    cout << "--- Test 5: Copy Constructor via Function Call ---" << endl;
    displayBookByValue(book2);
    cout << endl;
    
    // Test 6: Borrow and Return Operations
    cout << "--- Test 6: Borrow and Return Operations ---" << endl;
    cout << "Book 3 is borrowed: " << endl;
    book3.borrowBook();
    cout << book3.getTitle() << " is " << (book3.isBookAvailable()? "available" : "not available") << endl;
    cout << "Book 3 is returned: " << endl;
    book3.returnBook();
    cout << book3.getTitle() << " is " << (book3.isBookAvailable()? "available" : "not available") << endl;
    cout << endl;
    
    // Test 7: Dynamic Allocation
    cout << "--- Test 7: Dynamic Allocation ---" << endl;
    Book* bookPtr = new Book("Pride and Prejudice", "Jane Austen", "978-0-14-143951-8", 1813);
    bookPtr->displayInfo();
    cout << "\nDeleting dynamically allocated book..." << endl;
    delete bookPtr;
    cout << endl;
    
    return 0;
}
