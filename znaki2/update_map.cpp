// Обновление карты из fig или mp файла

#include <string>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "zn.h"

using namespace std;

void usage(){
    cerr << "usage: update_map_gk <map> <source> <conf file> <file.mp|file.fig>\n";
    exit(0);
}


const string maps_dir  = "./maps";

// проверка расширения
bool testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

//
main(int argc, char** argv){

  if (argc != 5) usage();
  string map_name  = argv[1];
  string source    = argv[2];
  string conf_file = argv[3];
  string infile    = argv[4];
  string file = maps_dir+"/"+map_name+".fig";

  // пробуем прочитать карту
  fig::fig_world MAP = fig::read(file.c_str());
  if (MAP.size()==0) {
    cerr << "bad file " << file << "\n";
    exit(0);
  }
  // извлекаем привязку
  g_map ref = fig::get_ref(MAP);

  zn::zn_conv zconverter(conf_file);

// здесь надо сделать backup исходной карты!!!!!


  fig::fig_world FIG; // заготовка для новой карты
  
  if (testext(infile, ".fig")){ // читаем fig
    FIG = fig::read(infile.c_str());
  } else
  if (testext(infile, ".mp")){ // читаем mp
    mp::mp_world MP = mp::read(infile.c_str());
    // копируем в FIG из MAP все объекты с глубинами 1-29 и 200-999
    // (сетку, привязку и т.п.)
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++)
      if ((i->depth <30) || (i->depth >=200)) FIG.push_back(*i);
    // преобразуем объекты из MP в FIG
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++)
      FIG.push_back(zconverter.mp2fig(*i, ref));
  } else usage();
  // Теперь в FIG у нас новая карта с привязкой.


  // найдем максимальный id элементов старой карты
  // распихаем объекты и подписи в хэши
  map<int, fig::fig_object> objects;
  multimap<int, fig::fig_object> labels;

  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    zn::zn_key key = zconverter.get_key(*i);
    if ((key.map_name != map_name) || (key.id == 0)) continue;
    if (key.label) labels.insert(pair<int, fig::fig_object>(key.id, *i));
    else objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }

  //  Обнулим старую карту и будем ее заполнять постепенно из FIG
  MAP.clear();

  for (fig::fig_world::const_iterator i=FIG.begin(); i!=FIG.end(); i++){
    // объекты с глубинами 1-29 и 200-999 (сетка, привязка и т.п.)
    if ((i->depth <30) || (i->depth >=200)) {
      MAP.push_back(*i); 
      continue;
    }
    // остальные объекты
    zn::zn_key key = zconverter.get_key(*i);
    if ((key.map_name == map_name) && (key.id !=0)){ // есть старый ключ от этой карты
      map<int, fig::fig_object>::iterator o = objects.find(key.id);
      if (o==objects.end()){
        cerr << "Конфликт: объект " << key.id << " был удален\n";
        cerr << "не добавляю его!\n";
        //... обвести рамкой
        continue;
      }
      zn::zn_key oldkey = zconverter.get_key(o->second);
      if (oldkey.time > key.time){
        cerr << "Конфликт: объект " << key.id << " был изменен\n";
        cerr << "не добавляю его!\n";
        //... обвести рамкой
        continue;
      }
      // ... проверить еще конфликты, когда два близких 
      // объекта были нарисованы в одном районе!
      key.time.set_current();
      key.source_id  = 0;
      key.source     = source;

      zconverter.add_key(*i, key);  // добавим ключ
      MAP.push_back(*i);            // запишем объект 

      // перерь еще подписи:
      list<fig::fig_object> new_labels;
      Point<int> shift;
      if (i->isshifted(o->second, shift)){
        // если старый и новый объекты различаются только смещением -
        // перетащить в new_labels старые подписи, сдвинув их
        for (multimap<int, fig::fig_object>::const_iterator l = labels.find(key.id); 
              (l != labels.end()) && (l->first == key.id); l++){
          new_labels.push_back(l->second + shift);
        }
      } else { 
        // иначе - сгенерить подписи заново
        new_labels = zconverter.make_labels(*i, key);
      }
      MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());  // записать подписи

    } 
    else { // объект без ключа
      maxid++;
      key = zconverter.make_key(*i);            // создать новый ключ
      key.time.set_current();
      key.id        = maxid;
      key.map_name  = map_name;
      key.source    = source;
      key.source_id = 0;
      zconverter.add_key(*i, key);  // добавим ключ
      MAP.push_back(*i);            // запишем объект
      list<fig::fig_object> new_labels = zconverter.make_labels(*i, key); // изготовить новые подписи
      MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());        // записать подписи
    }
  }

  // записываем MAP
  fig::write(MAP, file.c_str());

}
