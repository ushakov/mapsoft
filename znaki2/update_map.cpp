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

  // пробуем прочитать карту
  std::cerr << "Reading old map...\n";  
  fig::fig_world MAP = fig::read(file.c_str());
  if (MAP.size()==0) {
    cerr << "bad file " << file << "\n";
    exit(0);
  }
  // извлекаем привязку
  std::cerr << "Getting old ref...\n";  
  g_map ref = fig::get_ref(MAP);
  convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

  zn::zn_conv zconverter(conf_file);

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
  

  fig::fig_world FIG; 
  // заготовка для новой карты
  // Если мы обновляемся из fig - прочитаем сюда
  // все объекты из fig-файла
  // Если из mp - то сетки и т.п. возьмем из старого файла,
  // а объекты преобразуем

  std::cerr << "Reading new map...\n";  
  if (testext(infile, ".fig")){ // читаем fig
    FIG = fig::read(infile.c_str());
    /// ... преобразовать координаты!
  } else
  if (testext(infile, ".mp")){ // читаем mp
    mp::mp_world MP = mp::read(infile.c_str());
    // копируем в FIG из MAP все объекты с глубинами 1-44 и 400-999
    // (сетку, привязку и т.п.)
    for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++)
      if ((i->depth <45) || (i->depth >=400)) FIG.push_back(*i);
    // преобразуем объекты из MP в FIG
    // для новых объектов (без ключа) создается неполный ключ - только с типом
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++)
      FIG.push_back(zconverter.mp2fig(*i, cnv));
  } else usage();
  // Теперь в FIG у нас новая карта с привязкой.


  // найдем максимальный id элементов старой карты
  // распихаем объекты и подписи в хэши
  map<int, fig::fig_object> objects;
  multimap<int, fig::fig_object> labels;

  std::cerr << "Reading old map...\n";  
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if ((i->depth < 50)||(i->depth>=400)) continue;
    zn::zn_key key = zconverter.get_key(*i);
    if ((key.map != map_name) || (key.id == 0)) continue;
    if (key.label) labels.insert(pair<int, fig::fig_object>(key.id, *i));
    else objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }

  //  Обнулим старую карту и будем ее заполнять постепенно из FIG
  MAP.clear();
  // а в NC будем записывать объекты, которые мы не смогли преобразовать...
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
    }

    // объекты с глубинами 1-44 и 400-999 (сетка, привязка и т.п.)
    if ((i->depth <45) || (i->depth >=400)) {
      MAP.push_back(*i); 
      continue;
    } 
    // объекты с глубинами 45-49 (картинки, которые нам _вообще_ не нужны)
    if ((i->depth >=45) && (i->depth <50)) continue;

    // не-линии - копируем без изменений
    if ((i->type !=2) && (i->type !=3) && (i->type !=4)){
      MAP.push_back(*i); 
      continue;
    }

    // остальные объекты
    zn::zn_key key = zconverter.get_key(*i);

    if ((key.map == map_name) && (key.id !=0)){ // есть старый ключ от этой карты
      map<int, fig::fig_object>::iterator o = objects.find(key.id);
      if (o==objects.end()){
        cerr << "Конфликт: объект " << key.id << " был удален\n";
        cerr << "не добавляю его!\n";
        //... обвести рамкой
        NC.push_back(*i);
        continue;
      }
      zn::zn_key oldkey = zconverter.get_key(o->second);
      if (oldkey.time > key.time){
        cerr << "Конфликт: объект " << key.id << " был изменен\n";
        cerr << "не добавляю его!\n";
        //... обвести рамкой
        NC.push_back(*i);
        continue;
      }
      // ... проверить еще конфликты, когда два однотипных
      // объекта были нарисованы в одном районе!

      key.time.set_current();
      key.sid    = 0;
      key.source = source;

      zconverter.add_key(*i, key);  // добавим ключ
      MAP.push_back(*i);            // запишем объект 

      // теперь еще подписи:
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
        new_labels = zconverter.make_labels(*i);
      }
      MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());  // записать подписи
      continue;
    } 
    // остались объекты без ключа или с неполным ключом
    maxid++;
    if (key.type == 0) key.type = zconverter.get_type(*i);
    if (key.type == 0) {
      NC.push_back(*i);
      continue;
    }
    key.time.set_current();
    key.id     = maxid;
    key.map    = map_name;
    key.source = source;
    key.sid    = 0;
    zconverter.add_key(*i, key);  // добавим ключ
    MAP.push_back(*i);            // запишем объект
    list<fig::fig_object> new_labels = zconverter.make_labels(*i); // изготовить новые подписи
    MAP.insert(MAP.end(), new_labels.begin(), new_labels.end());        // записать подписи
  }

  std::cerr << MAP.size() << " objects converted\n";
  std::cerr << NC.size() << " objects not converted\n";
  for (fig::fig_world::iterator i = MAP.begin(); i!=MAP.end(); i++){
    std::cerr << i->type << " ";
  }
 
  // записываем MAP
  ofstream out(file.c_str());
  fig::write(out, MAP);
  ofstream outnc(ncfile.c_str());
  fig::write(outnc, NC);

}
