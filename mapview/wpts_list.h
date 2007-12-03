#ifndef WPTS_LIST_H
#define WPTS_LIST_H

// Gtk::Widget, показывающий waypoints активного файла из MapviewData

class WPTSList : public Gtk::TreeView{
  boost::shared_ptr<MapviewData> mapview_data;

  public:
  WPTSList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
  }

};

#endif
