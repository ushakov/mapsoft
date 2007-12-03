#ifndef MAPS_LIST_H
#define MAPS_LIST_H

// Gtk::Widget, показывающий карты активного файла из MapviewData

class MAPSList : public Gtk::TreeView{
  boost::shared_ptr<MapviewData> mapview_data;

  public:
  MAPSList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
  }

};

#endif
