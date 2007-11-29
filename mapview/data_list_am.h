#ifndef DATA_LIST_AM_H
#define DATA_LIST_AM_H

// ActionManager для DataList

class Mapview;

class DataListAM{
  boost::shared_ptr<DataList> data_list;
  Mapview * mapview;

  public:
  DataListAM(boost::shared_ptr<DataList> data_list_, Mapview * mapview_):
    data_list(data_list_), mapview(mapview_){
  }
};

#endif
