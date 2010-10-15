#ifndef ZN_LISTS_H
#define ZN_LISTS_H

#include <string>
#include <list>
#include <map>
#include "fig/fig.h"

#include "zn.h"

// составление разных списков знаков

namespace zn{

  // сделать пример объекта
  fig::fig_object make_sample(const std::map<int, zn>::const_iterator &i, int grid=100, int dg=30);

  // список всех знаков в формате fig
  //  - cmap - делать ли картинку для цветовой карты: на белом фоне и без описаний
  //  - grid - шаг сетки для объектов
  fig::fig_world make_legend(zn_conv & z, bool cmap=false, int grid = 100, int dg=30);

  // текстовый список всех знаков
  std::string make_text(zn_conv & z);

} // namespace

#endif

