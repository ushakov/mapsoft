#ifndef LEGEND_H
#define LEGEND_H

#include <string>
#include <map>
#include "../../core/libmp/mp_data.h"
#include "../../core/libfig/fig_data.h"

namespace vmap{

struct leg_el{
  std::string name, desc;
  mp::mp_object mp;        // mp template
  fig::fig_object fig;     // fig template
  fig::fig_object txt;     // fig template for a label
  std::string pic;         // path to picture (may be empty)
  bool istxt;              // make label or not

  leg_el(){istxt=false;}
};

struct legend: std::map<std::string, leg_el>{
  int min_fig_depth, max_fig_depth;
  fig::fig_object default_fig;
  mp::mp_object default_mp;

  // Constructor. Read all information from yaml-file
  legend(const std::string & style);

  // Is object have a depth between min and max depth
  // of cartographic objects
  bool is_map_depth(const fig::fig_object & o) const;

  // Find types for different formats
  std::string get_type (const fig::fig_object & o) const;
  std::string get_type (const mp::mp_object & o) const;

};

} // namespace
#endif
