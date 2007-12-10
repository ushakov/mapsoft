#ifndef ZN_LISTS_H
#define ZN_LISTS_H

#include <string>
#include <list>
#include <map>
#include "../geo_io/fig.h"

#include "zn.h"

// составление разных списков знаков

namespace zn{

  // список всех знаков в формате fig
  fig::fig_world make_legend(const zn_conv & z, int grid = 100, int dg=30);
  // текстовый список всех знаков
  std::string make_text(const zn_conv & z);

} // namespace

#endif

