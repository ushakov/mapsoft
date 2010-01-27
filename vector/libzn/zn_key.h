#ifndef ZN_KEY_H
#define ZN_KEY_H

#include <string>
#include <list>
#include <map>
#include "../../core/libgeo/geo_convs.h"
#include "../../core/libgeo_io/geofig.h"
#include "../../core/libmp/mp.h"

#include "../../core/utils/m_time.h"

namespace zn{

typedef unsigned long long  id_type;
// ключ объекта
struct zn_key{
  int  type;           // тип в mp-файле + line_mask или area_mask

  Time time;          // время последнего изменения

  id_type id;         // уникальный номер на карте
  std::string map;    // идентификатор карты
  int sid;            // уникальный номер в источнике
  std::string source; // идентификатор источника
 
  zn_key(): type(0), id(0), sid(0) {}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);

// ключ привязанного объекта
struct zn_label_key{
  id_type id;         // номер объекта, к которому привязываваемся
  std::string map;    // идентификатор карты
  zn_label_key(const id_type i=0, const std::string & s=""): id(i), map(s) {}
  zn_label_key(const zn_key & k): id(k.id), map(k.map) {}
  zn_label_key operator= (const zn_key & k){id=k.id; map=k.map;}
  bool operator== (const zn_label_key & k){return (id==k.id)&&(map==k.map);}
  bool operator== (const zn_key & k){return (id==k.id)&&(map==k.map);}
};

std::ostream & operator<< (std::ostream & s, const zn_label_key & t);
std::istream & operator>> (std::istream & s, zn_label_key & t);


  // Простые операции с ключом

// make unique id
id_type make_id(void);

// make new key with unique id
zn_key make_key(const std::string & map, const std::string & editor);

// Извлечь ключ из комментария (2-я строчка) к fig-объекту
zn_key get_key(const fig::fig_object & fig);

// Извлечь ключ подписи из комментария (2-я строчка) к fig-объекту
zn_label_key get_label_key(const fig::fig_object & fig);

// Извлечь ключ из комментария (1-я строчка) к mp-объекту
zn_key get_key(const mp::mp_object & mp);

// поместить ключ в комментарий к fig-объекту
void add_key(fig::fig_object & fig, const zn_key & key);

// поместить ключ подписи в комментарий к fig-объекту
void add_key(fig::fig_object & fig, const zn_label_key & key);
void add_key(std::list<fig::fig_object> & l, const zn_label_key & key);

// поместить ключ в комментарий к mp-объекту
void add_key(mp::mp_object & mp, const zn_key & key);

// удалить любой ключ (да и всю вторую стоку комментария)
void clear_key(fig::fig_object & fig);
void clear_key(mp::mp_object & mp);

// convert old-style pictures and labels
void fig_old2new(fig::fig_world & F);

} // namespace

#endif

