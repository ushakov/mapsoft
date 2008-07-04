#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "zn.h"

#include "../lib2d/point_utils.h"

#include <boost/lexical_cast.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

namespace zn{

// в этом диапазоне глубин должны лежать 
// только картографические объекты!
bool zn_conv::is_map_depth(const fig::fig_object & o){
  return ((o.type!=6) && (o.type!=-6) && (o.depth>=min_depth) && (o.depth<=max_depth));
}

// Заполняет массив знаков 
bool zn_conv::load_znaki(YAML::Node &root) {
   using namespace YAML;
   if (root.type != ARRAY) return false;

   min_depth=999;
   max_depth=0;
   znaki.clear();
   NodeList::iterator it = root.value_array.begin();
   for (it;it!=root.value_array.end();it++) {
      Node child = *it;
      if (child.type != HASH)
        return false;

      zn z;

      const char *v = child.hash_str_value("name");
      if (v==NULL) return false;
      z.name = v;

      v = child.hash_str_value("mp");
      if (v==NULL) return false;
      z.mp = mp::make_object(v);

      v = child.hash_str_value("fig");
      if (v==NULL) return false;
      z.fig = fig::make_object(v);
      if (min_depth>z.fig.depth) min_depth=z.fig.depth;
      if (max_depth<z.fig.depth) max_depth=z.fig.depth;

      v = child.hash_str_value("desc");
      if (v!=NULL) z.desc = v;

      v = child.hash_str_value("txt");
      if (v==NULL) z.istxt=false;
      else {
        z.istxt=true;
        z.txt = fig::make_object(v);
        if (z.txt.type!=4){ 
          std::cerr << "reading conf.file: not a text in txt object in "<< z.name <<"\n";
          return false;
        }
      }

      v = child.hash_str_value("pic");
      if (v!=NULL) z.pic = v;

      znaki.insert(std::pair<int, zn>(get_type(z.mp), z));   
   }
   return true;
}



//конструктор zn_conv
zn_conv::zn_conv(const std::string & conf_file){

  // читаем конф.файл.
  FILE *file = fopen(conf_file.c_str(), "r");
  if (file == NULL) {
      cout << "Проблемы с чтением файла " << conf_file << endl;
      exit(0);
  }
  YAML::Node &root = YAML::parse(file);
  if (!YAML::error.empty()) {
      cout << "Проблемы с разбором файла " << conf_file << ":" << endl;
      cout << YAML::error;
      cout << endl;
      exit(0);
   }
   if (!load_znaki(root)) {
      cout << "Файл " << conf_file << " содержит неподходящую структуру данных" << endl;
      exit(0);
   }
  default_fig = fig::make_object("2 1 2 2 4 7 10 -1 -1 6.000 0 2 -1 0 0 0");
}



// определить тип mp-объекта (почти тривиальная функция :))
int zn_conv::get_type(const mp::mp_object & o) const{
  return o.Type
     + ((o.Class == "POLYLINE")?line_mask:0)
     + ((o.Class == "POLYGON")?area_mask:0);
}

// определить тип fig-объекта по внешнему виду.
int zn_conv::get_type (const fig::fig_object & o) const {
  if ((o.type!=2) && (o.type!=3) && (o.type!=4)) return 0; // объект неинтересного вида


  for (std::map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){

    int c1 = o.pen_color;
    int c2 = i->second.fig.pen_color;
    // цвет -1 совпадает с цветом 0!
    if (c1 == -1) c1 = 0;
    if (c2 == -1) c2 = 0;

    if (((o.type==2) || (o.type==3)) && (o.size()>1)){
      // должны совпасть глубина и толщина
      if ((o.depth     != i->second.fig.depth) ||
          (o.thickness != i->second.fig.thickness)) continue;
      // для линий толщины не 0 - еще и цвет и тип линии
      if ((o.thickness  != 0 ) &&
          ((c1 != c2) ||
           (o.line_style != i->second.fig.line_style))) continue;

      // заливки
      int af1 = o.area_fill;
      int af2 = i->second.fig.area_fill;
      int fc1 = o.fill_color;
      int fc2 = i->second.fig.fill_color;
      // цвет -1 совпадает с цветом 0!
      if (fc1 == -1) fc1 = 0;
      if (fc2 == -1) fc2 = 0;
      // белая заливка бывает двух видов
      if ((fc1!=7)&&(af1==40)) {fc1=7; af1=20;}
      if ((fc2!=7)&&(af2==40)) {fc2=7; af2=20;}

      // тип заливки должен совпасть
      if (af1 != af2) continue;
      // если заливка непрозрачна, то и цвет заливки должен совпасть
      if ((af1!=-1) && (fc1 != fc2)) continue;

    // если заливка - штриховка, то и pen_color должен совпасть 
      // (даже для линий толщины 0)
      if ((af1>41) && (c1 != c2)) continue;
    
      // проведя все тесты, мы считаем, что наш объект соответствует
      // объекту из znaki!
      return i->first;
    }
    else if (((o.type==2) || (o.type==3)) && (o.size()==1)){ // точки
      // должны совпасть глубина, толщина, цвет, и закругление
      if ((o.depth     == i->second.fig.depth) &&
          (o.thickness == i->second.fig.thickness) &&
          (c1          == c2) &&
          (o.cap_style%2 == i->second.fig.cap_style%2))
        return i->first;
    }
    else if (o.type==4){ //текст
      // должны совпасть глубина, цвет, и шрифт
      if ((o.depth     == i->second.fig.depth) &&
          (c1          == c2) &&
          (o.font      == i->second.fig.font))
        return i->first;
    }
  }
  return 0;
}

// Преобразовать mp-объект в fig-объект
// Если тип 0, то он определяется функцией get_type по объекту
fig::fig_object zn_conv::mp2fig(const mp::mp_object & mp, convs::map2pt & cnv, int type){
  if (type ==0) type = get_type(mp);

  fig::fig_object ret = default_fig;
  if (znaki.find(type) != znaki.end()) ret = znaki.find(type)->second.fig;
  else {
    if (unknown_types.count(type) == 0){
      std::cerr << "mp2fig: unknown type: 0x" << std::setbase(16) << type << std::setbase(10) << "\n";
      unknown_types.insert(type);
    }
  }

  if (ret.type == 4){
    ret.text = mp.Label;
    ret.comment.push_back("");
  } else {
    ret.comment.push_back(mp.Label);
  }
  ret.comment.insert(ret.comment.end(), mp.Comment.begin(), mp.Comment.end());

  g_line pts = cnv.line_bck(mp);
  for (int i=0; i<pts.size(); i++) ret.push_back(pts[i]);
  // замкнутая линия
  if ((mp.Class == "POLYLINE") && (ret.size()>1) && (ret[0]==ret[ret.size()-1])){
    ret.resize(ret.size()-1);
    ret.close();
  }
  // стрелки
  if      (mp.DirIndicator==1){ret.forward_arrow=1;}
  else if (mp.DirIndicator==2){ret.backward_arrow=1;}
  return ret;
}

// преобразовать fig-объект в mp-объект
// Если тип 0, то он определяется функцией get_type по объекту
mp::mp_object zn_conv::fig2mp(const fig::fig_object & fig, convs::map2pt & cnv, int type){
  if (type ==0) type = get_type(fig);

  mp::mp_object mp = default_mp;
  if (znaki.find(type) != znaki.end()) mp = znaki.find(type)->second.mp;
  else {
    if (unknown_types.count(type) == 0){
      std::cerr << "fig2mp: unknown type: 0x" << std::setbase(16) << type << std::setbase(10) << "\n";
      unknown_types.insert(type);
    }
  }

  if (fig.comment.size()>1){
    if (fig.type == 4){
      mp.Label = fig.text;
      mp.Comment.insert(mp.Comment.begin(), fig.comment.begin()+1, fig.comment.end());
    } else if (fig.comment.size()>0){
      mp.Label = fig.comment[0];
      mp.Comment.insert(mp.Comment.begin(), fig.comment.begin()+1, fig.comment.end());
    }
  }

  g_line pts = cnv.line_frw(fig);
  for (int i=0; i<pts.size(); i++) mp.push_back(pts[i]);

  // если у нас замкнутая линия - добавим в mp еще одну точку:
  if ((mp.Class == "POLYLINE") && (fig.is_closed()) && (fig.size()>0) && (fig[0]!=fig[fig.size()-1]))
    mp.push_back(mp[0]);

  // если есть стрелка вперед -- установить DirIndicator=1
  // если есть стрелка назад -- установить  DirIndicator=2
  // если 0 или 2 стрелки - DirIndicator=0
  if ((fig.forward_arrow==1)&&(fig.backward_arrow==0)) mp.DirIndicator=1;
  if ((fig.forward_arrow==0)&&(fig.backward_arrow==1)) mp.DirIndicator=2;

  return mp;
}

// Поменять параметры в соответствии с типом.
// Если тип 0, то он определяется функцией get_type по объекту
void zn_conv::fig_update(fig::fig_object & fig, int type){
  if (type ==0) type = get_type(fig);

  fig::fig_object tmp = default_fig;
  if (znaki.find(type) != znaki.end()) tmp = znaki.find(type)->second.fig;
  else {
    if (unknown_types.count(type) == 0){
      std::cerr << "fig_update: unknown type: 0x" << std::setbase(16) << type << std::setbase(10) << "\n";
      unknown_types.insert(type);
    }
  }

  // копируем разные параметры:
  fig.line_style = tmp.line_style;
  fig.thickness  = tmp.thickness;
  fig.pen_color  = tmp.pen_color;
  fig.fill_color = tmp.fill_color;
  fig.depth      = tmp.depth;
  fig.pen_style  = tmp.pen_style;
  fig.area_fill  = tmp.area_fill;
  fig.style_val  = tmp.style_val;
  fig.cap_style  = tmp.cap_style;
  fig.join_style = tmp.join_style;
  fig.font       = tmp.font;
  fig.font_size  = tmp.font_size;
  fig.font_flags = tmp.font_flags;
}

// Поменять параметры подписи в соответствии с типом
// (шрифт, размер, цвет)
// Если тип 0 - ничего не менять
void zn_conv::label_update(fig::fig_object & fig, int type) const{

  std::map<int, zn>::const_iterator z = znaki.find(type);
  if (z != znaki.end()){
    fig.pen_color = z->second.txt.pen_color;
    fig.font      = z->second.txt.font;
    fig.font_size = z->second.txt.font_size;
    fig.depth     = z->second.txt.depth;
  }
}

// Создать картинку к объекту в соответствии с типом.
std::list<fig::fig_object> zn_conv::make_pic(const fig::fig_object & fig, int type){

  std::list<fig::fig_object> ret;
  if (fig.size()==0) return ret;
  ret.push_back(fig);

  if (type ==0) type = get_type(fig);
  std::map<int, zn>::const_iterator z = znaki.find(type);
  if (z == znaki.end()){
    if (unknown_types.count(type) == 0){
      std::cerr << "make_pic: unknown type: 0x" << std::setbase(16) << type << std::setbase(10) << "\n";
      unknown_types.insert(type);
    }
    return ret;
  }

  if (z->second.pic=="") return ret; // нет картинки

  fig::fig_world PIC;
  if (!fig::read(z->second.pic.c_str(), PIC)) return ret; // нет картинки

  for (fig::fig_world::iterator i = PIC.begin(); i!=PIC.end(); i++){
    (*i) += fig[0];
    if (is_map_depth(*i)){
      cerr << "warning: picture in " << z->second.pic 
           << " has objects with wrong depth!\n";
    }
    i->comment.resize(2); 
    i->comment[1] = "[skip]";
    ret.push_back(*i);
  }
  fig::fig_make_comp(ret);

  // скопируем комментарии из первого объекта в составной объект.
  if (ret.size()>1)
    ret.begin()->comment.insert(ret.begin()->comment.begin(), fig.comment.begin(), fig.comment.end());

  return ret;
}

// Создать подписи к объекту.
std::list<fig::fig_object> zn_conv::make_labels(const fig::fig_object & fig, int type){

  std::list<fig::fig_object> ret;
  if (fig.size() == 0) return ret;                   // странный объект

  if (type ==0) type = get_type(fig);

  map<int, zn>::const_iterator z = znaki.find(type);
  if (z==znaki.end()){
    if (unknown_types.count(type) == 0){
      std::cerr << "make_labels: unknown type: 0x" << std::setbase(16) << type << std::setbase(10) << "\n";
      unknown_types.insert(type);
    }
    return ret;
  }

  if (!z->second.istxt) return ret;            // подпись не нужна
  if ((fig.comment.size()==0)||
      (fig.comment[0].size()==0)) return ret;     // нечего писать!
  // заготовка для подписи
  fig::fig_object txt = z->second.txt;

  if (is_map_depth(txt)){
    std::cerr << "Error: label depth " << txt.depth << " is in map object depth range!";
    return ret;
  }

  int txt_dist = 7 * (fig.thickness+2); // fig units

  // определим координаты и наклон подписи
  Point<int> p = fig[0];
  if      (txt.sub_type == 0 ) p += Point<int>(1,-1)*txt_dist;
  else if (txt.sub_type == 1 ) p += Point<int>(0,-2)*txt_dist;
  else if (txt.sub_type == 2 ) p += Point<int>(-1,-1)*txt_dist;

  if (fig.size()>=2){

    if ((type >= line_mask) && (type < area_mask) && (txt.sub_type == 1)){ // линия с центрированным текстом
      // ставится в середину линии
      p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
      Point<int> p1 = fig[fig.size()/2-1] - fig[fig.size()/2];

      if ((p1.x == 0) && (p1.y == 0)) p1.x = 1;

      Point<double> v = pnorm(p1);
      if (v.x<0) v=-1.0*v;
      txt.angle = atan2(-v.y, v.x);

      p-= Point<int>(int(-v.y*txt_dist), int(v.x*txt_dist));

    }
    else { // другие случаи 
      if (txt.sub_type == 0 ) { // left just.text
        // ищем точку с максимальным x-y
        p = fig[0];
        int max = p.x-p.y;
        for (int i = 0; i<fig.size(); i++){
          if (fig[i].x-fig[i].y > max) {
            max = fig[i].x-fig[i].y;
            p = fig[i];
          }
        }
        p+=Point<int>(1,-1)*txt_dist;
      } else if (txt.sub_type == 2 ) { // right just.text
        // ищем точку с минимальным x+y
        p = fig[0];
        int min = p.x+p.y;
        for (int i = 0; i<fig.size(); i++){
          if (fig[i].x+fig[i].y < min) {
            min = fig[i].x+fig[i].y;
            p = fig[i];
          }
        }
        p+=Point<int>(-1,-1)*txt_dist;
      } else if (txt.sub_type == 1 ) { // centered text
        // ищем середину объекта
        Point<int> pmin = fig[0];
        Point<int> pmax = fig[0];
        for (int i = 0; i<fig.size(); i++){
          if (pmin.x > fig[i].x) pmin.x = fig[i].x;
          if (pmin.y > fig[i].y) pmin.y = fig[i].y;
          if (pmax.x < fig[i].x) pmax.x = fig[i].x;
          if (pmax.y < fig[i].y) pmax.y = fig[i].y;
        }
        p=(pmin+pmax)/2;
      }
    }
  }
  txt.text = fig.comment[0];
  txt.push_back(p);
  ret.push_back(txt);
  return(ret);
}
} // namespace

