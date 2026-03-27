#include<iostream>
#include<string>
#include"Shape.h"

Shape::Shape(const std::string& name): name(name){}
Shape::~Shape(){std::cout << "Shape " << getName() << " deleted." << std::endl;}
const std::string& Shape::getName() const{return name;}
