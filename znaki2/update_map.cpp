// Обновление карты из fig или mp файла

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../geo_io/geo_convs.h"

#include "zn.h"
#include "zn_key.h"

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

/*
    Что нам интересно:

 новые:           fig  mp
картогр.объекты   +    +
подписи           +    -
прочие объекты    +    -
 старые:
картогр.объекты   +    +
подписи           -    +
прочие объекты    -    +
*/

  
  fig::fig_world NEW; // новая карта - туда сразу пойдут прочие объекты
  std::multimap<int, fig::fig_object> new_objects; // по типу
  std::map<int, fig::fig_object> old_objects;      // по id
  std::multimap<int, fig::fig_object> labels;      // по id объекта


  int unk_count=0;

  std::cerr << "Reading new map...\n";  
  if (fig_not_mp){ // читаем fig
    fig::fig_world FIG = fig::read(infile.c_str());
    for (fig::fig_world::iterator i=FIG.begin(); i!=FIG.end(); i++){

      if (i->type == 6){ // составной объект
        // копируем комментарий в следующий объект (до последней непустой строчки!).
        // остальное нам не нужно
        fig::fig_world::iterator j = i; j++;
        if (j!=FIG.end()){
          if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
          for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
        }
        continue;
      }
      if (i->type == -6) continue;
      if ((i->comment.size()>1) && (i->comment[1]=="[skip]")) continue;

      // некартографические объекты
      if (!zn::is_map_depth(*i)) {
        if (i->comment.size()>1){ 
          zn::zn_label_key k = zn::get_label_key(*i);
          if ((k.id!=0) && (k.map==map_name)){// подписи
            labels.insert(std::pair<int, fig::fig_object>(k.id, *i)); 
            continue;
          }
        }
        NEW.push_back(*i); 
        continue;
      } 
      else {
        int type = zconverter.get_type(*i);
        if (type==0) {
          cerr << "can't determin object type! Making it's depth=10!!!\n";
          i->depth=10; unk_count++;
          NEW.push_back(*i);
          continue;
        }
        new_objects.insert(std::pair<int, fig::fig_object>(type, *i));
      }
    }
  } 
  else { // читаем mp
    // читаем из старой карты некартографические объекты и подписи
    //составных объектов здесь не ожидается, но на всякий сл. и для единообразия проверим...
    for (fig::fig_world::iterator i=MAP.begin(); i!=MAP.end(); i++){
      if (i->type == 6){ // составной объект
        // копируем комментарий в следующий объект (до последней непустой строчки!).
        // остальное нам не нужно
        fig::fig_world::iterator j = i; j++;
        if (j!=MAP.end()){
          if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
          for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];
        }
        continue;
      }
      if (i->type == -6) continue;
      if ((i->comment.size()>1) && (i->comment[1]=="[skip]")) continue;

      // некартографические объекты
      if (!zn::is_map_depth(*i)) {
        if (i->comment.size()>1){ 
          zn::zn_label_key k = zn::get_label_key(*i);
          if ((k.id!=0) && (k.map==map_name)){// подписи
            labels.insert(std::pair<int, fig::fig_object>(k.id, *i)); 
            continue;
          }
        }
        NEW.push_back(*i); 
        continue;
      } 
    }

    // извлекаем привязку из старой карты:
    g_map ref = fig::get_ref(MAP);
    convs::map2pt cnv(ref, Datum("wgs84"), Proj("lonlat"), Options());

    mp::mp_world MP = mp::read(infile.c_str());

    // mp->fig
    for (mp::mp_world::const_iterator i=MP.begin(); i!=MP.end(); i++){
      int type = zconverter.get_type(*i);
      fig::fig_object fig = zconverter.mp2fig(*i, cnv, type);
      // неизвестные объекты сами должны уйти на глубину 10
      // но на всякий случай продублируем...
      if (type==0) {
        cerr << "can't determin object type! Making it's depth=10!!!\n";
        fig.depth=10; unk_count++;
        NEW.push_back(fig);
        continue;
      }
      new_objects.insert(std::pair<int, fig::fig_object>(type, fig));
    }
  }


  // найдем максимальный id элементов старой карты
  // распихаем старые объекты в хэш
  // тип объектов из хранилища должен определяться только по ключу!
  int maxid=0;
  for (fig::fig_world::const_iterator i=MAP.begin(); i!=MAP.end(); i++){
    if (!zn::is_map_depth(*i)) continue;
    zn::zn_key key = zn::get_key(*i);
    if ((key.map != map_name) || (key.id == 0)){
      std::cerr << "strange object in map storage! skipping...\n";
      continue;
    }
    old_objects.insert(pair<int, fig::fig_object>(key.id, *i));
    if (key.id > maxid) maxid=key.id;
  }




  // Теперь добавляем новые картографические объекты, сравниваем их со старыми и т.п...
  // смотрим на подписи...

  int new_count = 0;
  int mod_count = 0;
  int old_count = 0;
  int con_count = 0;

  std::cerr << "Merging maps...\n";  
  for (std::multimap<int, fig::fig_object>::iterator i = new_objects.begin(); i!=new_objects.end(); i++){

    zn::zn_key key = zn::get_key(i->second);

    // Два случая: либо в объекте нет ключа, либо есть. Первый случай проще:
    if ((key.map != map_name) || (key.id ==0)){ 
      maxid++; new_count++;
      key.type   = i->first; // тип выставляется по виду объекта
      key.time.set_current();
      key.id     = maxid;
      key.map    = map_name;
      key.source = source;
      key.sid    = 0;
      zn::add_key(i->second, key);  // добавим ключ
      MAP.push_back(i->second);     // запишем объект
      list<fig::fig_object> l1 = zconverter.make_labels(i->second); // изготовим новые подписи
      MAP.insert(MAP.end(), l1.begin(), l1.end());   
    }
    else { // В объекте есть ключ. Тут все сложнее
      key.type = i->first; // сразу, чтоб не забыть. тип нового объекта - по его виду.
      // если старого объекта с таким id нет, значит его удалили
      // после звятия карты на редактирование
      map<int, fig::fig_object>::iterator o = old_objects.find(key.id);
      if (o==old_objects.end()){
        cerr << "Конфликт: из системы объект " << key.id << " был удален,\n";
        cerr << "а вы его опять туда запихиваете... :( В глубину 11 его!\n";
        i->second.depth = 11; con_count++;
        MAP.push_back(i->second);
        continue;
      }
      zn::zn_key oldkey = zn::get_key(o->second);
      // если старый объект новее нового
      if (oldkey.time > key.time){
        cerr << "Конфликт: объект " << key.id << " был изменен,\n";
        cerr << "а вы старую версию пытаетесь положить... :( В глубину 11 ее!\n";
        i->second.depth = 11; con_count++;
        MAP.push_back(i->second);
        // новый объект подменим старым...
        i->second = o->second;
        key = oldkey;
      }
      // если объект изменился - заменим время в ключе и источник.
      // сравниваем тип, текст и координаты
      if ((i->first != oldkey.type) || 
          (i->second.text != o->second.text) || 
          (i->second != o->second)){
        key.time.set_current();
        key.source = source;
        mod_count++;
      } else old_count++;

      zn::add_key(i->second, key);  // добавим обновленный ключ
      MAP.push_back(i->second);     // запишем объект

      // теперь еще подписи:
      // если у объекта есть название, но нет подписи - сделаем ее
      if ((i->second.comment.size()>0) && 
          (i->second.comment[0] != "") &&
          (labels.count(key.id) == 0)){
        list<fig::fig_object> l1 = zconverter.make_labels(i->second); // изготовим новые подписи
        MAP.insert(MAP.end(), l1.begin(), l1.end());   
        continue;
      } 
      // вытащим из хэша все старые подписи для этого объекта
      for (multimap<int, fig::fig_object>::iterator l = labels.find(key.id); 
          (l != labels.end()) && (l->first == key.id); l++){
        // текст подписи = название объекта
        if (i->second.comment.size()>0) l->second.text = i->second.comment[0];
        else l->second.text = "";

        if (l->second.text !="") MAP.push_back(l->second);
      }

    }
  }

  std::cerr << new_count << " new objects\n";
  std::cerr << mod_count << " modified objects\n";
  std::cerr << old_count << " non-modified objects\n";
  std::cerr << unk_count << " unknown objects (see depth 10)\n";
  std::cerr << con_count << " conflicts (see depth 11)\n";
  std::cerr << MAP.size() << " objects \n";
  // записываем MAP
  ofstream out(file.c_str());
  fig::write(out, MAP);

}
