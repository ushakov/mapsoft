//#include "pointset.h"
#include "rect.h"
#include "color.h"
#include "image.h"
#include <iostream>
#include <set>
#include <list>


main(){

  Point::Point<int> p1(10,10), p2(20,15);
  std::cout << p1/3 << "\n" << p2+p1*2 << "\n";
  std::cout << Point::Rect<int>(p1,p2) << "\n";

  Color::RGB c1(128,128,0), c2(0xFF00FF);
  Color::RGBA a1(128,128,0,0), a2(0xFF00FF);
  std::cout << c1 << " " << c2 << " " << Color::RGB((int)c1) << "\n";
  std::cout << a1 << " " << a2 << " " << Color::RGBA((int)a1) << "\n";

  Image<Color::RGB> i1(100,100);
}
