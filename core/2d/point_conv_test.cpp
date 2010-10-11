#include <iostream>
#include "point_conv.h"

struct mycnv : Conv{
  // x->x y->y**2
  virtual void frw(Point<double> & p) const{
    p.y*=p.y;
  }
  virtual void bck(Point<double> & p) const{
    p.y=sqrt(p.y);
  }
};

void print_line(const dLine & l){
  for (dLine::const_iterator i=l.begin(); i!=l.end(); i++){
    std::cout << i->x << " " << i->y << "\n";
  }
  std::cout << "\n";
}

main(){
  dLine l;
  l.push_back(dPoint(0,0));
  l.push_back(dPoint(1,1));

  std::cerr << "# use gnuplot to view this file\n";

  mycnv cnv;
  dLine m = cnv.line_frw(l, 0.05, 10);

  print_line(m);

  print_line(cnv.line_bck(m, 0.00000001, 7));
  print_line(cnv.line_bck(m, 0.001, 100)+dPoint(0,0.1));

  cnv.line_bck_p2p(m);
  print_line(m+dPoint(0,0.2));
}
