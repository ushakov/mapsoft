//#include "pointset.h"
#include "rect.h"
#include "pointset.h"
#include "color.h"
#include "cache.h"
#include "image.h"
#include <iostream>
#include <set>
#include <list>


main(){

  Point<int> p1(10,10), p2(20,15);
  std::cout << p1/3 << "\n" << p2+p1*2 << "\n";
  std::cout << Rect<int>(p1,p2) << "\n";

  RGB c1(128,128,0), c2(0xFF00FF);
  RGBA a1(128,128,0,0), a2(0xFF00FF);
  std::cout << c1 << " " << c2 << " " << RGB((int)c1) << "\n";
  std::cout << a1 << " " << a2 << " " << RGBA((int)a1) << "\n";

  Image<RGBA> i1(100,100,RGB(0,10,10));
  std::cout << i1 << "\n";
  i1.clip_window(Rect<int>(10,10,120,80));
  std::cout << i1 << "\n";
  std::cout << fast_resize(i1,2) << "\n";

  Cache<Point<int>, int> C(10);
  C.add(Point<int>(0,1), 100);
  C.add(Point<int>(2,1), 100);
  C.add(Point<int>(3,3), 200);
  C.add(Point<int>(2,2), 200);
  C.add(Point<int>(2,2), 100);
 std::cout << C;
  
}


