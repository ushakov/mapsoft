#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <vector>
#include <string>
#include "../geo_io/geo_data.h"

class zn;

struct map_object : public std::vector<g_point>{
  std::string               name;
  std::vector<std::string>  comm;
  const zn * type;

  public:
  map_object(const zn * _type=0): type(_type) {}
};

#endif
