#ifndef DATA_LIST_H
#define DATA_LIST_H

// Gtk::Widget, показывающий MapviewData

class DataList : public Gtk::Widget{
  boost::shared_ptr<MapviewData> mapview_data;

  public:
  DataList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
  }

};

#endif
