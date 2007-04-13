#ifndef GEOFIG_H
#define GEOFIG_H

#include <vector>

#include "fig.h"
#include "geo_data.h"


namespace fig {

  // Прочитать привязку из fig_world
  g_map get_map(const fig_world & w);
  // Убрать привязку из fig_world
  void rem_map(fig_world & w);
  // Добавить привязку в fig_world
  void set_map(fig_world & w, const g_map & m);

  // Извлечь треки и точки и поместить их в geo_data
  void get_wpts(const fig_world & w, geo_data & d);
  void get_trks(const fig_world & w, geo_data & d);

  // Добавить точки и треки в fig_world в соотв. с привязкой
  void set_wpts(fig_world & w, const g_map & m, const geo_data & d);
  void set_trks(fig_world & w, const g_map & m, const geo_data & d);
}
#endif
