/* Copyright © 2021 Chee Bin HOH. All rights reserved.
 *
 * Test program for c++ snippest to try to out some features.
 *
 */

#include <cstdio>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// template
template <typename T> const T &maxof(const T &arg1, const T &arg2) {
  return (arg1 > arg2) ? arg1 : arg2;
}

template <typename T> T pi = T(1.23456);

// Start of Point
class Point {
  int x = 0;
  int y = 0;

public:
  Point(int xVal = 1, int yVal = 1);
  Point(const Point &p);
  Point(Point &&p);
  ~Point();

  int getX();
  int getY();

  void reset();
  Point &operator=(const Point &p);
  Point &operator=(Point &&p);
  operator std::string() const;
  Point &operator++();
  Point operator++(int);
  Point operator-();
};

Point Point::operator-() {
  Point newP(-(x), -(y));

  return newP;
}

void Point::reset() {
  x = 0;
  y = 0;
}

Point::~Point() { printf("Destroy of point %s\n", std::string(*this).c_str()); }

Point Point::operator++(int) {
  Point tmp = *this;

  x++;
  y++;

  return tmp;
}

Point &Point::operator++() {
  x++;
  y++;

  return *this;
}

Point::operator std::string() const {
  return "Point " + std::to_string(x) + ", y = " + std::to_string(y);
}

Point::Point(const Point &p) {
  printf("Copy constructor\n");

  x = p.x;
  y = p.y;
}

Point &Point::operator=(Point &&p) {
  printf("Move assignment\n");

  if (&p != this) {
    x = std::move(p.x);
    y = std::move(p.y);

    p.reset();
  }

  return *this;
}

Point &Point::operator=(const Point &p) {
  printf("---- Point::operator = \n");
  if (this != &p) {
    x = p.x;
    y = p.y;
  }

  return *this;
}

std::ostream &operator<<(std::ostream &o, Point &p) {
  return o << "point x is " << p.getX() << ", point y is " << p.getY();
}

Point::Point(Point &&p) {
  printf("Move constructor\n");
  x = std::move(p.x);
  y = std::move(p.y);

  p.reset();
}

Point::Point(int xVal, int yVal) {
  x = xVal;
  y = yVal;
}

int Point::getX() { return x; }

int Point::getY() { return y; }
// end of Point

// start of Rectangle
class Rectangle : public Point {
  int width = 0;
  int length = 0;

public:
  Rectangle(int x = 0, int y = 0, int w = 1, int l = 1);
  int getWidth() const;
  int getLength() const;

  Rectangle &operator=(const Rectangle &r);
};

Rectangle &Rectangle::operator=(const Rectangle &r) {
  printf("---- Rectangle::operator = \n");

  if (this != &r) {
    Point *thisPoint = (Point *)this;
    const Point *point = (const Point *)&r;

    *thisPoint = *point;
    width = r.width;
    length = r.length;
  }

  return *this;
}

int Rectangle::getWidth() const { return width; }

int Rectangle::getLength() const { return length; }

Rectangle::Rectangle(int x, int y, int w, int l)
    : Point(x, y), width(w), length(l) {}

std::ostream &operator<<(std::ostream &o, Rectangle &r) {
  Point *p = &r;

  return o << "Rectangle " << *p << ", width is " << r.getWidth()
           << ", length is " << r.getLength();
}
// end of Rectangle

class S {
  std::string _s = "";

public:
  S(const char *s);
  std::string getStringValue();
  operator std::string() const;

  void transform(char (*)(char));
};

void S::transform(char (*f)(char)) {
  int i = 0;

  for (auto c : _s) {
    c = (*f)(c);
    _s[i] = c;

    i++;
  }
}

S::S(const char *s) : _s(s) {}

S::operator std::string() const { return _s; }

std::string S::getStringValue() { return _s; }

// utlities
void testPoint(Point p) { std::cout << p << std::endl; }

std::ostream &operator<<(std::ostream &o, S &s) { return o << std::string(s); }

char convertToUpper(char c) { return toupper(c); }

int main(int argc, char *argv[]) {
  Point p(5, 5);

  Point p2 = -p;
  std::cout << "The point is " << std::string(p2) << std::endl;

  /*
  std::forward_list<int>           list = {1, 2, 3, 4, 5};
  std::forward_list<int>::iterator iter;

  for ( iter = list.begin(); iter != list.end(); iter++ )
  {
      std::cout << *iter << ", ";
  }

  std::cout << std::endl;
  std::cout << "max value = " << maxof<int>(2, 3) << std::endl;
  */

  /*
  std::ostream_iterator<int> oiter(std::cout, " " );

  for ( auto i : {1, 2, 3, 4, 5 } )
  {
      *oiter++ = i;
  }

  std::cout << std::endl;
  */

  /*
  std::cout << "Enter two values: " << std::flush;

  std::istream_iterator<int> iiter(std::cin);
  std::cin.clear();


  std::cout << "first value: " << *iiter++ << std::endl;
  std::cout << "second value: " << *iiter << std::endl;
  */

  /*
  Point p1(5, 5);
  Point p2 = p1;
  Point p3(3, 3);


  std::cout << "Point 1: " << p1 << std::endl;
  std::cout << "Point 2: " << p2 << std::endl;
  std::cout << "Point 3: " << p3 << std::endl;

  std::cout << std::endl;
  p2 = std::move(p3);
  std::cout << "Point 2: " << p2 << std::endl;

  Point p4 = std::move(p2);
  std::cout << "Point 2: " << p2 << std::endl;
  std::cout << "Point 4: " << p4 << std::endl;
  */
  /*
  std::unique_ptr<Point> p(new Point(10, 10));

  std::cout << "Value is " << std::string(*p) << std::endl;
  p.reset();

  std::cout << "Done." << std::endl;

  S str("hoh chee bin");
  str.transform([](char c) -> char { return toupper(c); });

  std::cout << str << std::endl;

  std::vector<int> v = {};
  for ( int i = 0; i < 5; i++ )
  {
      v.insert(v.begin() + i, i);
  }

  while ( ! v.empty() )
  {
      int val = v.back();
      std::cout << val << " ";

      v.pop_back();
  }

  std::cout << std::endl;
  */

  return 0;
}
