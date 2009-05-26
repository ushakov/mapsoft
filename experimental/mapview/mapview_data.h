#ifndef MAPVIEW_DATA_H
#define MAPVIEW_DATA_H

// класс со всеми геоданными и workplane'ом

// данные упорядочены так:
// есть файлы, в которых находятся треки, наборы точек, карты. Файл - один объект deodata
// один файл - активен
// для каждого файла есть три layer'a: для показа всех треков, всех наборов точек и всех карт
// рисуются сперва все карты, потом все треки, потом все точки, в порядке файлов и объектов в них

#include <boost/shared_ptr.hpp>
#include <sigc++/sigc++.h>
#include <vector>
#include <string>

#include "../libgeo/geo_data.h"
#include "../libgeo_io/io.h"
#include "../layers/layer.h"
#include "workplane.h"

// вообще-то хочется сюда другие layer'ы сделать...
#include "../layers/layer_geodata.h"
#include "../layers/layer_geomap.h"

// файл данных
struct MapviewDataFile : public geo_data{
  std::string name;
  int         id;
  std::vector<int> wpts_v, trks_v, maps_v; // Видимость всех элементов
  std::vector<boost::shared_ptr<Layer> > wpts_l, trks_l, maps_l;
};

class MapviewData : public std::list<MapviewDataFile>{
  public:
    // workplane с данными
    boost::shared_ptr<Workplane> workplane;

    // активный файл
    std::list<MapviewDataFile>::iterator active_file;

    // сигналы, что данные изменились
    sigc::signal<void> signal_refresh_files; // для file_list
    sigc::signal<void> signal_refresh_data;  // для wpts_list, trks_list, maps_list, viewer

    MapviewData(){
      workplane.reset(new Workplane());
      active_file=end();
    }

    void load_file(std::string name){
      for ( MapviewData::const_iterator i = begin(); i!=end(); i++){
        if (i->name == name){
          std::cerr << "file " << name << " exists!\n";
          return;
        }
      }
      MapviewDataFile file;
      file.name = name;
      if (!io::in(name, file, Options())){
        std::cerr << "can't read file " << name << "\n";
        return;
      }

      file.wpts_v.resize(file.wpts.size(), true);
      file.trks_v.resize(file.trks.size(), true);
      file.maps_v.resize(file.maps.size(), true);
      file.wpts_l.resize(file.wpts.size());
      file.trks_l.resize(file.trks.size());
      file.maps_l.resize(file.maps.size());
// надо завести layer's для точек, треков и карт...
//      for (int i=0; i<file.wpts.size(); i++) wpts_l = new LayerGeoWPT(file);
      boost::shared_ptr<Layer> ML, DL;
      ML.reset(new LayerGeoMap(&file));
      DL.reset(new LayerGeoData(&file));
      file.wpts_l.push_back(DL);
      file.maps_l.push_back(ML);
      workplane->add_layer(DL, 100);
      workplane->add_layer(ML, 200);
      push_back(file);
      signal_refresh_files.emit();
      signal_refresh_data.emit();
    }

    void get_from_usb(){ load_file("usb:"); }


    void new_file(std::string name = "new_file"){
      for ( MapviewData::const_iterator i = begin(); i!=end(); i++){
        if (i->name == name){
          std::cerr << "file " << name << " exists!\n";
          return;
        }
      }
      MapviewDataFile new_file;
      new_file.name = name;
      push_back(new_file);
      signal_refresh_files.emit();
      signal_refresh_data.emit();
    }

    void set_active_file(std::string name, bool emit_refresh_files=true){
      for ( MapviewData::iterator i = begin(); i!=end(); i++){
        if (i->name == name) active_file=i;
      }
      if (emit_refresh_files) signal_refresh_files.emit();
      signal_refresh_data.emit();
    }

    void change_active_file_name(std::string name){
      for ( MapviewData::const_iterator i = begin(); i!=end(); i++){
        if ((i!=active_file)&&(i->name == name)){
          std::cerr << "file " << name << " exists!\n";
          return;
        }
      }
      if (active_file==end()) return;
      active_file->name=name;
      signal_refresh_files.emit();
    }

    void delete_active_file(){
      if (active_file==end()) return;
      active_file = erase(active_file);
      signal_refresh_files.emit();
      signal_refresh_data.emit();
    }

    void save_active_file(std::string name=""){
      if (name=="") name = active_file->name;
	//TODO
    }

    void put_active_to_usb(){
	//TODO
    }

};

#endif
