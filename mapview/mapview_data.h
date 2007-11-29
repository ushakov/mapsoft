#ifndef MAPVIEW_DATA_H
#define MAPVIEW_DATA_H

// ��������� ������ ��� �������� ������ ����������.

#include <list>
#include <string>
#include "../geo_io/geo_data.h"
#include "workplane.h"

class MapviewFile : geo_data{
  std::string fname; // ��� �����
  std::vector<int> wd, td, md;  // ������� �����, ������ � ����
  std::vector<int> wv, tv, mv;  // ��������� �����, ������ � ����
  MapviewFile(std::string name): fname(name){} // ������� ����� ����
  load(std::string name){ // ��������� �� �����
  }
};

class MapviewData : std::list<MapviewFile>{
  MapviewFile * active;
  Workplane workplane;
}

#endif