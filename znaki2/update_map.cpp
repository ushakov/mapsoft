// Обновление карты из fig или mp файла

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "zn.h"

using namespace std;

void usage(){
    cerr << "usage: update_map_gk <map> <source> <conf file> <file.mp|file.fig> <nc.fig>\n";
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

  if (argc != 6) usage();
  string map_name  = argv[1];
  string source    = argv[2];
  string conf_file = argv[3];
  string infile    = argv[4];
  string ncfile    = argv[5];
  string file = maps_dir+"/"+map_name+".fig";

  bool fig_not_mp;
  if      (testext(infile, ".fig")) fig_not_mp=true;
  else if (testext(infile, ".mp"))  fig_not_mp=false;
  else usage();

  // читаем старую карту
  std::cerr << "Reading old map...\n";  
  fig::fig_world MAP = fig::read(file.c_str());

  // backup исходной карты!
  std::cerr << "Backup...\n";  
  int ver_num = 0;
  std::string backup;
  do{ 
    ostringstream fn;
    fn << maps_dir << "/" << map_name << std::setw(6) << std::setfill('0') << ver_num << ".fig";
    backup = fn.str();
    ifstream test(backup.c_str());
    if (!test) break;
    ver_num++;
  } while (true);
  ofstream bu(backup.c_str());
  fig::write(bu, MAP);
  // ... сделать diff?

  zn::zn_conv zconverter(conf_file);


  // новая карта
  fig::fig_world FIG; 
  // Если мы обновляемся из fig - прочитаем сюда
  // все объекты из fig-файла
  // Если из mp - то картографические объекты возьмем из mp, 
  // а все остальное - из старой карты!
  std::cerr << "Reading new map...\n";  
  if (fig_not_mp){ // читаем fig
    FIG = fig::read(infile.c_str());
  } 
  else { // читаем mp
    mp::mp_world MP = mp::read(infile.c_str());
    // копируем в FIG из MAP все некартографические объекты
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++)
      if (!zn::is_map_depth(*i)) FIG.push_back(*i);

    // извлекаем привязку из старой карты:
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

    // преобразуем объекты из MP в FIG
    // для новых объектов (без ключа) создается неполный ключ - только с типом
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++)
      FIG.push_back(zconverter.mp2fig(*i, cnv));
    
  }
  // Теперь в FIG у нас новая карта с привязкой.


  // найдем максимальный id элементов старой карты
  // распихаем старые объекты в хэш
  map<int, fig::fig_object> objects;
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (!zn::is_map_depth(*i)) continue;
    zn::zn_key key = zn::get_key(*i);
    if ((key.map != map_name) || (key.id == 0)) continue;
    objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }

  // распихаем новые подписи в хэш
  multimap<int, fig::fig_object> labels;
  for (fig::fig_world::const_iterator i=FIG.begin(); i!=FIG.end(); i++){
    if (zn::is_map_depth(*i)) continue;
    zn::zn_label_key key = zn::get_label_key(*i);
    if ((key.map != map_name) || (key.id == 0)) continue;
    labels.insert(pair<int, fig::fig_object>(key.id, *i));
  }


  //  Обнулим старую карту и будем ее заполнять постепенно из FIG
  // а в NC будем записывать объекты, которые мы не смогли преобразовать...
  MAP.clear();
  fig::fig_world NC; 

  std::cerr << "Merging maps...\n";  
  for (fig::fig_world::iterator i=FIG.begin(); i!=FIG.end(); i++){

    if (i->type == 6){ // составной объект
      // копируем комментарий в следующий объект (до последней непустой строчки!).
      fig::fig_world::iterator j = i; j++;
      if (j!=FIG.end()){
        if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
        for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
      }
      continue;
    }
    if (i->type == -6) continue;

    // некартографические объекты
    if (!zn::is_map_depth(*i)) {
      if (i->comment.size()>1){ 
         if (i->comment[1]=="[skip]") continue;
         zn::zn_label_key k = zn::get_label_key(*i);
         if ((k.id!=0) && (k.map==map_name)) continue; // подпись нам не нужна
      }
      MAP.push_back(*i); 
      continue;
    } 

    // картографические объекты
    zn::zn_key key = zn::get_key(*i);

    if ((key.map == map_name) && (key.id !=0)){ // в объекте есть ключ от этой карты

      zn::zn_key oldkey;
      map<int, fig::fig_object>::iterator o = objects.find(key.id);
      if (o==objects.end()){
        cerr << "Конфликт: объект " << key.id << " был удален\n";
        //... обвести рамкой
//        NC.push_back(*i);
//        continue;
        key.time.set_current();
        key.sid    = 0;
        key.source = source;
        key.id     = maxid;
        key.map    = map_name;
      } else {
        oldkey = zn::get_key(o->second);
        if (oldkey.time > key.time){
          cerr << "Конфликт: объект " << key.id << " был изменен\n";
          //... обвести рамкой
//          NC.push_back(*i);
//          continue; 
          key.sid    = 0;
          key.source = source;
          key.time.set_current();
        } else if ( *i != o->second){
          key.time.set_current();
          key.sid    = 0;
          key.source = source;
        } else key = oldkey;
      }
      // ... проверить бы еще конфликты, когда два однотипных
      // объекта были нарисованы в одном районе!

      zn::add_key(*i, key);  // добавим обновленный ключ
      MAP.push_back(*i);            // запишем объект 

      // теперь еще подписи:
      // вытащим из хэша подписи для этого объекта
      for (multimap<int, fig::fig_object>::const_iterator l = labels.find(key.id); 
          (l != labels.end()) && (l->first == key.id); l++){
        // здесь надо бы еще обработать ситуации, когда объект сдвинут,
        // название объекта поменялось и т.п.
        MAP.push_back(l->second);
      }

      continue; 
    } 
    // остались объекты без ключа или с неполным ключом
    maxid++;
    if (key.type == 0) key.type = zconverter.get_type(*i);
    if (key.type == 0) {
      cerr <<  "Объект неизвестного вида\n";
      NC.push_back(*i);
      continue;
    }
    key.time.set_current();
    key.id     = maxid;
    key.map    = map_name;
    key.source = source;
    key.sid    = 0;
    zn::add_key(*i, key);  // добавим ключ
    MAP.push_back(*i);            // запишем объект
    list<fig::fig_object> new_labels = zconverter.make_labels(*i); // изготовить новые подписи
    MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());   
  }

  std::cerr << MAP.size() << " objects converted\n";
  std::cerr << NC.size() << " objects not converted\n";
 
  // записываем MAP
  ofstream out(file.c_str());
  fig::write(out, MAP);
  ofstream outnc(ncfile.c_str());
  fig::write(outnc, NC);

}
