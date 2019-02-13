#include <iostream>
#include <string>
#include <cassert>

#include <list>

#include "2d/point.h"
#include "fig/fig.h"
#include <boost/lexical_cast.hpp>


template <typename T>
Line<T> border_of_points(const Line<T> & points){
  Line<T> ret;
  if (points.size()==0) return ret;
  typename Line<T>::const_iterator p, p0, p1;

  // find point with minimal x
  p0=points.begin();
  for (p=points.begin(); p!=points.end(); p++){
    if (p0->x > p->x) p0=p;
  }

  dPoint v0(0,1);

  do {
    ret.push_back(*p0);
    p1=p0;
    p1++; if (p1==points.end()) p1=points.begin();
    if (p0==p1) break;

    // find point with maximal v*norm(p-p0)
    double cmax=-1;
    for (p=points.begin(); p!=points.end(); p++){
      if (p == p0) continue;
      dPoint v=pnorm(*p - *p0);
      double c=pscal(v0, v);
      if (cmax < c) { cmax=c; p1=p;}
    }
    v0=pnorm(*p1 - *p0);
    p0=p1;
    assert (ret.size() <= points.size());
  } while (*p1!=*ret.begin());
  return ret;
}

using namespace fig;

int
main(){

  fig_world W;
  read("border_of_points.fig", W);
  for (fig_world::iterator o=W.begin(); o!=W.end(); o++){
    if (o->type!=2) continue;
    o->iLine::operator=(border_of_points(*o));
    o->close();
  }
  write("border_of_points_out.fig", W);
  return 0;
}
