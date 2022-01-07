#include <iostream>

using namespace std;

class Box {
public:
  double length;  // Length of a box
  double breadth; // breadth of a box
  double height;  // height of a box

  Box(void);
  ~Box(void);

  double getVolume();
  //        double getVolume() {
  //
  //           return length * breadth * height;
  //	}
};

Box::Box(void) { cout << "constructor" << endl; }

Box::~Box(void) { cout << "destructor" << endl; }

double Box::getVolume() { return length * breadth * height; }

int main(int argc, char *argv[]) {
  Box box1;
  Box box2;
  double volume = 0.0;
  int *iPtr;

  iPtr = new int;
  *iPtr = 10;

  box1.height = 5.0;
  box1.length = 6.0;
  box1.breadth = 7.0;

  cout << *iPtr << endl;

  return 0;
}
