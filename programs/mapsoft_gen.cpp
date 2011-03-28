#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <cstring>
#include <list>

#include <sys/stat.h>
#include <math.h>

#include "geo_io/io.h"
#include "options/read_conf.h"

using namespace std;

void generalize(g_track & line, double e, int np){
  // какие точки мы хотим исключить:
  std::vector<bool> skip(line.size(), false);

  np-=2; // end points are not counted
  while (1){
    // для каждой точки найдем расстояние от нее до
    // прямой, соединяющей две соседние (не пропущенные) точки.
    // найдем минимум этой величины
    double min=-1;
    int mini; // index of point with minimal deviation
    int n=0;
    for (int i=1; i<int(line.size())-1; i++){
      if (skip[i]) continue;
      n++; // count point we doesn't plan to skip
      int ip, in; // previous and next indexes
      // skip[0] and skip[line.size()-1] are always false
      for (ip=i-1; ip>=0; ip--)            if (!skip[ip]) break;
      for (in=i+1; in<line.size()-1; in++) if (!skip[in]) break;
      Point<double> p1 = line[ip];
      Point<double> p2 = line[i];
      Point<double> p3 = line[in];
      double ll = pdist(p3-p1);
      dPoint v = (p3-p1)/ll;
      double prj = pscal(v, p2-p1);
      double dp;
      if      (prj<=0)  dp = pdist(p2,p1);
      else if (prj>=ll) dp = pdist(p2,p3);
      else              dp = pdist(p2-p1-v*prj);
      if ((min<0) || (min>dp)) {min = dp; mini=i;}
    }
    if (n<=2) break;
    // если этот минимум меньше e или точек в линии больше np - выкинем точку
    if ( ((e>0) && (min<e)) ||
         ((np>0) && (n>np))) skip[mini]=true;
    else break;
  }
  g_track::iterator i = line.begin();
  int j=0;
  while (i != line.end()){
    if (skip[j]) i = line.erase(i);
    else i++;
    j++;
  }
}

int main(int argc, char *argv[]) {

  Options opts;

  if (!read_conf(argc, argv, opts)) exit(0);
  if (opts.exists("help")) exit(0);

  string outfile = opts.get("out", string());
  if (outfile == "") exit(0);

  StrVec infiles = opts.get("cmdline_args", StrVec());

// чтение файлов

  geo_data world;
  for(StrVec::const_iterator i=infiles.begin(); i!=infiles.end(); i++)
    io::in(*i, world, opts);

// all these things -> io::filters ?
  io::skip(world, opts);

  for (vector<g_track>::iterator m=world.trks.begin(); m!=world.trks.end(); m++){
    generalize(*m, 1e-6, 400);
  }

/*  for(i=filters.begin(); i!=filters.end(); i++){
    if (*i == "map_nom_brd") filters::map_nom_brd(world);
  }*/

  io::out(outfile, world, opts);
}

