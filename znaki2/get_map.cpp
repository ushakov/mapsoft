// Получение fig-карты для редактирования

#include <string>
#include <fstream>
#include "zn.h"
#include "zn_key.h"

using namespace std;

/*
выдача карты из хранилища в формате fig или mp.
все параметры картографических объектов устанавливаются
в зависимости от их ключа (в хранилище внешний 
вид объектов не важен!)
*/

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
  fig::fig_world MAP;
  if (!fig::read(file.c_str(), MAP)){ cerr << "bad fig file " << file << "\n"; exit(0);}

  zn::zn_conv zconverter(conf_file);

  if (testext(out_file, ".fig")){ // пишем fig
    fig::fig_world F;

    std::multimap<int, fig::fig_object> labels; 
    fig::fig_world::iterator i=MAP.begin(); 


    // вынем из карты подписи
    while (i!=MAP.end()){
      if (!zn::is_map_depth(*i)){
        zn::zn_label_key k = zn::get_label_key(*i);
        if ((k.id!=0) && (k.map ==map_name)){
          labels.insert(std::pair<int, fig::fig_object>(k.id, *i));
          i=MAP.erase(i);
          continue;
        }
      }
      i++;
    }

    for (fig::fig_world::iterator i=MAP.begin(); i!=MAP.end(); i++){
      if (zn::is_map_depth(*i)){
        zn::zn_key k = zn::get_key(*i);
        zconverter.fig_update(*i, k.type);
        list<fig::fig_object> l1 = zconverter.make_pic(*i, k.type);
        F.insert(F.begin(), l1.begin(), l1.end());
        // вытащим из хэша все подписи для этого объекта
        for (multimap<int, fig::fig_object>::iterator l = labels.find(k.id);
            (l != labels.end()) && (l->first == k.id); l++){
          zconverter.label_update(l->second, k.type);
          F.push_back(l->second);
        }

      }
      else F.push_back(*i);
    }

    ofstream out(out_file.c_str());
    fig::write(out, F);
  } 

  else if (testext(out_file, ".mp")){ // читаем mp
    // извлекаем привязку
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, "wgs84", "lonlat");

    mp::mp_world M;
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
      if (zn::is_map_depth(*i)){
        zn::zn_key k = zn::get_key(*i);
        M.push_back(zconverter.fig2mp(*i, cnv, k.type));
      }
    }
    ofstream out(out_file.c_str());
    mp::write(out, M);
  }
}
