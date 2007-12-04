#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include <vector>
#include <string>
#include "../geo_io/geo_data.h"
#include "./zn.h"

struct map_object : public g_line{
  std::string               name;
  std::vector<std::string>  comm;
  zn *type;

  map_object(const zn * _type=0): type(_type) {}
};

#endif
