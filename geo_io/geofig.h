#ifndef GEOFIG_H
#define GEOFIG_H

#include <vector>

#include "fig.h"
#include "geo_data.h"


namespace fig {

  // Прочитать привязку из fig_world
  g_map get_ref(const fig_world & w);
  // Убрать привязку из fig_world
  void rem_ref(fig_world & w);
  // Добавить привязку в fig_world
  void set_ref(fig_world & w, const g_map & m, const Options & o);

  // Извлечь треки и точки и поместить их в geo_data
  void get_wpts(const fig_world & w, const g_map & m, geo_data & d);
  void get_trks(const fig_world & w, const g_map & m, geo_data & d);
  // Извлечь привязку растровых картинок
  void get_maps(const fig_world & w, const g_map & m, geo_data & d);
  // Добавить точки и треки в fig_world в соотв. с привязкой
  void put_wpts(fig_world & w, const g_map & m, const geo_data & d);
  void put_trks(fig_world & w, const g_map & m, const geo_data & d);
}
#endif
