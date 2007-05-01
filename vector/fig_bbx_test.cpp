#include <iostream>
#include <fstream>
#include "../geo_io/fig.h"
#include "../geo_io/gs_bbx.h"
#include <cmath>

using namespace std;
using namespace fig;

main(int argc, char **argv){

  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " <fig> \n";
    exit(0);
  }
  fig_world W1  = read(argv[1]);

  fig_psfonts fontnames;
  gs_bbx G;

  double k=1200/72*1.05;

  for (fig_world::iterator i=W1.begin(); i!=W1.end(); i++){
    if (i->type!=4) continue;
    G.set_font(int(i->font_size), fontnames[i->font].c_str());
    Rect<double> r = G.txt2bbx(i->text.c_str());

    fig_object o = make_object("2 2 0 0 4 4 1 -1 20 0.000 0 1 7 0 0 5");
    o.clear();
    o.depth=i->depth+1;
    r*=k;
    Rect<int> ri(int(r.x), int(-r.y-r.h), int(r.w), int(r.h));

    ri+= (*i)[0];

    o.push_back(ri.TLC());
    o.push_back(ri.TRC());
    o.push_back(ri.BRC());
    o.push_back(ri.BLC());
    o.push_back(ri.TLC());
    W1.push_back(o);
  }
  ofstream out(argv[1]);
  fig::write(out, W1);
}
