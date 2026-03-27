#ifndef CIRCLE_H
#define CIRCLE_H
#include "Shape.h"

class Circle : public Shape{
    private:
        double radius;
    public:
        Circle(double r);
        ~Circle();
        double getArea() const override;
        void display() const override;
};

#endif