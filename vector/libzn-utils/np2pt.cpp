#include <iostream>
#include <fstream>
#include "libfig/fig.h"
#include "options/m_time.h"
#include <cmath>
#include "../libzn/zn.h"
#include "lib2d/line_dist.h"

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
    exit(0);
  }
  string infile    = argv[1];
  string outfile   = argv[2];

  fig_world W;
  if (!read(infile.c_str(), W)){cerr << "Bad fig file " << infile << "\n"; exit(0);}
  fig_world NW;

  string style=W.opts.get("style", string());
  zn::zn_conv zconverter(style);

  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){

    int type = zconverter.get_type(*i);

    // город, деревня
    if ((type == 0x200001) || (type == 0x20000E)){
      if ((i->comment.size() == 0)||(i->comment[0]=="")) {NW.push_back(*i); continue;}
      // ищем середину объекта
      iPoint p=i->center();
      // создаем точку
      fig_object o = make_object("2 1 0 3 18 7 50 -1 -1 0.000 1 1 7 0 0 1");
      o.push_back(p);
      o.thickness = (type == 0x200001)? 5:3;
      o.comment.swap(i->comment);
      NW.push_back(o);
    }
    // удаляем пустые точки
    if ((type == 0x700) || (type == 0x800) || (type == 0x900)){
      if ((i->comment.size() < 1) || (i->comment[0]=="")) {continue;}
    }
    // прочие объекты - без изменений
    NW.push_back(*i);
  }
  ofstream out(outfile.c_str());
  write(out, NW);
}
