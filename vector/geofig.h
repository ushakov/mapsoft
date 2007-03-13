#ifndef GEOFIG_H
#define GEOFIG_H

#include <vector>

#include "fig.h"
#include "../geo_io/geo_data.h"


namespace fig {

  // Извлечь привязку из fig-картинки
  g_map get_map(const fig_world & w);

/*
  // Удалить старые точки привязки, если они есть,
  // сделать новые
  void  set_map(const fig_world & w, const g_map & map);

  // пересчитать точки объекта в lon-lat wgs84
  std::vector<g_point> get_geo_crds(const fig_object & o, const g_map & map); 
  // изменить точки объекта
  void set_geo_crds(fig_object & o, const g_map & map, const std::vector<g_point> crds); 
  // при пересчете линий могут добавляться новые точки!
*/
}
#endif
