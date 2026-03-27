#include<iostream>
#include<string>
#include<cmath>
#include"Shape.h"
#include"Circle.h"
//constexpr double PI = 3.14159265358979;

Circle::Circle(double r): Shape("Circle"), radius(r){}
Circle::~Circle(){std::cout << "Circle deleted." << std::endl;}
double Circle::getArea() const{return radius*radius*M_PI;}
void Circle::display() const{
    std::cout << "Circle: radius = " << radius << ", area = " << getArea() << std::endl;
}
