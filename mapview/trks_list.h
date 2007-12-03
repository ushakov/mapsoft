#ifndef TRKS_LIST_H
#define TRKS_LIST_H

// Gtk::Widget, показывающий треки активного файла из MapviewData

class TRKSList : public Gtk::TreeView{
  boost::shared_ptr<MapviewData> mapview_data;

  public:
  TRKSList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
  }

};

#endif
