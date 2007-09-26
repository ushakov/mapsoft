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
Преобразование карт в версии для редактирования. Есть две карты в
форматах mp или fig (формат определяется по расширению) 
mapconf <map1> <map2> конвертирует карту 1 в формат карты 2,
оставляя у карты 2 родную проекцию fig-файла или заголовок 
mp-файла. Если карты 2 не существует, то она создается. При этом
для fig-файла спрашиваются параметры проекции...
*/

void usage(){
  cout << "Преобразование fig<->mp\n";
  cout << "usage: mapconv [p|e] <infile> <outfile> [<style>]\n";
  cout << "Заголовок mp-файла сохраняется старый, если он есть\n";
  cout << "Проекция fig-файла сохраняется старой, если она есть\n";
  cout << "Формат файла определяется по расширению\n"; 
  exit(0);
}

bool testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

double get_dvalue(const char *name, double dflt){
  string str;
  cout << name << " ["<< dflt << "]: "; 
  cin >> str;
  if (str == "") return dflt;
  else return atof(str.c_str());

}

// заставить пользователя ввести параметры карты...
g_map get_user_map(){
  string str;
  cout << "No predefined geo-transformation found!...\n";
  cout << "I'll ask you about it!\n";

  cout << "datum [pulkovo]: "; cin >> str;
  if (str == "") str = "pulkovo";
  Datum datum(str);

  cout << "proj  [tmerc]:   "; cin >> str;
  if (str == "") str = "tmerc";
  Proj  proj(str);

  Options opts;
  if (str=="tmerc") {
    cout << "lon0, deg:   "; cin >> str;
     if (str != "") opts["lon0"] = str;
  }

  convs::pt2pt cnv(datum,proj,opts, Datum("wgs84"), Proj("lonlat"), Options());

  double scale = get_dvalue("scale, m/cm", 500);

  double minx = get_dvalue("minx, m", 0);
  double maxx = get_dvalue("maxx, m", 0);
  double miny = get_dvalue("miny, m", 0);
  double maxy = get_dvalue("maxy, m", 0);
  minx -= floor(minx/1000000);
  maxx -= floor(maxx/1000000);

  g_map map;
  double W = (maxx-minx)/scale * fig::cm2fig;
  double H = (maxy-miny)/scale * fig::cm2fig;
  g_refpoint rps[4] = {
    g_refpoint(minx, miny,0,H),
    g_refpoint(minx, maxy,0,0),
    g_refpoint(maxx, miny,W,H),
    g_refpoint(maxx, maxy,W,0)};
  g_point bps[4] = {
    g_point(0,H),
    g_point(0,0),
    g_point(W,H),
    g_point(W,0)};
  for (int n=0; n<4; n++){
    cnv.frw(rps[n]);
    map.push_back(rps[n]);
    map.border.push_back(bps[n]);
  }
  map.map_proj = proj;
  return map;
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
    g_map omap;
    if (OW.size()>0) omap = fig::get_ref(OW);
    else omap = get_user_map();
    convs::map2pt ocnv(omap, Datum("wgs84"), Proj("lonlat"), Options());

    OW.clear();
    znaki.fig_add_colors(OW);
    fig::set_ref(OW, omap, Options());

    cout << "converting objects...\n";
    for (list<map_object>::iterator i=MAP.begin(); i!=MAP.end(); i++){
      list<fig::fig_object> objs = (type=="p")? i->type->map2pfig(*i, ocnv) : i->type->map2fig(*i, ocnv);
      for (list<fig::fig_object>::iterator j=objs.begin(); j!=objs.end(); j++) {
        // если в старом файле есть навеска
        bool pushed = false;
        if ((j->comment.size()>0) && (j->comment[0].compare(0,4, "KEY:")==0)){
          for (fig::fig_world::iterator k=OW_old.begin(); k!=OW_old.end(); k++){
            if ((k->comment.size()>0) && (key_cmp(k->comment[0],j->comment[0], 0.01*fig::cm2fig))) 
              {OW.push_back(*k); pushed = true;}
          }
        }
        if (!pushed) OW.push_back(*j);
      }
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

    OW.clear();
    cout << "converting objects...\n";
    for (list<map_object>::iterator i=MAP.begin(); i!=MAP.end(); i++){
      list<mp::mp_object> objs = (type=="p")? i->type->map2pmp(*i) : i->type->map2mp(*i);
      for (list<mp::mp_object>::iterator j=objs.begin(); j!=objs.end(); j++) {
        // если в старом файле есть навеска
        bool pushed = false;
        if ((j->Comment.size()>0) && (j->Comment[0].compare(0,4, "KEY:")==0)){
          for (mp::mp_world::iterator k=OW_old.begin(); k!=OW_old.end(); k++){
            if ((k->Comment.size()>0) && (key_cmp(k->Comment[0],j->Comment[0], 0.00001))) 
              {OW.push_back(*k); pushed = true;}
          }
        }
        if (!pushed) OW.push_back(*j);
      }
    }
    cout << "writing mp to file...\n";
    ofstream out(outfile.c_str());
    if (out){ mp::write(out, OW); out.close();}
  }
}
