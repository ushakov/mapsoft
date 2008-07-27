#include <iostream>
#include <fstream>
#include "../libfig/fig.h"
#include "../utils/m_time.h"
#include <cmath>
#include "../libzn/zn.h"
#include "../libzn/zn_key.h"
#include "line_dist.h"

using namespace std;
using namespace fig;

/*
Дублирование урочищ точками
Для каждого урочища (текстового объекта) создается точка,
ключ перекидывается на нее, а текст становится ее подписью
*/

main(int argc, char **argv){

  if (argc!=4){
    std::cerr << "usage: " << argv[0] << " <conf_file> <in.fig> <out.fig>\n";
    exit(0);
  }
  string conf_file = argv[1];
  string infile    = argv[2];
  string outfile   = argv[3];

  zn::zn_conv zconverter(conf_file);

  fig_world W;
  if (!read(infile.c_str(), W)){cerr << "Bad fig file " << infile << "\n"; exit(0);}
  fig_world NW;

  int cnt=0;

  for (fig_world::iterator i=W.begin(); i!=W.end(); i++){

//    int type = zconverter.get_type(*i);

    if (test_object(*i, "4 1 12 55 * * * * 4")){
      if (i->size()<1) continue;
      // создаем точку
      fig_object o = make_object("2 1 0 4 12 7  55 -1 -1 0.000 0 1 -1 0 0 1");
      o.push_back((*i)[0]);
      o.comment.swap(i->comment);
      o.comment[0]=i->text;
      NW.push_back(o);
      i->depth=40;
      i->pen_color=0;
      add_key(*i,  zn::zn_label_key(zn::get_key(o)));
      cnt++;
    }
    // прочие объекты - без изменений
    NW.push_back(*i);
  }
  ofstream out(outfile.c_str());
  write(out, NW);
  cerr << cnt << " objects converted\n";
}
