#include <cassert>
#include <fstream>
#include <iostream>
#include "fig/fig.h"
#include "2d/line_rectcrop.h"

using namespace std;

void usage(){
  const char * prog = "rectcrop";

  cerr
     << prog << " -- test lib2d rectcrop_ml code.\n"
     << "  usage: " << prog << " <in_file.fig> <out_file.fig>\n"
     << "\n"
  ;
  exit(1);
}


main(int argc, char **argv){

  if (argc!=3) usage();
  const char *in  = argv[1];
  const char *out = argv[2];

  fig::fig_world F;
  fig::read(in, F);

  int w = int(10.0*fig::cm2fig);
  iRect cutter(0,0,w,w);

  iLine l;
  fig::fig_world::const_reverse_iterator f=F.rbegin();
  if (f!=F.rend()) l=*f;

  iMultiLine ml   = rect_crop_ml(cutter, l, f->is_closed());
  dMultiLine ml_d = rect_crop_ml(dRect(cutter), dLine(l), f->is_closed());

  fig::fig_object o=fig::make_object("2 1 0 1 4 7 20 -1 -1 0.000 1 0 0 0 0 0");

  for (iMultiLine::const_iterator i=ml.begin(); i!=ml.end(); i++){
    o.clear();
    o.iLine::operator=(*i);
    if (f->is_closed()) o.close();
    F.push_back(o);
  }

  fig::write(out,F);


  assert(ml.size() == ml_d.size());
//  for (int i = 0; i< ml.size(); i++){
//    assert (ml[i] == iLine(ml_d[i]));
//  }

  exit (0);
}


