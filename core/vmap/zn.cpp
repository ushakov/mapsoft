// now we use libyaml
#include <yaml.h>

#include <string>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#include "zn.h"

#include "2d/point.h"
#include "2d/line_utils.h"
#include "err/err.h"

#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>

namespace zn{

using namespace std;

// copy comment from compound to the first object in it
void
fig_copy_comment(const fig::fig_world::iterator & i,
                      const fig::fig_world::iterator & end){
  if (i->type == 6){ // составной объект
    // копируем первые непустые строки комментария в следующий объект
    // остальное нам не нужно
    fig::fig_world::iterator j=i; j++;

    // пропускаем подписи
    while ((j!=end) &&
      ( ((j->comment.size()>1) && (j->comment[1]=="[skip]")) ||
        (j->opts.get("MapType", std::string()) == "pic") )) j++;

    if ((j!=end) && (i->comment.size()>0)){
      if (j->comment.size()< i->comment.size()) j->comment.resize(i->comment.size());
      for (size_t n=0; n<i->comment.size(); n++) j->comment[n] = i->comment[n];

    }
  }
}
// object need to be skipped (compounds, pics, [skip])
bool
is_to_skip(fig::fig_object o){
  return (o.type==6) || (o.type==-6) ||
         ((o.comment.size()>1) && (o.comment[1]=="[skip]")) ||
         (o.opts.get("MapType", std::string()) == "pic");
}
// convert fig arrow to direction
// arr text dir(vmap, mp)
// --  1    0
// ->  0    1
// <-  2    2
int
fig_arr2dir(const fig::fig_object & f, bool text){
  if ((f.forward_arrow==1)&&(f.backward_arrow==0)) return text?0:1;
  if ((f.forward_arrow==0)&&(f.backward_arrow==1)) return 2;
  return text?1:0;
}
void
fig_dir2arr(fig::fig_object & f, int dir, bool text){
  if (text && (dir<2)) dir=(dir+1)%2;
  if      (dir==1){f.forward_arrow=1; f.backward_arrow=0;}
  else if (dir==2){f.backward_arrow=1; f.forward_arrow=0;}
  else    {f.backward_arrow=0; f.forward_arrow=0;}
}

// Remove compounds and objects with [skip] comment
void
fig_remove_pics(fig::fig_world & F){
  fig::fig_world::iterator i=F.begin();
  while (i!=F.end()){
    if (i->type==6) fig_copy_comment(i, F.end());
    if (is_to_skip(*i)){
      i=F.erase(i);
      continue;
    }
    i++;
  }
}
// Find nearest point in the line
double
dist( const iPoint & p, const iLine & l, iPoint & nearest){
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





// States for our YAML parser
typedef enum {
  N,
  KEY,
  VAL
} reading_state_t;

// zn_conv constructor. Read config file,
// build structure with fig and mp objects
zn_conv::zn_conv(const string & style_): style(style_){

  default_fig = fig::make_object("2 1 2 2 4 7 10 -1 -1 6.000 0 0 -1 0 0 0");
  default_txt = fig::make_object("4 0 4 10 -1 18 8 0.0000 4");
  default_mp = mp::make_object("POLYLINE 0x0 0 1");
  default_ocad = 704001;
  default_ocad_txt = 780000;

  yaml_parser_t parser;
  yaml_event_t event;

  vector<int> history;

  string key, val;
  zn z;

  reading_state_t state=N;

  min_depth=999;
  max_depth=0;
  znaki.clear();

  string conf_file;
  // looking for a style
  string gf="/usr/share/mapsoft/"+style+".cnf";
  string lf="./"+style+".cnf";
  struct stat st_buf;
  if (stat(lf.c_str(), &st_buf) == 0)
    conf_file=lf;
  else if (stat(gf.c_str(), &st_buf) == 0)
    conf_file=gf;
  else {
     throw Err() << "Can't find style " << style << " in "
                 << lf << " or " << gf;
  }

  // читаем конф.файл.
  FILE *file = fopen(conf_file.c_str(), "r");

  if (!file) {
    throw Err() << "Error while reading " << conf_file << ": "
                << "can't read YAML file " << conf_file;
  }

  if (!yaml_parser_initialize(&parser)){
    throw Err() << "Error while reading " << conf_file << ": "
                << "can't initialize YAML parser";
  }

  yaml_parser_set_input_file(&parser, file);

  do {

    if (!yaml_parser_parse(&parser, &event)) {
      throw Err() << "Error while reading " << conf_file << ": "
                  << parser.problem << " at line "
                  << parser.problem_mark.line+1;
    }

    if ((event.type == YAML_STREAM_START_EVENT) ||
        (event.type == YAML_DOCUMENT_START_EVENT) ||
        (event.type == YAML_SEQUENCE_START_EVENT) ||
        (event.type == YAML_MAPPING_START_EVENT) ){
      history.push_back(event.type);
    }

    if ((event.type == YAML_STREAM_END_EVENT) ||
        (event.type == YAML_DOCUMENT_END_EVENT) ||
        (event.type == YAML_SEQUENCE_END_EVENT) ||
        (event.type == YAML_MAPPING_END_EVENT) ){
      if ((history.size()<=0) || (history[history.size()-1] != event.type-1)) {
        throw Err() << "Error while reading " << conf_file << ": "
                    << "unmatched stop event at line " << event.start_mark.line+1;
      }
      history.pop_back();
    }

    if (event.type != YAML_SCALAR_EVENT) state=N;

    if (event.type == YAML_MAPPING_START_EVENT){
      z=zn();
      z.fig=default_fig;
      z.mp=default_mp;
      z.ocad=z.ocad_txt=0;
      state=KEY;
    }

    if (event.type == YAML_MAPPING_END_EVENT){
      int type = get_type(z.mp);
      // add label_type
      if (z.txt.type == 4){
        if (z.txt.sub_type==0){ // text on the TR side
          z.label_type=1;
          z.label_dir=0;
        }
        else if (z.txt.sub_type==2){ // text on the TL side
          z.label_type=2;
          z.label_dir=2;
        }
        else if (z.txt.sub_type==1) { //centered text
          z.label_dir=1;
          if (type & line_mask) z.label_type=4; // text along the line
          else z.label_type=3;                  // text on the center
        }
      }
      else{
        z.txt=default_txt;
        z.label_type=0;
        z.label_dir=0;
      }
      znaki.insert(pair<int, zn>(type, z));
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
        z.txt = fig::make_object(val);
        if (z.txt.type!=4){
          throw Err() << "Error while reading " << conf_file << ": "
                      << "bad txt object in "<< z.name;
        }
        continue;
      }

      if (key=="pic"){
        z.pic = val;
        if (z.pic!=""){
          struct stat st_buf;
          string g_pd="/usr/share/mapsoft/pics/";
          string l_pd="./pics/";
          if (stat((l_pd+z.pic+".fig").c_str(), &st_buf) == 0)
            z.pic=l_pd+z.pic;
          else if (stat((g_pd+z.pic+".fig").c_str(), &st_buf) == 0)
            z.pic=g_pd+z.pic;
          else {
            throw Err() << "Error while reading " << conf_file << ": "
                        << "can't find zn picture " << z.pic << ".fig"
                        << " in " << l_pd << " or " << g_pd;
          }
        }
        continue;
      }
      if (key=="pic_type"){
        z.pic_type = val;
        continue;
      }

      if (key=="ocad"){
        z.ocad = atoi(val.c_str());
        continue;
      }

      if (key=="ocad_txt"){
        z.ocad_txt = atoi(val.c_str());
        continue;
      }

      if ((key=="move_to") || (key=="rotate_to")){
        istringstream str(val);
        int i;
        while (str.good()){
          str >> hex >> i;
          if (str.good() || str.eof()) z.move_to.push_back(i);
        }
        z.rotate = (key=="rotate_to");
        continue;
      }

      if (key=="replace_by"){
        istringstream str(val);
        int i;
        str >> hex >> i;
        if (str.good() || str.eof()) z.replace_by = i;
        continue;
      }

      if (key=="curve"){
        z.curve = atoi(val.c_str());
        continue;
      }

      cerr << "Warning while reading " << conf_file << ": "
           << "unknown field: " << key << "\n";
      continue;
    }

    yaml_event_delete(&event);

  } while (history.size()>0);

  yaml_parser_delete(&parser);
  assert(!fclose(file));
}

// определить тип mp-объекта (почти тривиальная функция :))
int
zn_conv::get_type(const mp::mp_object & o) const{
  return o.Type
     + ((o.Class == "POLYLINE")?line_mask:0)
     + ((o.Class == "POLYGON")?area_mask:0);
}

// определить тип fig-объекта по внешнему виду.
int
zn_conv::get_type (const fig::fig_object & o) const {
  if ((o.type!=2) && (o.type!=3) && (o.type!=4)) return 0; // объект неинтересного вида


  for (map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){

    int c1 = o.pen_color;
    int c2 = i->second.fig.pen_color;

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
      // белая заливка бывает двух видов
      if ((fc1!=0xffffff)&&(af1==40)) {fc1=0xffffff; af1=20;}
      if ((fc2!=0xffffff)&&(af2==40)) {fc2=0xffffff; af2=20;}

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

// определить тип по номеру ocad-объекта
int
zn_conv::get_type(const int ocad_type) const{
  for (map<int, zn>::const_iterator i = znaki.begin(); i!=znaki.end(); i++){
    if (i->second.ocad == ocad_type) return (i->
first);
  }
  return 0;
}

map<int, zn>::const_iterator
zn_conv::find_type(int type){
  map<int, zn>::const_iterator ret = znaki.find(type);
  if (ret == znaki.end()){
    if (unknown_types.count(type) == 0){
      cerr << "zn: unknown type: 0x"
           << setbase(16) << type << setbase(10) << "\n";
      unknown_types.insert(type);
    }
  }
  return ret;
}

// Получить заготовку fig-объекта заданного типа
fig::fig_object
zn_conv::get_fig_template(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.fig;
  else return default_fig;
}

// Получить заготовку mp-объекта заданного типа
mp::mp_object
zn_conv::get_mp_template(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.mp;
  else return default_mp;
}

// Получить номер объекта ocad
int
zn_conv::get_ocad_type(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.ocad;
  else return default_ocad;
}

// Получить заготовку fig-подписи заданного типа
fig::fig_object
zn_conv::get_label_template(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.txt;
  else return default_txt;
}

// Получить номер объекта для подписи ocad
int
zn_conv::get_ocad_label_type(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.ocad_txt;
  else return default_ocad_txt;
}

// Получить тип подписи (для несуществующих - 0)
int zn_conv::get_label_type(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.label_type;
  else return 0;
}
// Получить направление подписи (для несуществующих - 0)
int zn_conv::get_label_dir(int type){
  map<int, zn>::const_iterator z = find_type(type);
  if (z != znaki.end()) return z->second.label_dir;
  else return 0;
}

// Преобразовать mp-объект в fig-объект
// Если тип 0, то он определяется функцией get_type по объекту
list<fig::fig_object>
zn_conv::mp2fig(const mp::mp_object & mp, convs::map2pt & cnv, int type){

  if (type ==0) type = get_type(mp);
  list<fig::fig_object> ret;
  fig::fig_object ret_o = get_fig_template(type);

  if (ret_o.type == 4){
    ret_o.text = mp.Label;
    ret_o.comment.push_back("");
  } else {
    ret_o.comment.push_back(mp.Label);
  }
  ret_o.comment.insert(ret_o.comment.end(), mp.Comment.begin(), mp.Comment.end());

  string source=mp.Opts.get<string>("Source");
  if (source!="") ret_o.opts.put("Source", source);

  // convert points
  if (mp.Class == "POLYGON"){
    ret_o.set_points(cnv.line_bck(join_polygons(mp)));
    ret.push_back(ret_o);
  }
  else {
    for (dMultiLine::const_iterator i=mp.begin(); i!=mp.end(); i++){
      ret_o.clear();
      ret_o.set_points(cnv.line_bck(*i));
      // замкнутая линия
      if ((mp.Class == "POLYLINE") && (ret_o.size()>1) && (ret_o[0]==ret_o[ret_o.size()-1])){
        ret_o.resize(ret_o.size()-1);
        ret_o.close();
      }
      // стрелки
      fig_dir2arr(ret_o, mp.Opts.get("DirIndicator",0) );
      ret.push_back(ret_o);
    }
  }
  return ret;
}


// преобразовать fig-объект в mp-объект
// Если тип 0, то он определяется функцией get_type по объекту
mp::mp_object
zn_conv::fig2mp(const fig::fig_object & fig, convs::map2pt & cnv, int type){
  if (type ==0) type = get_type(fig);

  mp::mp_object mp = get_mp_template(type);
  mp.Opts = fig.opts;

  if (fig.type == 4){
    mp.Label = fig.text;
    mp.Comment=fig.comment;
  } else if (fig.comment.size()>0){
    mp.Label = fig.comment[0];
    mp.Comment.insert(mp.Comment.begin(), fig.comment.begin()+1, fig.comment.end());
  }
  string source=fig.opts.get<string>("Source");
  if (source != "") mp.Opts.put("Source", source);

  dLine pts = cnv.line_frw(fig);

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
void
zn_conv::fig_update(fig::fig_object & fig, int type){
  if (type ==0) type = get_type(fig);
  fig::fig_object tmp = get_fig_template(type);

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
void
zn_conv::label_update(fig::fig_object & fig, int type){

  fig::fig_object tmp = get_label_template(type);
  fig.pen_color = tmp.pen_color;
  fig.font      = tmp.font;
  fig.font_size = tmp.font_size;
  fig.depth     = tmp.depth;
}

// Создать картинку к объекту в соответствии с типом.
list<fig::fig_object>
zn_conv::make_pic(const fig::fig_object & fig, int type){

  list<fig::fig_object> ret;
  if (fig.size()==0) return ret;
  ret.push_back(fig);

  if (type ==0) type = get_type(fig);
  map<int, zn>::const_iterator z = find_type(type);
  if (z == znaki.end()) return ret;
  if (z->second.pic=="") return ret; // нет картинки
  if ((type & area_mask) || (type & line_mask)) return ret;

  fig::fig_world PIC;
  if (!fig::read((z->second.pic+".fig").c_str(), PIC)) return ret; // нет картинки

  for (fig::fig_world::iterator i = PIC.begin(); i!=PIC.end(); i++){
    (*i) += fig[0];
//    if (is_map_depth(*i)){
//      cerr << "warning: picture in " << z->second.pic 
//           << " has objects with wrong depth!\n";
//    }
    i->opts["MapType"]="pic";
    ret.push_back(*i);
  }
  fig::fig_make_comp(ret);

  if (fig.opts.exists("Angle")){
    double a = fig.opts.get<double>("Angle", 0);
    fig::fig_rotate(ret, M_PI/180.0*a, fig[0]);
  }

  // скопируем комментарии из первого объекта в составной объект.
  if (ret.size()>1)
    ret.begin()->comment.insert(ret.begin()->comment.begin(), fig.comment.begin(), fig.comment.end());

  return ret;
}

// Создать подписи к объекту.
list<fig::fig_object>
zn_conv::make_labels(const fig::fig_object & fig, int type){

  list<fig::fig_object> ret;
  if (fig.size() == 0) return ret;                   // странный объект

  if (type ==0) type = get_type(fig);

  map<int, zn>::const_iterator z = find_type(type);
  if (z==znaki.end()) return ret;

  int lpos = z->second.label_type;

  if (!lpos) return ret;                      // no label for this object
  if ((fig.comment.size()==0)||
      (fig.comment[0].size()==0)) return ret; // empty label

  fig::fig_object txt = z->second.txt; // label template

  if (is_map_depth(txt)){
    cerr << "Error: label depth " << txt.depth << " is in map object depth range!";
    return ret;
  }

  int txt_dist = 7 * (fig.thickness+2); // fig units

  // определим координаты и наклон подписи
  iPoint p = fig[0];

  if (fig.size()>=2){
    if (lpos==4){ // label along the line
      p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
      iPoint p1 = fig[fig.size()/2-1] - fig[fig.size()/2];

      if ((p1.x == 0) && (p1.y == 0)) p1.x = 1;

      dPoint v = pnorm(p1);
      if (v.x<0) v=-v;
      txt.angle = atan2(-v.y, v.x);

      p-= iPoint(int(-v.y*txt_dist*2), int(v.x*txt_dist*2));
    }
    else if (lpos==1 ) { // left just.text
      // ищем точку с максимальным x-y
      p = fig[0];
      int max = p.x-p.y;
      for (size_t i = 0; i<fig.size(); i++){
        if (fig[i].x-fig[i].y > max) {
          max = fig[i].x-fig[i].y;
          p = fig[i];
        }
      }
      p+=iPoint(1,-1)*txt_dist;
    } else if (lpos==2) { // right just.text
      // ищем точку с минимальным x+y
      p = fig[0];
      int min = p.x+p.y;
      for (size_t i = 0; i<fig.size(); i++){
        if (fig[i].x+fig[i].y < min) {
          min = fig[i].x+fig[i].y;
          p = fig[i];
        }
      }
      p+=iPoint(-1,-1)*txt_dist;
    } else if (lpos=3 ) { // center
      // ищем середину объекта
      iPoint pmin = fig[0];
      iPoint pmax = fig[0];
      for (size_t i = 0; i<fig.size(); i++){
        if (pmin.x > fig[i].x) pmin.x = fig[i].x;
        if (pmin.y > fig[i].y) pmin.y = fig[i].y;
        if (pmax.x < fig[i].x) pmax.x = fig[i].x;
        if (pmax.y < fig[i].y) pmax.y = fig[i].y;
      }
      p=(pmin+pmax)/2;
    }
  }
  else { // one-point object
    if      (lpos == 1 ) p += iPoint(1,-1)*txt_dist;
    else if (lpos == 2 ) p += iPoint(-1,-1)*txt_dist;
    else if (lpos == 3 ) p += iPoint(0, 0)*txt_dist;
    else if (lpos == 4 ) p += iPoint(0,-2)*txt_dist;
  }
  txt.text = fig.comment[0];
  txt.push_back(p);
  ret.push_back(txt);
  return(ret);
}

// Depth region in wich all crtographic objects live.
// Its boundaries are minimal and maximum depth of objects in config file
bool
zn_conv::is_map_depth(const fig::fig_object & o) const{
  return ((o.type!=6) && (o.type!=-6) && (o.depth>=min_depth) && (o.depth<=max_depth));
}

/// functions for updating fig map

int
zn_conv::fig_add_pics(fig::fig_world & F){
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

int
zn_conv::fig_update_labels(fig::fig_world & F){

  double maxd1=1*fig::cm2fig; // for the nearest label with correct text
  double maxd2=1*fig::cm2fig; // for other labels with correct text
  double maxd3=0.2*fig::cm2fig; // for other labels

  fig::fig_world::iterator i;

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
  std::list<std::pair<fig::fig_world::iterator, int> > objs;
  for (i=F.begin(); i!=F.end(); i++){
    if (!is_map_depth(*i) || (i->size() <1)) continue;
    int type=get_type(*i);
    if ((znaki.count(type)<1) || !znaki[type].label_type) continue;
    if ((i->comment.size()>0) && (i->comment[0]!="")) 
      objs.push_back(std::pair<fig::fig_world::iterator, int>(i,0));
  }

  std::list<fig::fig_world::iterator>::iterator l;
  std::list<std::pair<fig::fig_world::iterator, int> >::iterator o;

  // one nearest label with correct text
  for (o=objs.begin(); o!=objs.end(); o++){
    double d0=1e99;
    std::list<fig::fig_world::iterator>::iterator l0;
    iPoint n0;

    for (l=labels.begin(); l!=labels.end(); l++){
      if ((*l)->text != o->first->comment[0]) continue;
      iPoint nearest;
      double d=dist((*l)->opts.get("RefPt", iPoint(0,0)), *(o->first), nearest);
      if ( d < d0){ d0=d; n0=nearest; l0=l;}
    }
    if (d0 < maxd1){
      (*l0)->opts.put("RefPt", n0);
      label_update(**l0, get_type(*(o->first)));
      o->second++;
      labels.erase(l0);
    }
  }

  // labels with correct text
  for (o=objs.begin(); o!=objs.end(); o++){
    l=labels.begin();
    while (l!=labels.end()){
      iPoint nearest;
      if (((*l)->text == o->first->comment[0]) &&
          (dist((*l)->opts.get("RefPt", iPoint(0,0)), *(o->first), nearest) < maxd2)){
        (*l)->opts.put("RefPt", nearest);
        label_update(**l, get_type(*(o->first)));
        o->second++;
        l=labels.erase(l);
        continue;
      }
      l++;
    }
  }

  // labels with changed text
  for (o=objs.begin(); o!=objs.end(); o++){
    l=labels.begin();
    while (l!=labels.end()){
      iPoint nearest;
      if (dist((*l)->opts.get("RefPt", iPoint(0,0)), *(o->first), nearest) < maxd3){
        (*l)->text=o->first->comment[0];
        (*l)->opts.put("RefPt", nearest);
        label_update(**l, get_type(*(o->first)));
        o->second++;
        l=labels.erase(l);
        continue;
      }
      l++;
    }
  }

  // create new labels
  for (o=objs.begin(); o!=objs.end(); o++){
    if (o->second > 0) continue;
    std::list<fig::fig_object> L=make_labels(*(o->first));
    for (std::list<fig::fig_object>::iterator j=L.begin(); j!=L.end(); j++){
      if (j->size() < 1) continue;
      iPoint nearest;
      dist((*j)[0], *(o->first), nearest);
      j->opts["MapType"]="label";
      j->opts.put("RefPt", nearest);
      F.push_back(*j);
    }
  }

  // remove unused labels
  for (l=labels.begin(); l!=labels.end(); l++) F.erase(*l);
  return 0;
}



} // namespace

