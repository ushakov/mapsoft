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
  // (./<style>.cnf, /usr/share/mapsoft/<style>.cnf)
  zn_conv(const std::string & style);

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


  /// functions for updating fig map

  // copy comment from compound to the first object in it
  void fig_copy_comment(const fig::fig_world::iterator & i,
                        const fig::fig_world::iterator & end){
    if (i->type == 6){ // составной объект
      // копируем первые непустые строки комментария в следующий объект
      // остальное нам не нужно
      fig::fig_world::iterator j=i; j++;

      // пропускаем подписи
      while ((j!=end) && (j->comment.size()>1) && (j->comment[1]=="[skip]")) j++;

      if ((j!=end) && (i->comment.size()>0)){
        if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
        for (int n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];

      }
    }
  }

  // Remove compounds and objects with [skip] comment
  void fig_remove_pics(fig::fig_world & F){
    fig::fig_world::iterator i=F.begin();
    while (i!=F.end()){
      // * copy comments from compounds, remove compounds
      // * remove pictures MapType=pic
      if (i->type==6) fig_copy_comment(i, F.end());
      if ((i->type==6) || (i->type==-6) ||
          (i->opts.get("MapType", std::string()) == "pic")){
        i=F.erase(i);
        continue;
      }
      i++;
    }
  }

  int fig_add_pics(fig::fig_world & F){
    fig::fig_world::iterator i=F.begin();
    int count=0;

    while (i!=F.end()){
      if (is_map_depth(*i)){
        std::list<fig::fig_object> l1 = make_pic(*i, get_type(*i));
        if (l1.size()>1){
          count++;
          i=F.erase(i);
          F.insert(i, l1.begin(), l1.end());
          continue;
        }
      }
      i++;
    }
    return count;
  }

  double dist( const iPoint & p, const iLine & l, iPoint & nearest){
    if (l.size()<1) return 1e99;
    double ret=pdist(p,l[0]);
    nearest=l[0];
    for (iLine::const_iterator i=l.begin(); i!=l.end(); i++){
      if (ret > pdist(p, *i)){
        ret=pdist(p, *i);
        nearest=*i;
      }
    }
    return ret;
  }

  int fig_update_labels(fig::fig_world & F){

    double maxd=0.2*fig::cm2fig;

    fig::fig_world::iterator i;
    int count=0;

    // find all labels
    std::list<fig::fig_world::iterator> labels;
    for (i=F.begin(); i!=F.end(); i++){
      if ((i->opts.exists("MapType")) &&
          (i->opts["MapType"]=="label") &&
          (i->opts.exists("RefPt")) ){
        labels.push_back(i);
      }
    }

    // find all objects with title
    std::list<fig::fig_world::iterator> needlabel;
    for (i=F.begin(); i!=F.end(); i++){
      if (!is_map_depth(*i) || (i->size() <1)) continue;
      if ((i->comment.size()>0) && (i->comment[0]!="")) needlabel.push_back(i);
    }

    std::list<fig::fig_world::iterator>::iterator o,l;

    // first pass: labels with correct text
    o=needlabel.begin();
    while (o!=needlabel.end()){
      bool found=false;
      l=labels.begin();
      while (l!=labels.end()){
        iPoint nearest;
        if (((*l)->text == (*o)->comment[0]) &&
            (dist((*l)->opts.get("RefPt", iPoint(0,0)), **o, nearest) < maxd)){
          (*l)->opts.put("RefPt", nearest);
          found=true;
          label_update(**l, get_type(**o));
          l=labels.erase(l);
          continue;
        }
        l++;
      }
      if (found) o=needlabel.erase(o); else o++;
    }

    // second pass: labels with changed text
    o=needlabel.begin();
    while (o!=needlabel.end()){
      bool found=false;
      l=labels.begin();
      while (l!=labels.end()){
        iPoint nearest;
        if (dist((*l)->opts.get("RefPt", iPoint(0,0)), **o, nearest) < maxd){
          (*l)->text=(*o)->comment[0];
          (*l)->opts.put("RefPt", nearest);
          found=true;
          label_update(**l, get_type(**o));
          l=labels.erase(l);
          continue;
        }
        l++;
      }
      if (found) o=needlabel.erase(o); else o++;
    }

    // 3rd pass: create new labels
    for (o=needlabel.begin(); o!=needlabel.end(); o++){
      std::list<fig::fig_object> L=make_labels(**o);
      for (std::list<fig::fig_object>::iterator j=L.begin(); j!=L.end(); j++){
        if (j->size() < 1) continue;
        iPoint nearest;
        double d=dist((*j)[0], **o, nearest);
        j->opts["MapType"]="label";
        j->opts.put("RefPt", nearest);
        F.push_back(*j);
      }
    }

    // 4th pass: remove unused labels
    for (l=labels.begin(); l!=labels.end(); l++) F.erase(*l);

    return count;
  }

};


} // namespace

#endif

