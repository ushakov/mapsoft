#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"

#include "../utils/m_time.h"

namespace zn{

// ключ объекта
struct zn_key{
  int  mp_type;            // тип в mp-файле
  char mp_class;           // 'p', 'l', 'a', для точечных, линейных, площадных объектов

  Time time;               // время последнего изменения
  
  std::string map_name;    // идентификатор карты
  std::string source;      // идентификатор источника
  int id;                  // уникальный номер на карте
  int source_id;           // уникальный номер в источнике
 
  bool label;
  bool empty;

  zn_key():empty(true){}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);

// информация об условном обозначении, прочитанная из конф.файла
struct zn{
  std::string name, descr; // название, подробное описание
  std::string fig_mask;    // маска для создания fig-объекта
  std::string txt_mask;    // маска для создания подписи (м.б. пустой)
  std::string pic;         // картинка для точечного объекта (м.б. пустой)
};

// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать
class zn_conv{
  std::map<int, zn> zn_p, zn_l, zn_a;

  public:
  zn_conv(const std::string & conf_file);

  // создать новый ключ по виду fig-объекта
  // (заполняются только поля mp_type и mp_class)
  zn_key make_key(const fig::fig_object & fig);


  // преобразовать mp-объект в fig-объект
  fig::fig_object mp2fig(const mp::mp_object & mp, g_map & ref);

  // преобразовать fig-объект в mp-объект
  // (если ключа нет - тип объекта определяется процедурой make_key)
  mp::mp_object fig2mp(const fig::fig_object & fig, g_map & ref);


  // извлечь ключ из комментария к fig-объекту
  zn_key get_key(const fig::fig_object & fig); 
  zn_key get_key(const mp::mp_object & mp); 

  // поместить ключ в комментарий к fig-объекту
  zn_key add_key(const fig::fig_object & fig, const zn_key & key); 
  zn_key add_key(const mp::mp_object & mp, const zn_key & key); 

  // подготовить объект для отдачи пользователю:
  // - поменять параметры в соответствии с ключом
  // - создать картинку, если надо
  // ключ берется из комментария к объекту.
  std::list<fig::fig_object> fig2user(const fig::fig_object & fig);

  // - создать подписи к объекту
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig, const zn_key & key);
};


} // namespace

#endif

