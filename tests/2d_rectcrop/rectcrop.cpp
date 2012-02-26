#include <iostream>
#include <string>
#include <cassert>

#include "2d/line_rectcrop.h"
#include "2d/line_utils.h"
#include "fig/fig.h"
#include <boost/lexical_cast.hpp>

// see also fig_mod/fig_crop program

using namespace fig;
main(){

  fig_world W;
  fig_object o = fig::make_object("2 1 0 1 1 7 50 -1 -1 0.000 0 0 -1 0 0 0");

  iRect cutter;
  iLine L, L1,L2,L3,L4, CL1,CL2,CL3,CL4;

// creating data

  // cutter
  cutter = boost::lexical_cast<iRect>("1000x1000+0+0");

  // initial line
  L = boost::lexical_cast<iLine>
    ("405,-720,-495,405,1395,-405,450,1215");
// add test line
  o.clear();
  o.pen_color=0;
  o.depth=49;
  o.thickness=1;
  o.iLine::operator=(L);
  W.push_back(o);

// add cutter rect
  o.clear();
  o.pen_color=0x0000FF;
  o.depth=50;
  o.thickness=1;
  o.iLine::operator=(rect2line(cutter));
  W.push_back(o);

// add cropped line with closed=false
  L1=L;
  rect_crop(cutter, L1, false);
  o.clear();
  o.pen_color=0xFF00FF;
  o.depth=51;
  o.thickness=2;
  o.iLine::operator=(L1);
  W.push_back(o);

// add cropped line with closed=true
  L2=L;
  rect_crop(cutter, L2, true);
  o.clear();
  o.pen_color=0x00FFFF;
  o.depth=52;
  o.thickness=3;
  o.iLine::operator=(L2);
  W.push_back(o);

  write("rectcrop1.fig", W);

  // cropped line 1 - expected result
  CL1 = boost::lexical_cast<iLine>
    ("0,0,0,193,450,0,1000,0,1000,272,575,1000");

  // cropped line 2 - expected result
  CL2 = boost::lexical_cast<iLine>
    ("421,0,0,0,0,193,450,0,1000,0,1000,272,575,1000,445,1000");

  assert(L1 == CL1);
  assert(L2 == CL2);

// second test
  W.clear();

  // initial line
  L = boost::lexical_cast<iLine>(
    std::string("540,-675,855,-585,1170,-315,1440,135,1440,360,1170,450,")+
    std::string("855,405,360,-135,45,-360,-270,-270,-270,135,-180,495,")+
    std::string("45,855,315,1125,495,1170"));


// add test line
  o.clear();
  o.pen_color=0;
  o.depth=49;
  o.thickness=1;
  o.iLine::operator=(L);
  W.push_back(o);

// add cutter rect
  o.clear();
  o.pen_color=0x0000FF;
  o.depth=50;
  o.thickness=1;
  o.iLine::operator=(rect2line(cutter));
  W.push_back(o);

// add cropped line with closed=false
  L1=L;
  rect_crop(cutter, L1, false);
  o.clear();
  o.pen_color=0xFF00FF;
  o.depth=51;
  o.thickness=2;
  o.iLine::operator=(L1);
  W.push_back(o);

// add cropped line with closed=true
  L2=L;
  rect_crop(cutter, L2, true);
  o.clear();
  o.pen_color=0x00FFFF;
  o.depth=52;
  o.thickness=3;
  o.iLine::operator=(L2);
  W.push_back(o);

  write("rectcrop2.fig", W);


  // cropped line 1 - expected result
  CL1 = boost::lexical_cast<iLine>(
    std::string("1000,0,1000,426,855,405,483,0,0,0,0,783,")+
    std::string("45,855,190,1000"));

  // cropped line 2 - expected result
  CL2 = boost::lexical_cast<iLine>(
    std::string("524,0,1000,0,1000,426,855,405,483,0,0,0,")+
    std::string("0,783,45,855,190,1000,499,1000"));

  assert(L1 == CL1);
  assert(L2 == CL2);

}
