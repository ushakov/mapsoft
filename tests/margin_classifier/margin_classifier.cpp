#include <string>
#include <fstream>
#include <iostream>
#include "fig/fig.h"
#include "2d/line_utils.h"

using namespace std;

void usage(){
  const char * prog = "margin_classifier";

  cerr
     << prog << " -- test lib2d margin_classifier code.\n"
     << "  usage: " << prog << " <in_file.fig> <out_file.fig>\n"
     << "\n"
  ;
  exit(1);
}

int
main(int argc, char **argv){

  if (argc!=3) usage();
  const char *in  = argv[1];
  const char *out = argv[2];

  fig::fig_world F;
  fig::read(in, F);

  iLine l1;
  iLine l2;
  fig::fig_world::const_iterator f=F.begin();
  if (f!=F.end()) l1=*f;
  f++;
  if (f!=F.end()) l2=*f;

  iPoint p0;
  dPoint t;
  double m=margin_classifier(l1,l2,p0,t);

  // проверка, что dLine и iLine считаются одинаково
  dPoint p0d, td;
  double md=margin_classifier(dLine(l1),dLine(l2),p0d,td);
  if ((md!=m) || (td!=t) || (iPoint(p0d)!=p0)){
    std::cerr << "different results for int and double lines!\n";
    return 1;
  }

  dPoint n(t.y, -t.x);
  iPoint tx(t*fig::cm2fig*5);
  iPoint nx(n*m/2);

  fig::fig_object o=fig::make_object("2 1 0 1 4 7 20 -1 -1 0.000 1 0 0 0 0 0");

  o.push_back(p0 + tx);
  o.push_back(p0 - tx);
  F.push_back(o); o.clear();

  o.push_back(p0 - nx + tx);
  o.push_back(p0 - nx - tx);
  F.push_back(o); o.clear();

  o.push_back(p0 + nx + tx);
  o.push_back(p0 + nx - tx);
  F.push_back(o); o.clear();

  fig::write(out,F);
  return 0;
}


