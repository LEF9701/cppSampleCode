#include<iostream>
#include<string>
#include"Shape.h"
#include"Triangle.h"

Triangle::Triangle(double b, double h): Shape("Triangle"), base(b), height(h){}
Triangle::~Triangle(){std::cout << "Triangle deleted." << std::endl;}
double Triangle::getArea() const{return (base*height)/2;}
void Triangle::display() const{
    std::cout << "Triangle: base = " << base << ", height = " << height << ", area = " << getArea() << std::endl;
}
