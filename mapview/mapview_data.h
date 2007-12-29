#ifndef MAPVIEW_DATA_H
#define MAPVIEW_DATA_H

// ����� �� ����� ���������� � workplane'��

#include <boost/shared_ptr.hpp>
#include <sigc++/sigc++.h>
#include <vector>
#include <string>

#include "../geo_io/geo_data.h"
#include "../geo_io/io.h"
#include "../layers/layer.h"
#include "workplane.h"

// ������-�� ������� ���� ������ layer'� �������...
#include "../layers/layer_geodata.h"
#include "../layers/layer_geomap.h"

// ���� ������
struct MapviewDataFile : public geo_data{
  std::string name;
  std::vector<int> wpts_d, trks_d, maps_d; // ������� ���� ���������
  std::vector<int> wpts_v, trks_v, maps_v; // ��������� ���� ���������
  std::vector<boost::shared_ptr<Layer> > wpts_l, trks_l, maps_l;
};

class MapviewData : public std::list<MapviewDataFile>{
  public:
    // workplane � �������
    boost::shared_ptr<Workplane> workplane;

    // �������� ����
    std::list<MapviewDataFile>::iterator current_file;

    // ������, ��� ������ ����������
    sigc::signal<void> signal_refresh;

    MapviewData(){
      workplane.reset(new Workplane());
      current_file=end();
    }

    void load_file(std::string name){
      MapviewDataFile file;
      file.name = name;
      io::in(name, file, Options());
      file.wpts_d.resize(file.wpts.size(), 100);
      file.trks_d.resize(file.trks.size(), 200);
      file.maps_d.resize(file.maps.size(), 300);
      file.wpts_v.resize(file.wpts.size(), true);
      file.trks_v.resize(file.trks.size(), true);
      file.maps_v.resize(file.maps.size(), true);
      file.wpts_l.resize(file.wpts.size());
      file.trks_l.resize(file.trks.size());
      file.maps_l.resize(file.maps.size());
// ���� ������� layer's ��� �����, ������ � ����...
//      for (int i=0; i<file.wpts.size(); i++) wpts_l = new LayerGeoWPT(file);
      boost::shared_ptr<Layer> ML, DL;
      ML.reset(new LayerGeoMap(&file));
      DL.reset(new LayerGeoData(&file));
      file.wpts_l.push_back(DL);
      file.maps_l.push_back(ML);
      workplane->add_layer(DL, 100);
      workplane->add_layer(ML, 200);
      push_back(file);
      signal_refresh.emit();
    }

    void save_file(std::string name){
    }

    void new_file(std::string name = "new_file"){
      MapviewDataFile new_file;
      new_file.name = name;
      push_back(new_file);
      signal_refresh.emit();
    }

};

#endif
