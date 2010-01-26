#include <yaml.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#include "legend.h"

#include "../../core/lib2d/point_utils.h"
#include "../../core/libfig/fig.h"
#include "../../core/libmp/mp.h"

#include <boost/lexical_cast.hpp>

namespace vmap{
using namespace std;

// States for our YAML parser
typedef enum {
  N,
  KEY,
  VAL
} reading_state_t;

// zn_conv constructor. Read config file,
// build structure with fig and mp objects
legend::legend(const string & style){

  yaml_parser_t parser;
  yaml_event_t event;

  vector<int> history;
  bool first;
  string type;

  string key, val;
  leg_el le;

  reading_state_t state=N;

  default_fig = fig::make_object("2 1 2 2 4 7 10 -1 -1 6.000 0 2 -1 0 0 0");
  default_mp = mp::make_object("POLYLINE 0x0 0 1");

  min_fig_depth=999;
  max_fig_depth=0;

  clear();
  string conf_file;

  // looking for a style
  string gf="/usr/share/mapsoft/"+style+".sty";
  string lf="./"+style+".sty";
  struct stat st_buf;
  if (stat(lf.c_str(), &st_buf) == 0)
    conf_file=lf;
  else if (stat(gf.c_str(), &st_buf) == 0)
    conf_file=gf;
  else {
     cerr << "Can't find style " << style << " in "
          << lf << " or " << gf << "\n";
     exit(1);
  }

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
      le=leg_el();
      state=KEY;
    }

