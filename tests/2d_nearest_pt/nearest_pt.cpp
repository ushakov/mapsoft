#include <cassert>
#include <fstream>
#include <iostream>
#include "fig/fig.h"
#include "2d/line_utils.h"

using namespace std;

void usage(){
  const char * prog = "rectcrop";

  cerr
     << prog << " -- test lib2d nearest_pt code.\n"
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

  // push all fig objects into single multiline
  iMultiLine ml;
  fig::fig_world::const_iterator f;
  for (f=F.begin(); f!=F.end(); f++) ml.push_back(*f);

  fig::fig_object o=fig::make_object("2 1 0 1 4 7 20 -1 -1 0.000 1 0 0 0 0 0");

  // use points on greed 10x10 with 1cm step
  for (int i=0; i<10; i++){
    for (int j=0; j<10; j++){
      iPoint p(i,j); p*=fig::cm2fig;
      dPoint dp(p);

      o.clear();
      o.push_back(p);

      dPoint v;
      nearest_pt(ml, v, p, fig::cm2fig);

      o.push_back(p);
      F.push_back(o);
      // check for double points
//      assert(iPoint(dp) == p);
    }
  }

  fig::write(out,F);


//  assert(ml.size() == ml_d.size());
//  for (int i = 0; i< ml.size(); i++){
//    assert (ml[i] == iLine(ml_d[i]));
//  }

  exit (0);
}


