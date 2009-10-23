#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "../../core/libfig/fig.h"
#include "../../core/utils/m_time.h"
#include <cmath>
#include <cstring>
#include "../libzn/zn.h"
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

  iRect rng=W.range();


#define GS_SETBBOX    0
#define GS_MOVETO     1
#define GS_RMOVETO    2
#define GS_LINETO     3
#define GS_RLINETO    4
#define GS_CURVETO    5
#define GS_RCURVETO   6
#define GS_ARC        7
#define GS_ARCN       8
#define GS_ARCT       9
#define GS_CLOSEPATH 10
#define GS_UCACHE    11


  // print definitions of postscript upaths for all types
  for (omap_t::const_iterator t=objects.begin();
                  t!=objects.end(); t=objects.upper_bound(t->first)){
    // for each key in multimap

    char type='p';
    if (t->first & zn::line_mask) type='l';
    if (t->first & zn::area_mask) type='a';

    if (type=='p') continue;

    vector<int> crd, cmd;
    cmd.push_back(GS_UCACHE);
    crd.push_back(0);
    crd.push_back(0);
    crd.push_back(rng.w);
    crd.push_back(rng.h);
    cmd.push_back(GS_SETBBOX);
    for (omap_t::const_iterator i=objects.lower_bound(t->first);
                               i!=objects.upper_bound(t->first); i++){
      if (i->second->size() < 1) continue;
      cmd.push_back(GS_MOVETO);
      int s=i->second->size()-1;
      while ( s > 255-32){
        cmd.push_back(255);
        cmd.push_back(GS_LINETO);
        s-=255-32;
      }
      cmd.push_back(s+32);
      cmd.push_back(GS_LINETO);
      for (fig_object::const_iterator j=i->second->begin();
                                      j!=i->second->end(); j++){
        crd.push_back(j->x - rng.x);
        crd.push_back(j->y - rng.y);
      }
      if (type=='a') cmd.push_back(GS_CLOSEPATH);
    }

    switch (type){
      case 'l': cout << "/POLYLINE"; break;
      case 'a': cout << "/POLYGON"; break;
      case 'p': cout << "/POI"; break;
    }
    cout << "_0x" << hex << (t->first & 0xFFFFF)
         << " {\n<9520" << setw(4) << setfill('0') << crd.size();
    int c=9;
    for (vector<int>::const_iterator i=crd.begin(); i!=crd.end(); i++){
      if (c>70){
        cout << "\n";
        c=0;
      }
      else {
        cout << " ";
        c++;
      }
      cout << setw(4) << setfill('0') << *i;
      c+=4;
    }
    cout << ">\n<";
    c=1;
    for (vector<int>::const_iterator i=cmd.begin(); i!=cmd.end(); i++){
      if (c>70){
        cout << "\n";
        c=0;
      }
      cout << setw(2) << setfill('0') << *i;
      c+=2;
    }
    cout << dec << ">\n} cvlit def\n\n";
  }

  cout << dec << "(./common.ps) run\n\n";

}


