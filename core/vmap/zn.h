#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include <set>
#include "geo/geo_convs.h"
#include "geo_io/geofig.h"
#include "mp/mp.h"

#include "options/m_time.h"

// операции с конф.файлом и типами объектов

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
  std::string pic;         // картинка (м.б. пустой)
  std::string pic_type;    // для многоугольников "fill": использовать заливку
                           // картинкой
  int ocad, ocad_txt;      // номера символов OCAD
  bool istxt;              // делать ли подпись? // использовать label_type
  int label_type;          // положение подписи
                           // 0 - отсутствует
                           // 1 - справа-сверху (все, что с выровненным направо текстом)
                           // 2 - слева-сверху (все, что с выровненным налево текстом)
                           // 3 - в центре (точки и полигоны с центрованным текстом)
                           // 4 - вдоль линии (линии с центрованным текстом)
  int label_dir;           // направление подписи:
                           // 0 - center, 1 - left, 2 - right
  std::vector<int> move_to; // список объектов, к которым надо подвинуть данный
  bool rotate;              // надо ли при этом поворачивать
  int replace_by;           // на какой объект заменять (для устаревших объектов)
  int curve;                // параметр кривой (метры изгиба?)

  // положение подписи:
  zn(){label_type=0; label_dir=0; rotate=false; replace_by=0; curve=0;}
};


// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать
class zn_conv{
  public:

  std::map<int, zn> znaki;      // упорядочены по типу
  fig::fig_object default_fig;  // как рисовать неизвестные знаки
  fig::fig_object default_txt;  // как рисовать неизвестные подписи
  mp::mp_object default_mp;     // как рисовать неизвестные знаки
  int default_ocad, default_ocad_txt;
  const std::string style;

  std::set<int> unknown_types; // база неизвестных типов, чтоб на каждый ругаться единожды
  int min_depth, max_depth;

  // Конструктор - чтение конфигурационного файла
  // (./<style>.cnf, /usr/share/mapsoft/<style>.cnf)
  zn_conv(const std::string & style);

    // Операции с mp- и fig-объектами, для которых 
    // требуется конф.файл

  const std::string & get_style() const {return style;}

  // определить тип mp-объекта (почти тривиальная функция :))
  int get_type(const mp::mp_object & o) const;

  // определить тип fig-объекта по внешнему виду, в соответствии с конф.файлом
  int get_type(const fig::fig_object & o) const;

  // определить тип по номеру ocad-объекта
  int get_type(const int ocad_type) const;


  // то же, что и find, но ругается, если тип не найден и
  // про него еще не ругались
  std::map<int, zn>::const_iterator find_type(int type);

  // Получить заготовку fig-объекта заданного типа
  fig::fig_object get_fig_template(int type);

  // Получить заготовку mp-объекта заданного типа
  mp::mp_object get_mp_template(int type);

  // Получить номер объекта ocad
  int get_ocad_type(int type);

  // Получить заготовку fig-подписи заданного типа
  fig::fig_object get_label_template(int type);

  // Получить номер объекта для подписи ocad
  int get_ocad_label_type(int type);

  // Получить тип и направление подписи (по умолчанию 0)
  int get_label_type(int type);
  int get_label_dir(int type);

      // в следующих функциях, если указан тип 0, то он определяется по объекту

  // Создать картинку к объекту
  std::list<fig::fig_object> make_pic(const fig::fig_object & fig, int type=0);

  //Грубая проверка, является ли объект картографическим объектом.
  //Лежит ли его глубина в диапазоне между максимальной и минимальной глубиной
  //объектов из конф.файла.
  bool is_map_depth(const fig::fig_object & o) const;


  // Все следующие функции, насколько я понимаю, становятся ненужными при переходе на vmap3:
  // пока они используются в mapsoft_vmap и к.

  // Преобразовать mp-объект в fig-объекты
  std::list<fig::fig_object> mp2fig(const mp::mp_object & mp, convs::map2pt & cnv, int type=0);

  // преобразовать fig-объект в mp-объект
  mp::mp_object fig2mp(const fig::fig_object & fig, convs::map2pt & cnv, int type=0);

  // Поменять параметры в соответствии с типом
  void fig_update(fig::fig_object & fig, int type=0);

  // Создать подписи к объекту
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig, int type=0); 

  // Поменять параметры подписи в соответствии с типом
  // (шрифт, размер, цвет, глубина)
  // Если тип 0 - ничего не менять
  void label_update(fig::fig_object & fig, int type);

  /// functions for updating fig map
  int fig_add_pics(fig::fig_world & F);
  int fig_update_labels(fig::fig_world & F);
};

} // namespace

#endif

