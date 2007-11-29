#ifndef MAPVIEW_DATA_H
#define MAPVIEW_DATA_H

// Структуры данных для хранения разной информации.

#include <list>
#include <string>
#include "../geo_io/geo_data.h"
#include "workplane.h"

class MapviewFile : geo_data{
  std::string fname; // имя файла
  std::vector<int> wd, td, md;  // глубины точек, треков и карт
  std::vector<int> wv, tv, mv;  // видимость точек, треков и карт
  MapviewFile(std::string name): fname(name){} // создать новый файл
  load(std::string name){ // загрузить из файла
  }
};

class MapviewData : std::list<MapviewFile>{
  MapviewFile * active;
  Workplane workplane;
}

#endif