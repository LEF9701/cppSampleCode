#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "Shape.h"

class Triangle : public Shape{
    private:
        double base;
        double height;
    public:
        Triangle(double b, double h);
        ~Triangle();
        double getArea() const override;
        void display() const override;
};

#endif