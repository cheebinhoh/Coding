#include <iostream>

struct Point {
  int x{};
  int y{};
};

int main(int argc, char *argv[]) {
  Point p{};

  p.x = 100;
  std::cout << "x: " << p.x << "\n";
  std::cout << "y: " << p.y << "\n";

  int(Point::*memberPtr);

  memberPtr = &Point::x;
  p.*memberPtr = 200;

  memberPtr = &Point::y;
  p.*memberPtr = 200;

  std::cout << "x: " << p.x << "\n";
  std::cout << "y: " << p.y << "\n";

  return 0;
}
