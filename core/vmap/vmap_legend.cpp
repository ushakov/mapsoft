#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "vmap.h"
#include "zn.h"
#include "zn_lists.h"
#include "2d/line_utils.h"

using namespace std;
namespace vmap{

// сделать пример объекта, размером 5х1
vmap::object
make_sample(int type){
  double d=0.3;

  vmap::object o;
  dLine l;
  if (type >= zn::area_mask){
    l.push_back(dPoint(0,   0));
    l.push_back(dPoint(5,   0));
    l.push_back(dPoint(5-d, 1));
    l.push_back(dPoint(d, 1-d));
  }
  else if (type >= zn::line_mask){
    l.push_back(dPoint(0, 0));
    l.push_back(dPoint(4, 0));
    l.push_back(dPoint(5, 1));
  }
  else{
    l.push_back(dPoint(2.5,  0.5));
  }
  o.type=type;
  o.text = "text";
  o.push_back(l);
  return o;
}


// список всех знаков
vmap::world
make_legend(string style){
  zn::zn_conv z(style);
  world ret;
  double sc=0.002;

  int count=0;
  map<int, zn::zn>::const_iterator i;
  for (i = z.znaki.begin(); i!=z.znaki.end(); i++){
    vmap::object o = make_sample(i->first);
    o+=dPoint(0, count*2);
    o*=sc;
    o.comm.push_back(i->second.name);
    if (i->second.desc.length()) o.comm.push_back(i->second.desc);
    ret.push_back(o);
    count++;
  }
  ret.rscale = 100000;
  ret.brd = rect2line(dRect(-0.5,-0.5,8,2*count+2) * sc);
  return ret;
}

} // namespace

