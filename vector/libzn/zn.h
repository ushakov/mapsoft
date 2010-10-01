#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include <set>
#include "../../core/libgeo/geo_convs.h"
#include "../../core/libgeo_io/geofig.h"
#include "../../core/libmp/mp.h"

#include "../../core/utils/m_time.h"

// операции с конф.файлом и типами объектов
// хочется, чтоб этот класс ничего не знал про ключи, а работал только с типами...

namespace zn{

const int line_mask = 0x100000; // сколько добавлять к типу для линейных объектов
const int area_mask = 0x200000; // сколько добавлять к типу для площадных объектов


// copy comment from compound to the first object in it
void fig_copy_comment(const fig::fig_world::iterator & i,
                      const fig::fig_world::iterator & end);

// is object need to be skipped (compounds, pics, [skip])
bool is_to_skip(fig::fig_object o);

// convert fig arrow to direction
int  fig_arr2dir(const fig::fig_object & f, bool text=false);
void fig_dir2arr(fig::fig_object & f, int dir, bool text=false);

// Remove compounds and objects with [skip] comment
void fig_remove_pics(fig::fig_world & F);

// Find nearets point in the line
double dist( const iPoint & p, const iLine & l, iPoint & nearest);



// информация об условном обозначении, прочитанная из конф.файла
struct zn{
  std::string name, desc;  // название, подробное описание
  mp::mp_object mp;        // заготовка mp-объекта
  fig::fig_object fig;     // заготовка fig-объекта
  fig::fig_object txt;     // заготовка fig-объекта подписи
  std::string pic;         // картинка для точечного объекта (м.б. пустой)
  bool istxt;              // делать ли подпись? // использовать label_type
  int label_pos;           // положение подписи
  int label_dir;           // направление подписи: 0 - center, 1 - left, 2 - right
  // положение подписи:
  // 0 - отсутствует
  // 1 - справа-сверху (все, что с выровненным направо текстом)
  // 2 - слева-сверху (все, что с выровненным налево текстом)
  // 3 - в центре (точки и полигоны с центрованным текстом)
  // 4 - вдоль линии (линии с центрованным текстом)
  zn(){label_pos=0; label_dir=0;}
};


// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать
class zn_conv{
  public:

  std::map<int, zn> znaki;      // упорядочены по типу
  fig::fig_object default_fig;  // как рисовать неизвестные знаки
  fig::fig_object default_txt;  // как рисовать неизвестные подписи
  mp::mp_object default_mp;     // как рисовать неизвестные знаки

  std::set<int> unknown_types; // база неизвестных типов, чтоб на каждый ругаться единожды
  int min_depth, max_depth;

  // Конструктор - чтение конфигурационного файла
  // (./<style>.cnf, /usr/share/mapsoft/<style>.cnf)
  zn_conv(const std::string & style);

    // Операции с mp- и fig-объектами, для которых 
    // требуется конф.файл

  // определить тип mp-объекта (почти тривиальная функция :))
  int get_type(const mp::mp_object & o) const;

  // определить тип fig-объекта по внешнему виду, в соответствии с конф.файлом
  int get_type(const fig::fig_object & o) const;

  // Получить заготовку fig-объекта заданного типа
  fig::fig_object get_fig_template(int type);

  // Получить заготовку mp-объекта заданного типа
  mp::mp_object get_mp_template(int type);

  // Получить заготовку fig-подписи заданного типа
  fig::fig_object get_label_template(int type);


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
  void label_update(fig::fig_object & fig, int type);

  //Грубая проверка, является ли объект картографическим объектом.
  //Лежит ли его глубина в диапазоне между максимальной и минимальной глубиной
  //объектов из конф.файла.
  bool is_map_depth(const fig::fig_object & o) const;

  /// functions for updating fig map
  int fig_add_pics(fig::fig_world & F);
  int fig_update_labels(fig::fig_world & F);
};

} // namespace

#endif

