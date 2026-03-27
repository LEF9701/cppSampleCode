#include<iostream>
#include<string>
#include"Shape.h"

Shape::Shape(std::string name): name(name){}
Shape::~Shape(){std::cout << "Shape " << getName() << " deleted." << std::endl;}
std::string Shape::getName(){return name;}


