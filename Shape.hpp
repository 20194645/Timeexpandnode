#ifndef Shape_HPP
#define Shape_HPP
#include <iostream>
#include <string>
#include "Point.hpp"
using namespace std;

class Shape
{
public:
    Point *start;
    Point *end;
    double d = 1;
    double time = 0;
    string name;

    void setName(string name)
    {
        this->name = name;
        if(name.find("E") == std::string::npos
             && name.find("J") == std::string::npos){
            cout<<"DEBUG: "<<__FILE__<<":"<<__LINE__<<":"<<endl;
        }
    }

    void setPoint(Point *start, Point *end)
    {
        this->start = start;
        this->end = end;
    }

    bool equals(Shape *s)
    {
        return ((this->start)->equals(s->start) && (this->end)->equals(s->end));
    }
    virtual double getWeight( ){
	 if(time == 0){
		//time = this->d/Constant::v ; //Với v là hằng số của lớp Constant
        time = 1;
	 }	
	 return time;
    }

};
#endif 