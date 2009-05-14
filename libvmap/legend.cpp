#include <yaml.h>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#include "legend.h"

#include "../lib2d/point_utils.h"
#include "../libfig/fig_mask.h"
#include "../libmp/mp_mask.h"

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
legend::legend(const string & conf_file){

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
      le=leg_el();
      state=KEY;
    }

    if (event.type == YAML_MAPPING_END_EVENT){
      insert(pair<std::string, leg_el>(type, le));
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
        le.mp = mp::make_object(val);
        continue;
      }

      if (key=="fig"){
        le.fig = fig::make_object(val);
        if (min_fig_depth>le.fig.depth) min_fig_depth=le.fig.depth;
        if (max_fig_depth<le.fig.depth) max_fig_depth=le.fig.depth;
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
          struct stat st_buf;
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

// Region of FIG depths in wich all cartographic objects live.
// Its boundaries are minimal and maximum depth of objects in config file
bool legend::is_map_depth(const fig::fig_object & o) const{
  return ((o.type!=6) && (o.type!=-6) && 
          (o.depth>=min_fig_depth) && (o.depth<=max_fig_depth));
}


// find type for mp object
string legend::get_type(const mp::mp_object & o) const{
  for (map<string, leg_el>::const_iterator i = begin(); i!=end(); i++){
    if ((o.Type==i->second.mp.Type) &&
        (o.Class==i->second.mp.Class)) return i->first;
  }
  return "";
}

// find type for fig object
string legend::get_type (const fig::fig_object & o) const {
  if ((o.type!=2) && (o.type!=3) && (o.type!=4)) return ""; // объект неинтересного вида


  for (map<string, leg_el>::const_iterator i = begin(); i!=end(); i++){

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
  return "";
}



} // namespace

