#include <iostream>
#include <fstream>
#include "../libfig/fig.h"
#include "gs_bbx.h"
#include <cmath>

using namespace std;
using namespace fig;

main(int argc, char **argv){

  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " <fig> \n";
    exit(0);
  }
  fig_world W1;
  if (read(argv[1], W1)) exit(0);

  fig_psfonts fontnames;
  gs_bbx G;


  for (fig_world::iterator i=W1.begin(); i!=W1.end(); i++){
    if (i->type!=4) continue;
    G.set_font(int(i->font_size), fontnames[i->font].c_str());
    Rect<int> r = G.txt2bbx_fig(i->text.c_str());

    fig_object o = make_object("2 2 0 0 4 4 1 -1 20 0.000 0 1 7 0 0 5");
    o.clear();
    o.depth=i->depth+1;
    r+= (*i)[0];

    o.push_back(r.TLC());
    o.push_back(r.TRC());
    o.push_back(r.BRC());
    o.push_back(r.BLC());
    o.push_back(r.TLC());
    W1.push_back(o);
  }
  ofstream out(argv[1]);
  fig::write(out, W1);
}
