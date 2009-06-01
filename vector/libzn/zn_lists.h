#ifndef ZN_LISTS_H
#define ZN_LISTS_H

#include <string>
#include <list>
#include <map>
#include "../../core/libfig/fig.h"

#include "zn.h"

// составление разных списков знаков

namespace zn{

  // сделать пример объекта
  fig::fig_object make_sample(const std::map<int, zn>::const_iterator &i, int grid=100, int dg=30);

  // список всех знаков в формате fig
  fig::fig_world make_legend(zn_conv & z, int grid = 100, int dg=30);
  // текстовый список всех знаков
  std::string make_text(zn_conv & z);

} // namespace

#endif

