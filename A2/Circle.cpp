#include<iostream>
#include<string>
//constexpr double PI = 3.14159265358979;
#include<cmath>
#include"Shape.h"
#include"Circle.h"

Circle::Circle(double r): Shape("Circle"), radius(r){}
Circle::~Circle(){std::cout << "Circle deleted." << std::endl;}
double Circle::getArea(){return radius*radius*M_PI;}
void Circle::display(){
    std::cout << "Circle: radius = " << radius << ", area = " << getArea() << std::endl; 
}
