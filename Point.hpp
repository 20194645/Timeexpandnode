#ifndef POINT_HPP
#define POINT_HPP
#include <iostream>
using namespace std;

class Point
{
public:
    double x;
    double y;

    void setPoint(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    double getX()
    {
        return x;
    }
    double getY()
    {
        return y;
    }

    void printPoint()
    {
        cout << this->getX() << " " << this->getY() << endl;
    }

    bool equals(Point *p)
    {
        return (this->x == p->x) && (this->y == p->y);
    }
};
#endif 