#include "pointset.h"
#include "rect.h"
#include <iostream>
#include <set>
#include <list>


main(){

  Point::Point<int> p1(10,10), p2(20,15);
  std::cout << p1/3 << "\n" << p2+p1 << "\n";
  std::cout << Point::Rect<int>(p1,p2) << "\n";
}
