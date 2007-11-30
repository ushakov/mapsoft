#include <fstream>
#include "zn.h"

namespace zn{

/*// ключ объекта
struct zn_key{
  std::string map_name;    // идентификатор карты, для которой объект был создан
  int id;                  // уникальный номер объекта на карте
  char mp_class;           // 'p', 'l', 'a', для точечных, линейных, площадных объектов
  int  mp_type;            // тип в mp-файле

  Time time;               // время последнего изменения
  
  std::string source;      // источник
  int source_id;           // уникальный номер в источнике
 
  bool label;
};*/

std::ostream & operator<< (std::ostream & s, const zn_key & t){
  if (t.label) 
    s << "+ " << t.map_name << " " << t.id << "\n";
  else
    s << t.map_name << " " << t.id << " " << t.mp_class << " " << t.mp_type << " " 
      << t.time << " " << t.source << " " << t.source_id;
  return s;
}
std::istream & operator>> (std::istream & s, zn_key & t){
}

// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать

zn_conv::zn_conv(const std::string & conf_file){
  // читаем конф.файл.
  std::ifstream conf(conf_file.c_str());
  if (!conf) {
    std::cerr << "Can't open " << conf_file << "\n";
    exit(0);
  }
  // простой конф.файл: 
  // mp_mask
  // fig_mask
  // fig_mask for text
  // fig_mask for text

  zn               z;
  mp::mp_object    m;
  std::string      str;
  while (!conf.eof()){
   do { getline(conf, str); } while (str == "");
   m = mp::make_object(str);
   getline(conf, z.fig_mask); 
   getline(conf, z.txt_mask); 
   while (getline(conf,str), str!="") { z.pic+=str;}
   if      (m.Class == "POI")      zn_p.insert(std::pair<int, zn>(m.Type, z));
   else if (m.Class == "POLYLINE") zn_l.insert(std::pair<int, zn>(m.Type, z));
   else if (m.Class == "POLYGON")  zn_a.insert(std::pair<int, zn>(m.Type, z));
   else {std::cerr << "unknown mp-class in conf file: " << m.Class << "\n";}
  }
}



} // namespace

