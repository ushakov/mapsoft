#ifndef FILE_LIST_AM_H
#define FILE_LIST_AM_H

// ActionManager для DataList

#include "mapview.h"

class FileListAM{
  boost::shared_ptr<FileList> file_list;
  boost::shared_ptr<Mapview>  mapview;

  public:
  FileListAM(boost::shared_ptr<FileList> file_list_, 
             boost::shared_ptr<Mapview>  mapview_):
    file_list(file_list_), mapview(mapview_){

      
//    mapview->menubar->add_item(
//	"File/New", 
//	Gtk::AccelKey('n', Gdk::CONTROL_MASK, "<MainWindow>/File/New1"), 
//	// понять, как можно применять адрес в AccelKey!!!
//        sigc::mem_fun(mapview->mapview_data, MapviewData::new_file)
//    );

  }
};

#endif
