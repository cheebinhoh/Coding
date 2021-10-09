/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 *
 */

#include <cstdio>
#include <string>
#include <memory>
#include <iostream>


// Start of Point
class Point
{
    int x = 0;
    int y = 0;

public:
    Point(int xVal = 1, int yVal = 1);
    Point(const Point & p);
    ~Point();

    int getX();
    int getY();

    Point & operator = (const Point & p);
    operator std::string() const;
    Point & operator ++ ();
    Point operator ++ (int);
};

Point::~Point()
{
    printf("Destroy of point: %s\n", std::string(*this).c_str());
}

Point Point::operator ++ (int)
{
   Point tmp = *this;

   x++;
   y++;

   return tmp;
}

Point & Point::operator ++ ()
{
   x++;
   y++;

   return *this;
}

Point::operator std::string() const
{
    return "x = " + std::to_string(x) + ", y = " + std::to_string(y);
}

Point::Point(const Point & p)
{
    printf("Copy constructor\n");

    x = p.x;
    y = p.y;
}

Point & Point::operator = (const Point & p)
{
    printf("---- Point::operator = \n");
    if ( this != &p )
    {
        x = p.x;
        y = p.y;
    }

    return *this;
}

std::ostream & operator << (std::ostream & o, Point & p)
{
    return o << "point x is " << p.getX() << ", point y is " << p.getY();
}

Point::Point(int xVal, int yVal)
{
    x = xVal;
    y = yVal;
}

int Point::getX()
{
    return x;
}

int Point::getY()
{
    return y;
}
// end of Point

// start of Rectangle
class Rectangle : public Point
{
    int width  = 0;
    int length = 0;

public:
    Rectangle(int x = 0, int y = 0, int w = 1, int l = 1);
    int getWidth() const;
    int getLength() const;

    Rectangle & operator = (const Rectangle & r);
};

Rectangle & Rectangle::operator = (const Rectangle & r)
{
    printf("---- Rectangle::operator = \n");

    if ( this != &r )
    {
        Point *thisPoint   = (Point *) this;
        const Point *point = (const Point *) &r;

        *thisPoint = *point;
        width      = r.width;
        length     = r.length;
    }

    return *this;
}

int Rectangle::getWidth() const
{
    return width;
}

int Rectangle::getLength() const
{
    return length;
}

Rectangle::Rectangle(int x, int y, int w, int l) : Point(x, y), width(w), length(l)
{
}

std::ostream & operator << (std::ostream & o, Rectangle & r)
{
    Point *p = &r;

    return o << "Rectangle " << *p << ", width is " << r.getWidth() << ", length is " << r.getLength();
}
// end of Rectangle

// utlities
void testPoint( Point p )
{
    std::cout << p << std::endl;
}

int main(int argc, char *argv[])
{
    std::unique_ptr<Point> p(new Point(10, 10));

    std::cout << "Value is " << std::string(*p) << std::endl;
    p.reset();

    std::cout << "Done." << std::endl;

    return 0;
}
