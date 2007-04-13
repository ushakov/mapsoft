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
  fig::fig_world W2,UC;

W1.colors[32]=0xaaffaa;
W1.colors[33]=0x5066ff;
W1.colors[34]=0x404040;
W1.colors[35]=0x888888;

  for (fig::fig_world::iterator i=W1.begin(); i!=W1.end(); i++){
/*    // болота
    if ((fig::test_object(*i, "2 * * 1 33 * * * * * * * * * * 2"))&&
        (i->y[0]==i->y[1])){
//      W2.push_back(fig::make_object(*i, "2 1 0 1 33 7 87 -1 -1 0.000 0 1 -1 0 0 *"));
      *i=fig::make_object(*i, "2 1 0 1 33 7 87 -1 -1 0.000 0 1 -1 0 0 *");
      continue;
    }
*/

// Убрать газопроводные кружки
//    if (fig::test_object(*i, "1 3 0 1 35 7 57 -1 20 * * * * * * * * * * *")){
//      i->depth=500;
//      continue;
//    }

//    // овраги неправильные
//    if (((fig::test_object(*i, "2 1 0 2 26 * * -1 * * * * * * * *"))||
//        (fig::test_object(*i, "3 0 0 2 26 * * -1 * * * * * *"))) && 
//        (i->x.size()<5)){
//      i->pen_color=25; i->depth=89;
//      continue;
//    }

//    // ручьи-5
//    if (((fig::test_object(*i, "2 1 0 * 33 * 86 * -1 * * * * * * *"))||
//        (fig::test_object(*i, "3 0 0 * 33 * 86 * -1 * * * * *"))) && 
//        (i->thickness>2)){
//      i->thickness=5;
//      continue;
//    }


    if ((fig::test_object(*i, "4 * 0 55 * 3 * * *"))||
        (fig::test_object(*i, "4 * 0 55 * 1 * * *"))){
      bool x=false;
      bool num=true;
      if ((i->text.size()>2)&&(i->text[0] == 'у')&&(i->text[1] == 'р')&&(i->text[2] == '.')) x=true;
      if ((i->text.size()>3)&&(i->text[0] == 'б')&&(i->text[1] == 'о')&&(i->text[2] == 'л')&&(i->text[3] == '.')) x=true;
      if (i->text.size()==0) num=false;
      for (int n=0;n<i->text.size();n++)  
        if (((i->text[n]<'0')||(i->text[n]>'9'))&&(i->text[n]!='.')) num=false;
      if (x || num){
        i->font=3;
        i->sub_type=1;
        i->pen_color=12;
        i->font_size=8;
        i->x[0]+=int(i->length/2);
      }
    }


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

    UC.push_back(*i);
  }
  fig::write(out, W1);
}
