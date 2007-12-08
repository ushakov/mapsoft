#ifndef ZN_H
#define ZN_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/geo_convs.h"
#include "../geo_io/geofig.h"
#include "../geo_io/mp.h"
#include "../yaml/yaml.h"

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
 
  zn_key(): type(0), id(0), sid(0) {}
};

std::ostream & operator<< (std::ostream & s, const zn_key & t);
std::istream & operator>> (std::istream & s, zn_key & t);

// ключ привязанного объекта
struct zn_label_key{
  int id;             // номер объекта, к которому привязываваемся
  std::string map;    // идентификатор карты
  zn_label_key(const int i=0, const std::string & s=""): id(i), map(s) {}
  zn_label_key(const zn_key & k): id(k.id), map(k.map) {}
  zn_label_key operator= (const zn_key & k){id=k.id; map=k.map;}
  bool operator== (const zn_label_key & k){return (id==k.id)&&(map==k.map);}
  bool operator== (const zn_key & k){return (id==k.id)&&(map==k.map);}
};

std::ostream & operator<< (std::ostream & s, const zn_label_key & t);
std::istream & operator>> (std::istream & s, zn_label_key & t);


// в этом диапазоне глубин должны лежать
// только картографические объекты!
bool is_map_depth(const fig::fig_object & o);

  // Простые операции с ключом 

// Извлечь ключ из комментария (2-я строчка) к fig-объекту
zn_key get_key(const fig::fig_object & fig);

// Извлечь ключ подписи из комментария (2-я строчка) к fig-объекту
zn_label_key get_label_key(const fig::fig_object & fig);

// Извлечь ключ из комментария (1-я строчка) к mp-объекту
// Тип объекта в ключе заполняется из mp-типа, а не из ключа!
// (поскольку из ключа mp-объекта он _никогда_ не должен браться)
zn_key get_key(const mp::mp_object & mp);

// поместить ключ в комментарий к fig-объекту
void add_key(fig::fig_object & fig, const zn_key & key);

// поместить ключ подписи в комментарий к fig-объекту
void add_key(fig::fig_object & fig, const zn_label_key & key);

// поместить ключ в комментарий к mp-объекту
void add_key(mp::mp_object & mp, const zn_key & key);


// информация об условном обозначении, прочитанная из конф.файла
struct zn{
  std::string name, desc;  // название, подробное описание
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
  mp::mp_object default_mp;   // как рисовать неизвестные знаки

  public:
  // Конструктор - чтение конфигурационного файла
  zn_conv(const std::string & conf_file);
  bool load_znaki(YAML::Node &root);


    // Операции с mp- и fig-объектами, для которых 
    // требуется конф.файл

  // определить тип mp-объекта (почти тривиальная функция :))
  int get_type(const mp::mp_object & o) const;

  // определить тип fig-объекта.
  // если есть ключ - тип определяется из него, иначе -
  // по внешнему виду, в соответствии с конф.файлом)
  int get_type(const fig::fig_object & o) const;

  // преобразовать mp-объект в fig-объект
  // ключ сохраняется старый, или создается неполный (только с типом)
  fig::fig_object mp2fig(const mp::mp_object & mp, convs::map2pt & cnv) const;

  // преобразовать fig-объект в mp-объект
  // ключ сохраняется старый, или создается неполный (только с типом)
  // подписи не преобразовываются.
  std::list<mp::mp_object> fig2mp(const fig::fig_object & fig, convs::map2pt & cnv) const;

  // заключить fig-объекты в составной объект. Комментарий
  // составного объекта копируется из первого объекта (!)
  void fig_make_comp(std::list<fig::fig_object> & objects);

  // Подготовить объект для отдачи пользователю:
  // - поменять параметры в соответствии с ключом
  // - создать картинку, если надо
  // Объект должен иметь полный ключ!
  std::list<fig::fig_object> fig2user(const fig::fig_object & fig);

  // Создать подписи к объекту. Объект должен иметь полный ключ!
  std::list<fig::fig_object> make_labels(const fig::fig_object & fig);

  // список всех знаков в формате fig
  fig::fig_world make_legend(int grid = 100, int dg=30);

  // текстовый список всех знаков
  std::string make_text();
};


} // namespace

#endif

