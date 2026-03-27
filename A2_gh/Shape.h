#ifndef SHAPE_H
#define SHAPE_H

#include<iostream>
#include<string>

class Shape{
    protected:
        std::string name;
    public:
        Shape(const std::string& name);
        virtual ~Shape();
        virtual double getArea() const = 0;
        virtual void display() const = 0;
        const std::string& getName() const;
};

#endif