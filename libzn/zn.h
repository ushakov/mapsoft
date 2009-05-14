#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include <set>
#include "../libgeo/geo_convs.h"
#include "../libgeo_io/geofig.h"
#include "../libmp/mp.h"

#include "../utils/m_time.h"

// операции с конф.файлом и типами объектов
// хочется, чтоб этот класс ничего не знал про ключи, а работал только с типами...

namespace zn{

const int line_mask = 0x100000; // сколько добавлять к типу для линейных объектов
const int area_mask = 0x200000; // сколько добавлять к типу для площадных объектов

// информация об условном обозначении, прочитанная из конф.файла
struct zn{
  std::string name, desc;  // название, подробное описание
  mp::mp_object mp;        // заготовка mp-объекта
  fig::fig_object fig;     // заготовка fig-объекта
  fig::fig_object txt;     // заготовка fig-объекта подписи
  std::string pic;         // картинка для точечного объекта (м.б. пустой)
  bool istxt;              // делать ли подпись?
  zn(){istxt=false;}
};


// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать
class zn_conv{
  public:

  std::map<int, zn> znaki;      // упорядочены по типу
  fig::fig_object default_fig;  // как рисовать неизвестные знаки
  mp::mp_object default_mp;     // как рисовать неизвестные знаки

  std::set<int> unknown_types; // база неизвестных типов, чтоб на каждый ругаться единожды
  int min_depth, max_depth;

  // Конструктор - чтение конфигурационного файла
  zn_conv(const std::string & conf_file);

    // Операции с mp- и fig-объектами, для которых 
    // требуется конф.файл

  // определить тип mp-объекта (почти тривиальная функция :))
  int get_type(const mp::mp_object & o) const;

  // определить тип fig-объекта по внешнему виду, в соответствии с конф.файлом
  int get_type(const fig::fig_object & o) const;

      // в следующих функциях, если указан тип 0, то он определяется по объекту

  // Преобразовать mp-объект в fig-объекты
  std::list<fig::fig_object> mp2fig(const mp::mp_object & mp, convs::map2pt & cnv, int type=0);

  // преобразовать fig-объект в mp-объект
  mp::mp_object fig2mp(const fig::fig_object & fig, convs::map2pt & cnv, int type=0);

  // Поменять параметры в соответствии с типом
  void fig_update(fig::fig_object & fig, int type=0);

  // Создать картинку к объекту
  std::list<fig::fig_object> make_pic(const fig::fig_object & fig, int type=0);

  // Создать подписи к объекту
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig, int type=0);

  // Поменять параметры подписи в соответствии с типом
  // (шрифт, размер, цвет, глубина)
  // Если тип 0 - ничего не менять
  void label_update(fig::fig_object & fig, int type) const;

  //Грубая проверка, является ли объект картографическим объектом.
  //Лежит ли его глубина в диапазоне между максимальной и минимальной глубиной
  //объектов из конф.файла.
  bool is_map_depth(const fig::fig_object & o);
};


} // namespace

#endif