    if (event.type == YAML_MAPPING_END_EVENT){
      (*this)[type]=le;
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

      if (key=="type"){
        type = val;
        continue;
      }

      if (key=="name"){
        le.name = val;
        continue;
      }

      if (key=="mp"){
        le.mp.push_back(mp::make_object(val));
        continue;
      }

      if (key=="fig"){
        fig::fig_object o=fig::make_object(val);
        le.fig.push_back(o);
        if (min_fig_depth>o.depth) min_fig_depth=o.depth;
        if (max_fig_depth<o.depth) max_fig_depth=o.depth;
        continue;
      }

      if (key=="desc"){
        le.desc = val;
        continue;
      }

      if (key=="txt"){
        le.istxt=true;
        le.txt = fig::make_object(val);
        if (le.txt.type!=4){
          cerr << "Error while reading " << conf_file << ": "
               << "bad txt object in "<< le.name <<"\n";
          exit(1);
        }
        continue;
      }

      if (key=="pic"){
        le.pic = val;
        if (le.pic!=""){
          string g_pd="/usr/share/mapsoft/pics/";
          string l_pd="./pics/";
          if (stat((g_pd+le.pic).c_str(), &st_buf) == 0)
            le.pic=g_pd+le.pic;
          else if (stat((l_pd+le.pic).c_str(), &st_buf) == 0)
            le.pic=l_pd+le.pic;
          else {
            cerr << "Error while reading " << conf_file << ": "
                 << "can't find legend picture " << le.pic
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

}

/**************************************************************************/
// Region of FIG depths in wich all cartographic objects live.
// Its boundaries are minimal and maximum depth of objects in config file
bool legend::is_map_depth(const fig::fig_object & o) const{
  return ((o.type!=6) && (o.type!=-6) && 
          (o.depth>=min_fig_depth) && (o.depth<=max_fig_depth));
}

/**************************************************************************/
// find type for mp object
string legend::get_type(const mp::mp_object & o) const{
  for (map<string, leg_el>::const_iterator i = begin(); i!=end(); i++){
    for (vector<mp::mp_object>::const_iterator mp=i->second.mp.begin();
              mp!=i->second.mp.end(); mp++){
      if ((o.Type==mp->Type) &&
          (o.Class==mp->Class)) return i->first;
    }
  }
  return "";
}

/**************************************************************************/
// find type for fig object
string legend::get_type (const fig::fig_object & o) const {

  // we are interested in lines only
  if ((o.type!=2) && (o.type!=3)) return "";

  for (map<string, leg_el>::const_iterator i = begin(); i!=end(); i++){
    for (vector<fig::fig_object>::const_iterator fig=i->second.fig.begin();
              fig!=i->second.fig.end(); fig++){

      int c1 = o.pen_color;
      int c2 = fig->pen_color;

      // there is no difference between -1 and 0 colors
      if (c1 == -1) c1 = 0;
      if (c2 == -1) c2 = 0;

      if (o.size()>1){ // lines
        // compare depth and thickness
        if ((o.depth     != fig->depth) ||
            (o.thickness != fig->thickness)) continue;
        // if thickness != 0 then compare color and line type
        if ((o.thickness  != 0 ) &&
            ((c1 != c2) ||
             (o.line_style != fig->line_style))) continue;

        int af1 = o.area_fill;
        int af2 = fig->area_fill;
        int fc1 = o.fill_color;
        int fc2 = fig->fill_color;
        // there is no difference between -1 and 0 colors
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
      else if (o.size()==1){ // points
        // должны совпасть глубина, толщина, цвет, и закругление
        if ((o.depth     == fig->depth) &&
            (o.thickness == fig->thickness) &&
            (c1          == c2) &&
            (o.cap_style%2 == fig->cap_style%2))
          return i->first;
      }
    }
  }
  return "";
}

/**************************************************************************/
list<fig::fig_object> legend::make_pic(
      const fig::fig_object & fig, string type){

  list<fig::fig_object> ret;
  if (fig.size()==0) return ret;
  ret.push_back(fig);

  if (type == "") type = get_type(fig);
  const_iterator z=find(type);
  if (z==end()){
    if (unknown_types.count(type) == 0){
      cerr << "make_pic: unknown type: \"" << type << "\"\n";
      unknown_types.insert(type);
    }
    return ret;
  }

  if (z->second.pic=="") return ret; // no picture
  fig::fig_world PIC;
  if (!fig::read(z->second.pic.c_str(), PIC)){
    cerr << "Warning: can't read picture: \"" << z->second.pic << "\"\n";
    return ret;
  }

  for (fig::fig_world::iterator i = PIC.begin(); i!=PIC.end(); i++){
    (*i) += fig[0];
    if (is_map_depth(*i)){
      cerr << "Warning: picture in \"" << z->second.pic
           << "\" has objects with wrong depth!\n";
    }
    i->comment.resize(2);
    i->comment[1] = "[skip]"; // BC!!!
    i->opts["Skip"]="yes";
    ret.push_back(*i);
  }
  fig::fig_make_comp(ret);

  // copy comments to the compound
  if (ret.size()>1)
    ret.begin()->comment.insert(
      ret.begin()->comment.begin(), fig.comment.begin(), fig.comment.end());

  return ret;
}

/**************************************************************************/
list<fig::fig_object> legend::make_labels(
      const fig::fig_object & fig, string type){

  list<fig::fig_object> ret;
  if (fig.size() == 0) return ret;

  if (type == "") type = get_type(fig);

  const_iterator z = find(type);
  if (z==end()){
    if (unknown_types.count(type) == 0){
      cerr << "make_pic: unknown type: \"" << type << "\"\n";
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
    cerr << "Error: label depth " << txt.depth << " is in object depth range!";
    return ret;
  }

  int txt_dist = 7 * (fig.thickness+2); // fig units

  // определим координаты и наклон подписи
  iPoint p = fig[0];
  if      (txt.sub_type == 0 ) p += iPoint(1,-1)*txt_dist;
  else if (txt.sub_type == 1 ) p += iPoint(0,-2)*txt_dist;
  else if (txt.sub_type == 2 ) p += iPoint(-1,-1)*txt_dist;

  if (fig.size()>=2){
    // line with centered text: put rotated text at the middle of the line
    if ((fig.area_fill==-1) && (txt.sub_type == 1)){
      p = (fig[fig.size()/2-1] + fig[fig.size()/2]) / 2;
      iPoint p1 = fig[fig.size()/2-1] - fig[fig.size()/2];

      if ((p1.x == 0) && (p1.y == 0)) p1.x = 1;

      dPoint v = pnorm(p1);
      if (v.x<0) v=-1.0*v;
      txt.angle = atan2(-v.y, v.x);

      p-= iPoint(int(-v.y*txt_dist), int(v.x*txt_dist));

    }
    else {
      if (txt.sub_type == 0 ) { // left just.text
        // find max(x-y)
        p = fig[0];
        int max = p.x-p.y;
        for (int i = 0; i<fig.size(); i++){
          if (fig[i].x-fig[i].y > max) {
            max = fig[i].x-fig[i].y;
            p = fig[i];
          }
        }
        p+=iPoint(1,-1)*txt_dist;
      } else if (txt.sub_type == 2 ) { // right just.text
        // find min(x-y)
        p = fig[0];
        int min = p.x+p.y;
        for (int i = 0; i<fig.size(); i++){
          if (fig[i].x+fig[i].y < min) {
            min = fig[i].x+fig[i].y;
            p = fig[i];
          }
        }
        p+=iPoint(-1,-1)*txt_dist;
      } else if (txt.sub_type == 1 ) { // centered text
        // find center of the object
        iPoint pmin = fig[0];
        iPoint pmax = fig[0];
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

/**************************************************************************/
void legend::fig_update(fig::fig_object & fig, string type){
  if (type == "") type = get_type(fig);

  fig::fig_object tmp = default_fig;

  if ((count(type) > 0) &&
      (find(type)->second.fig.size()>0)){
    tmp = find(type)->second.fig[0];
  }
  else {
    if (unknown_types.count(type) == 0){
      cerr << "fig_update: unknown type \"" << type << "\"\n";
      unknown_types.insert(type);
    }
  }

  // copy parameters:
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

/**************************************************************************/
void legend::label_update(fig::fig_object & fig, string type){
  const_iterator z = find(type);
  if (z != end()){
    fig.pen_color = z->second.txt.pen_color;
    fig.font      = z->second.txt.font;
    fig.font_size = z->second.txt.font_size;
    fig.depth     = z->second.txt.depth;
  }
}

} // namespace

