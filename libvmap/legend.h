#ifndef LEGEND_H
#define LEGEND_H

#include <string>
#include <map>
#include "../libmp/mp_data.h"
#include "../libfig/fig_data.h"

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

struct legend: std::map<int, leg_el>{
  int min_fig_depth, max_fig_depth;
  // Constructor. Read all information from yaml-file
  legend(const std::string & conf_file);

  // Find types for different formats
  int get_type (const fig::fig_object & o) const;
  int get_type (const mp::mp_object & o) const;

};

} // namespace
#endif
