// now we use libyaml
#include <yaml.h>

#include <string>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#include "zn.h"

#include "../lib2d/point_utils.h"
#include "../lib2d/line_utils.h"

#include <boost/lexical_cast.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

namespace zn{

using namespace std;

// Depth region in wich all crtographic objects live.
// Its boundaries are minimal and maximum depth of objects in config file
bool zn_conv::is_map_depth(const fig::fig_object & o){
  return ((o.type!=6) && (o.type!=-6) && (o.depth>=min_depth) && (o.depth<=max_depth));
}

// States for our YAML parser
typedef enum {
  N,
  KEY,
  VAL
} reading_state_t;

// zn_conv constructor. Read config file,
// build structure with fig and mp objects
zn_conv::zn_conv(const string & conf_file){

  yaml_parser_t parser;
  yaml_event_t event;

  vector<int> history;
  bool first;

  string key, val;
  zn z;

  reading_state_t state=N;

  min_depth=999;
  max_depth=0;
  znaki.clear();

  // читаем конф.файл.
  FILE *file = fopen(conf_file.c_str(), "r");

  if (!file) {
    cerr << "Error while reading " << conf_file << ": "
         << "can't read YAML file " << conf_file << endl;
    exit(1);
  }

  if (!yaml_parser_initialize(&parser)){
    cerr << "Error while reading " << conf_file << ": "
         << "can't initialize YAML parser\n";
    exit(1);
  }

  yaml_parser_set_input_file(&parser, file);

  do {

    if (!yaml_parser_parse(&parser, &event)) {
      cerr << "Error while reading " << conf_file << ": "
           << parser.problem << " at line "
           << parser.problem_mark.line+1 << " \n";
      exit(1);
    }

    if ((event.type == YAML_STREAM_START_EVENT) ||
        (event.type == YAML_DOCUMENT_START_EVENT) ||
        (event.type == YAML_SEQUENCE_START_EVENT) ||
        (event.type == YAML_MAPPING_START_EVENT) ){
      history.push_back(event.type);
      first=true;
    }

    if ((event.type == YAML_STREAM_END_EVENT) ||
        (event.type == YAML_DOCUMENT_END_EVENT) ||
        (event.type == YAML_SEQUENCE_END_EVENT) ||
        (event.type == YAML_MAPPING_END_EVENT) ){
      if ((history.size()<=0) || (history[history.size()-1] != event.type-1)) {
        cerr << "Error while reading " << conf_file << ": "
             << "unmatched stop event at line " << event.start_mark.line+1 << " \n";
        exit(1);
      }
      history.pop_back();
    }

    if (event.type != YAML_SCALAR_EVENT) state=N;

    if (event.type == YAML_MAPPING_START_EVENT){
      z=zn();
      state=KEY;
    }

    if (event.type == YAML_MAPPING_END_EVENT){
      znaki.insert(pair<int, zn>(get_type(z.mp), z));
      state=N;
    }


    if ( (event.type == YAML_SCALAR_EVENT) && (state==KEY)){
      state=VAL;
      key.clear();
      key.insert(0, (const char *)event.data.scalar.value, event.data.scalar.length);
      continue;
    }

    if ( (event.type == YAML_SCALAR_EVENT) && (state==VAL)){
      state=KEY;
      val.clear();
      val.insert(0, (const char *)event.data.scalar.value, event.data.scalar.length);

      if (key=="name"){
        z.name = val; 
        continue;
      }

      if (key=="mp"){
        z.mp = mp::make_object(val);
        continue;
      }

      if (key=="fig"){
        z.fig = fig::make_object(val);
        if (min_depth>z.fig.depth) min_depth=z.fig.depth;
        if (max_depth<z.fig.depth) max_depth=z.fig.depth;
        continue;
      }

      if (key=="desc"){
        z.desc = val;
        continue;
      }

      if (key=="txt"){
        z.istxt=true;
        z.txt = fig::make_object(val);
        if (z.txt.type!=4){
          cerr << "Error while reading " << conf_file << ": "
               << "bad txt object in "<< z.name <<"\n";
          exit(1);
        }
        continue;
      }

      if (key=="pic"){
        z.pic = val;
        if (z.pic!=""){
          struct stat st_buf;
          string g_pd="/usr/share/mapsoft/pics/";
          string l_pd="./pics/";
          if (stat((g_pd+z.pic).c_str(), &st_buf) == 0)
            z.pic=g_pd+z.pic;
          else if (stat((l_pd+z.pic).c_str(), &st_buf) == 0)
            z.pic=l_pd+z.pic;
          else {
            cerr << "Error while reading " << conf_file << ": "
                 << "can't find zn picture " << z.pic
                 << " in " << g_pd << " or " << l_pd << "\n";
            exit(1);
          }
        }
        continue;
      }

      cerr << "Warning while reading " << conf_file << ": "
           << "unknown field" << key << "\n";
      continue;
    }

    yaml_event_delete(&event);

  } while (history.size()>0);

  yaml_parser_delete(&parser);
  assert(!fclose(file));

  default_fig = fig::make_object("2 1 2 2 4 7 10 -1 -1 6.000 0 2 -1 0 0 0");
  default_mp = mp::make_object("POLYLINE 0x0 0 1");
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


  for (map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){

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
list<fig::fig_object> zn_conv::mp2fig(const mp::mp_object & mp, convs::map2pt & cnv, int type){

  if (type ==0) type = get_type(mp);

  list<fig::fig_object> ret;
  fig::fig_object ret_o = default_fig;

  if (znaki.find(type) != znaki.end()) ret_o = znaki.find(type)->second.fig;
  else {
    if (unknown_types.count(type) == 0){
      cerr << "mp2fig: unknown type: 0x" << setbase(16) << type << setbase(10) << "\n";
      unknown_types.insert(type);
    }
  }

  if (ret_o.type == 4){
    ret_o.text = mp.Label;
    ret_o.comment.push_back("");
  } else {
    ret_o.comment.push_back(mp.Label);
  }
  ret_o.comment.insert(ret_o.comment.end(), mp.Comment.begin(), mp.Comment.end());

  // convert points
  if (mp.Class == "POLYGON"){
    ret_o.set_points(cnv.line_bck(join_polygons(mp)));
    ret.push_back(ret_o);
  }
  else {
    for (MultiLine<double>::const_iterator i=mp.begin(); i!=mp.end(); i++){
      ret_o.clear();
//      g_line pts = cnv.line_bck(*i);
//      for (int i=0; i<pts.size(); i++) ret_o.push_back(pts[i]);
      ret_o.set_points(cnv.line_bck(*i));
      // замкнутая линия
      if ((mp.Class == "POLYLINE") && (ret_o.size()>1) && (ret_o[0]==ret_o[ret_o.size()-1])){
        ret_o.resize(ret_o.size()-1);
        ret_o.close();
      }
      // стрелки
      int dir = mp.Opts.get("DirIndicator",0);
      if      (dir==1){ret_o.forward_arrow=1; ret_o.backward_arrow=0;}
      else if (dir==2){ret_o.backward_arrow=1; ret_o.forward_arrow=0;}
      else    {ret_o.backward_arrow=0; ret_o.forward_arrow=0;}
      ret.push_back(ret_o);
    }
  }
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
      cerr << "fig2mp: unknown type: 0x" << setbase(16) << type << setbase(10) << "\n";
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

  // если у нас замкнутая линия - добавим в mp еще одну точку:
  if ((mp.Class == "POLYLINE") &&
      (fig.is_closed()) &&
      (fig.size()>0) &&
      (fig[0]!=fig[fig.size()-1]))  pts.push_back(pts[0]);
  mp.push_back(pts);

  // если есть стрелка вперед -- установить DirIndicator=1
  // если есть стрелка назад -- установить  DirIndicator=2
  if ((fig.forward_arrow==1)&&(fig.backward_arrow==0)) mp.Opts["DirIndicator"]="1";
  if ((fig.forward_arrow==0)&&(fig.backward_arrow==1)) mp.Opts["DirIndicator"]="2";

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
      cerr << "fig_update: unknown type: 0x" << setbase(16) << type << setbase(10) << "\n";
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

  map<int, zn>::const_iterator z = znaki.find(type);
  if (z != znaki.end()){
    fig.pen_color = z->second.txt.pen_color;
    fig.font      = z->second.txt.font;
    fig.font_size = z->second.txt.font_size;
    fig.depth     = z->second.txt.depth;
  }
}

// Создать картинку к объекту в соответствии с типом.
list<fig::fig_object> zn_conv::make_pic(const fig::fig_object & fig, int type){

  list<fig::fig_object> ret;
  if (fig.size()==0) return ret;
  ret.push_back(fig);

  if (type ==0) type = get_type(fig);
  map<int, zn>::const_iterator z = znaki.find(type);
  if (z == znaki.end()){
    if (unknown_types.count(type) == 0){
      cerr << "make_pic: unknown type: 0x" << setbase(16) << type << setbase(10) << "\n";
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
list<fig::fig_object> zn_conv::make_labels(const fig::fig_object & fig, int type){

  list<fig::fig_object> ret;
  if (fig.size() == 0) return ret;                   // странный объект

  if (type ==0) type = get_type(fig);

  map<int, zn>::const_iterator z = znaki.find(type);
  if (z==znaki.end()){
    if (unknown_types.count(type) == 0){
      cerr << "make_labels: unknown type: 0x" << std::setbase(16) << type << setbase(10) << "\n";
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
    cerr << "Error: label depth " << txt.depth << " is in map object depth range!";
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

