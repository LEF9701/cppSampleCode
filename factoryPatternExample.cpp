#include <iostream>
#include <memory>
#include <string>

// 1️⃣ Product Interface
class Shape {
public:
    virtual void draw() const = 0;   // pure virtual
    virtual ~Shape() = default;      // virtual destructor (IMPORTANT)
};

// 2️⃣ Concrete Products

class Rectangle : public Shape {
public:
    void draw() const override {
        std::cout << "Inside Rectangle::draw() method.\n";
    }
};

class Circle : public Shape {
public:
    void draw() const override {
        std::cout << "Inside Circle::draw() method.\n";
    }
};

class Square : public Shape {
public:
    void draw() const override {
        std::cout << "Inside Square::draw() method.\n";
    }
};

// 3️⃣ Factory
class ShapeFactory {
public:
    static std::unique_ptr<Shape> getShape(const std::string& shapeType) {
        if (shapeType == "CIRCLE")
            return std::make_unique<Circle>();
        else if (shapeType == "RECTANGLE")
            return std::make_unique<Rectangle>();
        else if (shapeType == "SQUARE")
            return std::make_unique<Square>();
        else
            return nullptr;
    }
};

// 4️⃣ Client
int main() {

    auto shape1 = ShapeFactory::getShape("CIRCLE");
    shape1->draw();

    auto shape2 = ShapeFactory::getShape("RECTANGLE");
    shape2->draw();

    auto shape3 = ShapeFactory::getShape("SQUARE");
    shape3->draw();

    return 0;
}
