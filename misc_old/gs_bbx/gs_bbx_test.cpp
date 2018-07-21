#include <iostream>
#include <fstream>
#include <fig/fig.h>
#include <utils/gs_bbx.h>
#include <cmath>

using namespace std;
using namespace fig;

int
main(int argc, char **argv){

  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " <fig> \n";
    return 1;
  }

  fig_world W1;
  if (!read(argv[1], W1)){
    cerr << "Can't read " << argv[1] <<"\n";
    return 1;
  }

  gs_bbx G;
  int n=0;

  for (fig_world::iterator i=W1.begin(); i!=W1.end(); i++){
    if (i->type!=4) continue;

    map<int,string>::const_iterator fn = fig::psfonts.find(i->font);
    if (fn == fig::psfonts.end()) continue;

    G.set_font(int(i->font_size), fn->second.c_str());
    iRect r = G.txt2bbx_fig(i->text.c_str());

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
    n++;
    cerr << ".";
  }
  cerr << "\n" << n << " text objects processed\n";
  fig::write(argv[1], W1);
  return 0;
}
