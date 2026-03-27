#ifndef SHAPE_H
#define SHAPE_H

#include<iostream>
#include<string>

class Shape{
    protected:
        std::string name;
    public:
        Shape(std::string name);
        virtual ~Shape();
        virtual double getArea() = 0;
        virtual void display() = 0;
        std::string getName();
};

#endif
