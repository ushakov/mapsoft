// Получение fig-карты для редактирования

#include <string>
#include <fstream>
#include "zn.h"

using namespace std;

void usage(){
    cerr << "usage: get_map <map> <conf_file> <out.fig|out.mp>\n";
    exit(0);
}


const string maps_dir  = "./maps";


// проверка расширения
bool testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

main(int argc, char** argv){

  if (argc != 4) usage();
  string map_name  = argv[1];
  string conf_file = argv[2];
  string out_file = argv[3];

  string file = maps_dir+"/"+map_name+".fig";

  // пробуем прочитать карту
  fig::fig_world MAP = fig::read(file.c_str());
  if (MAP.size()==0) {
    cerr << "bad file " << file << "\n";
    exit(0);
  }



  zn::zn_conv zconverter(conf_file);
  
  
  if (testext(out_file, ".fig")){ // пишем fig
    fig::fig_world F;
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
      if ((i->depth >=50) && (i->depth <400)){
        list<fig::fig_object> o = zconverter.fig2user(*i);
        F.insert(F.begin(), o.begin(), o.end());
      }
      else F.push_back(*i);
    }
    ofstream out(out_file.c_str());
    fig::write(out, F);
  } else
  if (testext(out_file, ".mp")){ // читаем mp
    // извлекаем привязку
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

    mp::mp_world M;
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
      if ((i->depth >=50) && (i->depth <400)){
        std::list<mp::mp_object> mp_list = zconverter.fig2mp(*i, cnv);
        M.insert(M.end(), mp_list.begin(), mp_list.end());
      }
    }
    ofstream out(out_file.c_str());
    mp::write(out, M);
  }
}
