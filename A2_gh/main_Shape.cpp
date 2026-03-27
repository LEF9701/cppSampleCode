#include <iostream>
#include <iomanip>
#include "Shape.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Triangle.h"
#include <vector>
using namespace std;

// Demonstrate polymorphism with pointer
void printShape(Shape* s) {
    cout << "Processing: " << s->getName() << endl;
    cout << "Area: " << s->getArea() << endl;
    s->display();
    cout << endl;
}

int main() {
    cout << fixed << setprecision(2);
    
    cout << "========================================" << endl;
    cout << "       SHAPE DRAWING SYSTEM TEST       " << endl;
    cout << "========================================" << endl;
    
    // Test 1: Create individual shapes
    cout << "\n--- Test 1: Individual Shapes ---" << endl;
    Circle c(5.0);
    Rectangle r(4.0, 6.0);
    Triangle t(3.0, 8.0);
    
    c.display();
    r.display();
    t.display();
    
    // Test 2: Polymorphism with pointers
    cout << "\n--- Test 2: Polymorphism with Pointers ---" << endl;
    vector<Shape*> vShape;
    vShape.push_back(new Circle(3.0));
    vShape.push_back(new Rectangle(5.0, 4.0));
    vShape.push_back(new Triangle(6.0, 3.0));
    
    for (auto x : vShape) {
        printShape(x);
    }
    
    // Cleanup
    cout << "--- Deleting shapes ---" << endl;
    for (auto x : vShape) {
        delete x;
    }
    
    cout << "\n========================================" << endl;
    cout << "            END OF TEST                " << endl;
    cout << "========================================" << endl;
    
    return 0;
}