#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "../../core/libfig/fig.h"
#include "../../core/utils/m_time.h"
#include <cmath>
#include <cstring>
#include "../libzn/zn.h"
#include "line_dist.h"
#include "../../core/lib2d/line_polycrop.h"

using namespace std;
using namespace fig;

typedef multimap<int, fig_world::iterator> omap_t;
typedef pair<int, fig_world::iterator> op_t;





main(int argc, char **argv){

  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " <conf_file> <in.fig>\n";
    exit(0);
  }

  string conf_file = argv[1];
  string infile    = argv[2];

  zn::zn_conv zconverter(conf_file);

  fig_world W;
  if (!read(infile.c_str(), W)){
    cerr << "Bad fig file " << infile << "\n"; exit(0);
  }

  omap_t objects;

  // insert objects into multimap<type,object>
  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){

    if ((i->type == 4) && (i->comment.size()>0) && (i->comment[0] == "CURRENT DATE")){
      Time t; t.set_current();
      i->text = t.date_str();
    }

    if (i->type == 6){ // shift comments into compounds
      fig::fig_world::iterator j = i; j++;
      if (j!=W.end()){
        if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
        for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
      }
      continue;
    }

    if (i->type == -6) continue;

    if ((i->comment.size()>0) && (i->comment[0] == "[skip]")) continue;
    if ((i->comment.size()>1) && (i->comment[1] == "[skip]")) continue;

    if ((i->depth <50) || (i->depth>=200)) continue;
    if (i->size() == 0) continue;

    int type = zconverter.get_type(*i);

    objects.insert(op_t(type,i));
  }

  int scale=20;
  iRect rng=W.range()/scale;

  // print definitions of postscript upaths for all types
  for (omap_t::const_iterator t=objects.begin();
                  t!=objects.end(); t=objects.upper_bound(t->first)){
    // for each key in multimap

    if (! (t->first & zn::line_mask)) continue;

    vector<int> crd, cmd;
    cmd.push_back(11);
    crd.push_back(rng.x);
    crd.push_back(rng.y);
    crd.push_back(rng.x+rng.w);
    crd.push_back(rng.y+rng.h);
    cmd.push_back(0);
    for (omap_t::const_iterator i=objects.lower_bound(t->first);
                               i!=objects.upper_bound(t->first); i++){
      if (i->second->size() < 1) continue;
      cmd.push_back(1);
      int s=i->second->size();
      if ( s > 256-32){
        cmd.push_back(255);
        cmd.push_back(3);
      }
      cmd.push_back(s-1+32);
      cmd.push_back(3);
      for (fig_object::const_iterator j=i->second->begin();
                                      j!=i->second->end(); j++){
        crd.push_back(j->x/scale);
        crd.push_back(j->y/scale);
      }
    }
    cmd.push_back(10);

    cout << "/POLYLINE_0x" << setbase(16) << (t->first & 0xFFFFF)
         << " {\n{" << setbase(10);
    for (vector<int>::const_iterator i=crd.begin(); i!=crd.end(); i++)
      cout << *i << " ";
    cout << "}\n<" << setbase(16);
    for (vector<int>::const_iterator i=cmd.begin(); i!=cmd.end(); i++)
      cout << setw(2) << setfill('0') << *i << " ";
    cout << setbase(10) << ">\n} cvlit def\n";
  }
}


