#ifndef LEGEND_H
#define LEGEND_H

#include <string>
#include <map>
#include "../../core/libmp/mp_data.h"
#include "../../core/libfig/fig_data.h"

namespace vmap{

struct leg_el{
  std::string name, desc;  // название, подробное описание
  mp::mp_object mp;        // заготовка mp-объекта
  fig::fig_object fig;     // заготовка fig-объекта
  fig::fig_object txt;     // заготовка fig-объекта подписи
  std::string pic;         // картинка для точечного объекта (м.б. пустой)
  bool istxt;              // делать ли подпись?

  leg_el(){istxt=false;}
};

struct legend: std::map<std::string, leg_el>{
  int min_fig_depth, max_fig_depth;
  fig::fig_object default_fig;
  mp::mp_object default_mp;

  // Constructor. Read all information from yaml-file
  legend(const std::string & style);

  // Region of FIG depths in wich all cartographic objects live.
  // Its boundaries are minimal and maximum depth of objects in config file
  bool is_map_depth(const fig::fig_object & o) const;

  // Find types for different formats
  std::string get_type (const fig::fig_object & o) const;
  std::string get_type (const mp::mp_object & o) const;

};

} // namespace
#endif
