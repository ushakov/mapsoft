#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/geo_convs.h"
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"

#include "../utils/m_time.h"

namespace zn{

const int line_mask = 0x100000; // сколько добавлять к типу для линейных объектов
const int area_mask = 0x200000; // сколько добавлять к типу для площадных объектов


// ключ объекта
struct zn_key{
  int  type;           // тип в mp-файле + line_mask или area_mask

  Time time;          // время последнего изменения
  
  int id;             // уникальный номер на карте
  std::string map;    // идентификатор карты
  int sid;            // уникальный номер в источнике
  std::string source; // идентификатор источника
 
  bool label;

  zn_key():mp_type(0), id(0), sid(0), label(false){}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);



// информация об условном обозначении, прочитанная из конф.файла
struct zn{
  std::string name, descr; // название, подробное описание
  mp::mp_object mp;        // заготовка mp-объекта
  fig::fig_object fig;     // заготовка fig-объекта
  fig::fig_object txt;     // заготовка fig-объекта подписи
  std::string pic;         // картинка для точечного объекта (м.б. пустой)
  bool istxt;              // делать ли подпись?
};

// класс, знающий (из конф.файла) про конкретные условные обозначения 
// и умеющий их преобразовывать
class zn_conv{
  std::map<int, zn> znaki;      // упорядочены по типу
  fig::fig_object default_fig;  // как рисовать неизвестные знаки
  fig::fig_object default_mp;   // как рисовать неизвестные знаки

  public:
  // Конструктор - чтение конфигурационного файла
  zn_conv(const std::string & conf_file);


    // Простые операции с ключом 

  // Извлечь ключ из комментария (2-я строчка) к fig-объекту
  zn_key get_key(const fig::fig_object & fig) const;

  // Извлечь ключ из комментария (1-я строчка) к mp-объекту
  // Тип объекта в ключе заполняется из mp-типа, а не из ключа!
  // (поскольку из ключа mp-объекта он _никогда_ не должен браться)
  zn_key get_key(const mp::mp_object & mp) const;

  // поместить ключ в комментарий к fig-объекту
  void add_key(fig::fig_object & fig, const zn_key & key) const;

  // поместить ключ в комментарий к mp-объекту
  void add_key(mp::mp_object & mp, const zn_key & key) const;



    // Операции с mp- и fig-объектами, для которых 
    // требуется конф.файл

  // определить тип mp-объекта (почти тривиальная функция :))
  int zn_conv::get_type(const mp::mp_object & o) const;

  // определить тип fig-объекта.
  // если есть ключ - тип определяется из него, иначе -
  // по внешнему виду, в соответствии с конф.файлом)
  int zn_conv::get_type(const fig::fig_object & o) const;



  // преобразовать mp-объект в fig-объект
  // ключ сохраняется старый, или создается неполный (только с типом)
  fig::fig_object mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const;

  // преобразовать fig-объект в mp-объект
  // ключ сохраняется старый, или создается неполный (только с типом)
  mp::mp_object fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const;



  // подготовить объект для отдачи пользователю:
  // - поменять параметры в соответствии с ключом
  // - создать картинку, если надо
  // ключ берется из комментария к объекту.
  std::list<fig::fig_object> fig2user(const fig::fig_object & fig){
    // ...
  }

  // создать подписи к объекту. Объект должен иметь полный ключ!
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig){

    txt_dist = 10; // fig units

    std::list<fig::fig_object> ret;
    zn_key key = get_key(fig);
    if (fig.size() < 1) return ret;                    // странный объект
    if ((key.id == 0) && (key.type == 0)) return ret;  // неполный ключ
    if (znaki.find(key.type)==znaki.end()) return ret; // про такой тип объектов неизвестно
    if (!znaki[key.type].istxt) return ret; // подпись не нужна
    // заготовка для подписи
    fig::fig_object txt = znaki[fig.type].txt;

    // определим координаты и наклон подписи
    Point<int> p = fig[0] + Point<int>(1,1)*txt_dist;
    if (fig.size()>=2){
      if ((key.type > line_mask) && (key.type < area_mask)){ // линия
        p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
        Point<int> v = fig[fig.size()/2-1] - fig[fig.size()/2];
        if (v.x<0) v.x=-v.x;
        txt.angle = atan2(v.x, v.y);
        txt.sub_type = 1; // centered text
        Point<int> vp(-v.y, v.x);
        p+=vp*txt_dist;
      } 
      else { // площадной объект
        Point<int> max = p;
        for (int i = 0; i<fig.size(); i++){
          
        }
      }
    }
    txt.push_back(p);
    ret.push_back(txt)
    return(ret);
  }
};


} // namespace

#endif

