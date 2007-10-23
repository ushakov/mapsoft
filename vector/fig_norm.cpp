#include <iostream>
#include <fstream>
#include "../geo_io/fig.h"
#include <cmath>

using namespace std;

main(int argc, char **argv){

  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " in.fig out.fig unconv.fig\n";
    exit(0);
  }
  string infile  = argv[1];
  string outfile = argv[2];
//  string unconv  = argv[3];

  ofstream out(outfile.c_str());
//  ofstream  uc(unconv.c_str());

  fig::fig_world W1  = fig::read(argv[1]);

//W1.colors[32]=0xaaffaa;
//W1.colors[33]=0x5066ff;
//W1.colors[34]=0x404040;
//W1.colors[35]=0x888888;

  for (fig::fig_world::iterator i=W1.begin(); i!=W1.end(); i++){
/*    // болота
    if ((fig::test_object(*i, "2 * * 1 33 * * * * * * * * * * 2"))&&
        (i->y[0]==i->y[1])){
//      W2.push_back(fig::make_object(*i, "2 1 0 1 33 7 87 -1 -1 0.000 0 1 -1 0 0 *"));
      *i=fig::make_object(*i, "2 1 0 1 33 7 87 -1 -1 0.000 0 1 -1 0 0 *");
      continue;
    }
*/


    if (((fig::test_object(*i, "2 * * 1 * * 90 * * * * * * * * *"))||
        (fig::test_object(*i,  "3 * * 1 * * 90 * * * * * * *"))) && 
	(i->comment.size()>0) &&
        ((i->comment[0] == "2000") ||
         (i->comment[0] == "3000") || 
         (i->comment[0] == "4000") || 
         (i->comment[0] == "5000")))
      {
      i->thickness=2;
      continue;
    }

//    // ручьи-5
//    if (((fig::test_object(*i, "2 1 0 * 33 * 86 * -1 * * * * * * *"))||
//        (fig::test_object(*i, "3 0 0 * 33 * 86 * -1 * * * * *"))) && 
//        (i->thickness>2)){
//      i->thickness=5;
//      continue;
//    }

/* // вернуть назад преобразованные дороги...
    if ((fig::test_object(*i, "2 * 0 1 0 * 80 * * * * * * * * * "))||
        (fig::test_object(*i, "3 * 0 1 0 * 80 * * * * * * * "))){
      fig::fig_object o = *i;
      o.depth=79;
      o.pen_color=7;
      o.line_style=2;
      o.cap_style=2;
      o.style_val=10;
      bool t=false;
      for (fig::fig_world::iterator j=W1.begin(); j!=W1.end(); j++){
        if (o==*j){
          W1.erase(j);
          t=true;
          break;
        }
      }
      if (t) i->pen_color=34;
    }

    if ((fig::test_object(*i, "2 * 0 3 0 * 80 * * * * * * * * * "))||
        (fig::test_object(*i, "3 * 0 3 0 * 80 * * * * * * * "))){
      fig::fig_object o = *i;
      o.depth=79;
      o.pen_color=7;
      o.thickness=1;
      bool t=false;
      for (fig::fig_world::iterator j=W1.begin(); j!=W1.end(); j++){
        if (o==*j){
          W1.erase(j);
          t=true;
          break;
        }
      }
      if (t) i->pen_color=34;
    }

    if ((fig::test_object(*i, "2 * 0 4 0 * 80 * * * * * * * * * "))||
        (fig::test_object(*i, "3 * 0 4 0 * 80 * * * * * * * "))){
      fig::fig_object o = *i;
      o.depth=79;
      o.pen_color=27;
      o.thickness=2;
      bool t=false;
      for (fig::fig_world::iterator j=W1.begin(); j!=W1.end(); j++){
        if (o==*j){
          W1.erase(j);
          t=true;
          break;
        }
      }
      if (t) i->pen_color=34;
    }
*/

/*    // ручьи-1
    if ((fig::test_object(*i, "2 1 * 1 33 * 86 * -1 * * * * * * *"))||
        (fig::test_object(*i, "3 0 * 1 33 * 86 * -1 * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 1 0 1 33 7 86 -1 -1 0.000 0 1 -1 0 0 *"));
      continue;
    }
    // ручьи-2
    if ((fig::test_object(*i, "2 1 0 2 33 * * * -1 * * * * * * *"))||
        (fig::test_object(*i, "3 0 0 2 33 * * * -1 * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 1 0 2 33 7 86 -1 -1 0.000 0 1 -1 0 0 *"));
      continue;
    }
    // ручьи-5
    if ((fig::test_object(*i, "2 1 0 * 33 * * * -1 * * * * * * *"))||
        (fig::test_object(*i, "3 0 0 * 33 * * * -1 * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 1 0 5 33 7 86 -1 -1 0.000 0 1 -1 0 0 *"));
      continue;
    }
    // ручьи пересыхающие
    if ((fig::test_object(*i, "2 1 * 1 33 * * * -1 * * * * * * * "))||
        (fig::test_object(*i, "3 0 * 1 33 * * * -1 * * * * * "))){
      W2.push_back(fig::make_object(*i, "2 1 1 1 33 7 86 -1 -1 4.000 0 1 -1 0 0 *"));
      continue;
    }
    // озера
    if ((fig::test_object(*i, "2 * 0 1 33 3 * * 20 * * * * * * *"))||
        (fig::test_object(*i, "3 * 0 1 33 3 * * 20 * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 * 0 1 33 3 85 -1 20 0.000 0 0 -1 0 0 *"));
      continue;
    }
    // острова
    if ((fig::test_object(*i, "2 * 0 1 33 * * * 40 * * * * * * *"))||
        (fig::test_object(*i, "3 * 0 1 33 * * * 40 * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 * 0 1 33 3 84 -1 40 0.000 0 0 -1 0 0 *"));
      continue;
    }
    // горизонтали тонкие
    if ((fig::test_object(*i, "2 * 0 1 26 * * -1 * * * * * * * *"))||
        (fig::test_object(*i, "3 * 0 1 26 * * -1 * * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 * 0 1 26 7 90 -1 -1 0.000 1 0 0 0 0 *"));
      continue;
    }
    // горизонтали пунктирные
    if ((fig::test_object(*i, "2 * 2 1 26 * * -1 * * * * * * * *"))||
        (fig::test_object(*i, "3 * 2 1 26 * * -1 * * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 * 2 1 26 7 90 -1 -1 2.000 1 0 0 0 0 *"));
      continue;
    }
    // овраги нормальные 
    if ((fig::test_object(*i, "2 1 0 2 25 * * -1 * * * * * * * *"))||
        (fig::test_object(*i, "3 0 0 2 25 * * -1 * * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 * 0 1 25 7 89 -1 -1 0.000 1 0 0 0 0 *"));
      continue;
    }
    // горизонтали толстые
    if ((fig::test_object(*i, "2 * 0 2 26 * * -1 * * * * * * * *"))||
        (fig::test_object(*i, "3 * 0 2 26 * * -1 * * * * * *"))){
      W2.push_back(fig::make_object(*i, "2 * 0 2 26 7 90 -1 -1 0.000 1 0 0 0 0 *"));
      continue;
    }*/

  }
  fig::write(out, W1);
}
