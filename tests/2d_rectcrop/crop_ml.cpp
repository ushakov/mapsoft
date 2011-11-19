#include <iostream>
#include <string>
#include <cassert>

#include "2d/line_rectcrop.h"
#include "2d/line_polycrop.h"
#include "2d/line_utils.h"
#include "fig/fig.h"
#include <boost/lexical_cast.hpp>

// crop with stlitting lines

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
  L = boost::lexical_cast<iLine>(
   "-765,675,855,-585,1440,135,1170,450,855,405,585,-90,0,540,45,855,495,1170");

  // cropped line 1 - expected result
  CL1 = boost::lexical_cast<iLine>
    ("0,0,0,193,450,0,1000,0,1000,272,575,1000");

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
  iMultiLine ML = rect_crop_ml(cutter, L);

  o.pen_color=0xFF00FF;
  o.depth=51;
  o.thickness=2;
  for (iMultiLine::const_iterator i=ML.begin(); i!=ML.end(); i++){
    o.clear();
    o.iLine::operator=(*i);
    W.push_back(o);
  }

// add cropped line with closed=true
  ML = rect_crop_ml(cutter, L, true);
  o.pen_color=0xFF0000;
  o.depth=52;
  o.thickness=3;
  for (iMultiLine::const_iterator i=ML.begin(); i!=ML.end(); i++){
    o.clear();
    o.iLine::operator=(*i);
    o.close();
    W.push_back(o);
  }

  write("crop_ml.fig", W);

//  assert(ML.size()==3);
//  assert(ML[0] == boost::lexical_cast<iLine>("0,80,103,0"));
//  assert(ML[1] == boost::lexical_cast<iLine>("1000,426,855,405,634,0"));
//  assert(ML[2] == boost::lexical_cast<iLine>("502,0,0,540,45,855,253,1000"));
}
