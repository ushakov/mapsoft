#include <iostream>
#include <fstream>
#include "fig/fig.h"
#include "options/m_time.h"
#include <cmath>
#include "vmap/zn.h"
#include "2d/line_dist.h"

using namespace std;
using namespace fig;

/*
Дублирование населенных пунктов точками
Для каждого нас.пункта-полигона создается точка 0x700 или 0x900,
ключ перекидывается на нее, так что она получает подпись.
*/

main(int argc, char **argv){

  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " <in.fig> <out.fig>\n";
    exit(1);
  }
  const char * infile    = argv[1];
  const char * outfile   = argv[2];

  fig_world W;
  if (!read(infile, W)){
    cerr << "Bad fig file " << infile << "\n";
    exit(1);
  }

  string style=W.opts.get<string>("style", "default");
  zn::zn_conv zconverter(style);

  fig_world::iterator i=W.begin();
  while(i!=W.end()){
    int type = zconverter.get_type(*i);
    // город, деревня
    if ((type == 0x200001) || (type == 0x20000E)){
      if ((i->comment.size() == 0)||(i->comment[0]=="")) { i++; continue; }
      // ищем середину объекта
      iPoint p=i->center();
      // создаем точку
      fig_object o = make_object("2 1 0 3 18 7 50 -1 -1 0.000 1 1 7 0 0 1");
      o.push_back(p);
      o.thickness = (type == 0x200001)? 5:3;
      o.comment.swap(i->comment);
      W.push_back(o);
    }
    // удаляем пустые точки
    if ((type == 0x700) || (type == 0x800) || (type == 0x900)){
      if ((i->comment.size() < 1) || (i->comment[0]=="")) {
        i=W.erase(i);
        continue;
      }
    }
    i++;
  }

  ofstream out(outfile);
  write(out, W);
}
