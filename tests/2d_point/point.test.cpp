#include <iostream>
#include <sstream>
#include <cassert>
#include "2d/point.h"
#include "2d/line.h"

using namespace std;

int main() {

  // reading points
  iPoint p;
  {
    std::stringstream s("10,11"); s >> p;
    assert(p == iPoint(10,11));
    assert(s.eof() == true);
    assert(s.fail() == false);
  }

  {
    std::stringstream s("10,11a"); s >> p;
    assert(p == iPoint(10,11));
    assert(s.eof() == false);
    assert(s.fail() == false);
  }

  {
    std::stringstream s("10a,11a"); s >> p;
    assert(p == iPoint(10,11));
    assert(s.eof() == false);
    assert(s.fail() == true);
  }

  {
    std::stringstream s("10,11 ");
    s >> p;
    assert(p == iPoint(10,11));
    assert(s.eof() == true);
    assert(s.fail() == false);
  }

  {
    std::stringstream s("10 , 11 11  ,12 ");
    s >> p;
    assert(p == iPoint(10,11));
    assert(s.eof() == false);
    assert(s.fail() == false);
    s >> p;
    assert(p == iPoint(11,12));
    assert(s.eof() == true);
    assert(s.fail() == false);
  }

  // reading lines
  iLine l;
  {
    std::stringstream s("10,11");
    s >> l;
    assert(l.size() == 1);
    assert(l[0] == iPoint(10,11));
    assert(s.eof() == true);
    assert(s.fail() == false);
  }

  {
    l = iLine();
    std::stringstream s("10,11,12 , 13 ");
    s >> l;
    assert(l.size() == 2);
    assert(l[0] == iPoint(10,11));
    assert(l[1] == iPoint(12,13));
    assert(s.eof() == true);
    assert(s.fail() == false);
  }

}
