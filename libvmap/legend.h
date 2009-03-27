#ifndef LEGEND_H
#define LEGEND_H

#include <string>
#include <map>
#include "../libmp/mp_data.h"
#include "../libfig/fig_data.h"

namespace vmap{

struct leg_el{
  std::string name, desc;  // ��������, ��������� ��������
  mp::mp_object mp;        // ��������� mp-�������
  fig::fig_object fig;     // ��������� fig-�������
  fig::fig_object txt;     // ��������� fig-������� �������
  std::string pic;         // �������� ��� ��������� ������� (�.�. ������)
  bool istxt;              // ������ �� �������?

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
