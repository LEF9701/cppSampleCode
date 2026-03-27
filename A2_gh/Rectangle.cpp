#include<iostream>
#include<string>
#include"Shape.h"
#include"Rectangle.h"

Rectangle::Rectangle(double w, double h): Shape("Rectangle"),width(w), height(h){}
Rectangle::~Rectangle(){std::cout << "Rectangle deleted." << std::endl;}
double Rectangle::getArea() const{return width * height;}
void Rectangle::display() const{
    std::cout << "Rectangle: width = " << width << ", height = " << height << ", area = " << getArea() << std::endl;
}
