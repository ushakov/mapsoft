#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <cmath>

#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "legend.h" // информация обо всех условных обозначениях карты

using namespace std;

/* 
Доьавить информацию в карту
*/

void usage(){
  cout << "Добавить информацию в карту\n";
  cout << "usage: add2map [p|e] <infile> <outfile> [<style>]\n";
  cout << "Формат файла определяется по расширению\n"; 
  exit(0);
}

bool testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}


main(int argc, char **argv){

  string infile, outfile, style="";
  string type="e";

// разбор командной строки
  if ((argc!=5)&&(argc!=4)) usage();
  type    = argv[1];
  infile  = argv[2];
  outfile = argv[3];
  style   = (argc>4)? argv[4]:"";

  int ifmt, ofmt;
  if (testext(infile, ".fig")){
    ifmt = 0;
  } else if (testext(infile, ".mp")){
    ifmt = 1;
  } else {usage();}

  if (testext(outfile, ".fig")){
    ofmt = 0;
  } else if (testext(outfile, ".mp")){
    ofmt = 1;
  } else {usage();}


  legend znaki(style);
  list<map_object> MAP;  

  if (ifmt == 0){ // читаем fig-файл
    cout << "reading fig-file: " << infile << ", ";
    fig::fig_world IW = fig::read(infile.c_str());
    cout << IW.size() << " objects\n";
    // определяем проекцию файла
    g_map imap = fig::get_ref(IW);
    convs::map2pt icnv(imap, Datum("wgs84"), Proj("lonlat"), Options());
    // преобразования объектов
    for (fig::fig_world::iterator i=IW.begin(); i!=IW.end(); i++){
      map_object obj = znaki.fig2map(*i, IW, icnv);
      if (obj.size()>0) MAP.push_back(obj);
    }
  } else 
  if (ifmt == 1){ // читаем mp-файл
    cout << "reading mp-file: " << infile << ", ";
    mp::mp_world IW = mp::read(infile.c_str());
    cout << IW.size() << " objects\n";
    // преобразования объектов
    for (mp::mp_world::iterator i=IW.begin(); i!=IW.end(); i++){
      map_object obj = znaki.mp2map(*i);
      if (obj.size()>0) MAP.push_back(obj);
    }
  }


  if (ofmt == 0){ // пишем fig-файл
    cout << "reading old fig-file: " << outfile << ", ";
    fig::fig_world OW = fig::read(outfile.c_str());
    fig::fig_world OW_old = OW;
    cout << OW.size() << " objects\n";

    // определяем проекцию файла
    g_map omap = fig::get_ref(OW);
//    else {cerr << "can't get geo reference data from " << outfile << "\n"; exit();}
    convs::map2pt ocnv(omap, Datum("wgs84"), Proj("lonlat"), Options());

    znaki.fig_add_colors(OW); // добавим в fig цвета, нужные для знаков

    cout << "converting objects...\n";
    for (list<map_object>::iterator i=MAP.begin(); i!=MAP.end(); i++){
      list<fig::fig_object> objs = (type=="p")? i->type->map2pfig(*i, ocnv) : i->type->map2fig(*i, ocnv);
      for (list<fig::fig_object>::iterator j=objs.begin(); j!=objs.end(); j++) OW.push_back(*j);
    }
    cout << "writing fig to file...\n";
    ofstream out(outfile.c_str());
    if (out){ fig::write(out, OW); out.close();}
  } else 


  if (ofmt == 1){ // пишем mp-файл
    cout << "reading old mp-file: " << outfile << ", ";
    mp::mp_world OW = mp::read(outfile.c_str());
    cout << OW.size() << " objects\n";
    mp::mp_world OW_old = OW;

    cout << "converting objects...\n";
    for (list<map_object>::iterator i=MAP.begin(); i!=MAP.end(); i++){
      list<mp::mp_object> objs = (type=="p")? i->type->map2pmp(*i) : i->type->map2mp(*i);
      for (list<mp::mp_object>::iterator j=objs.begin(); j!=objs.end(); j++) OW.push_back(*j);
    }
    cout << "writing mp to file...\n";
    ofstream out(outfile.c_str());
    if (out){ mp::write(out, OW); out.close();}
  }
}
