#ifndef WPTS_LIST_H
#define WPTS_LIST_H

#include "../utils/m_time.h"
// Gtk::Widget, показывающий waypoints активного файла из MapviewData

class WPTSListColumns : public Gtk::TreeModelColumnRecord {
  public:
    WPTSListColumns() {  add(name);}
    Gtk::TreeModelColumn<Glib::ustring> name;
//    Gtk::TreeModelColumn<Time>          time;
};


class WPTSList : public Gtk::TreeView{
  // ссылка на данные
  boost::shared_ptr<MapviewData> mapview_data;

  Glib::RefPtr<Gtk::ListStore> store;
  WPTSListColumns columns;

  public:
  WPTSList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
    // подцепим сигнал
    mapview_data->signal_refresh_data.connect(sigc::mem_fun(*this, &WPTSList::refresh));
    store = Gtk::ListStore::create(columns);
    set_model(store);
    set_headers_visible(false);
    append_column_editable("name", columns.name);
//    append_column_editable("time", columns.time);
    store->set_sort_column(columns.name, Gtk::SORT_ASCENDING);
    set_enable_search(false);
    add_events(Gdk::BUTTON_PRESS_MASK);
    refresh();
  }
  // обновить данные
  void refresh(){
    store->clear();
    if (mapview_data->active_file == mapview_data->end()) return;
    for (std::vector<g_waypoint_list>::const_iterator
        i = mapview_data->active_file->wpts.begin();
        i!= mapview_data->active_file->wpts.end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      Gtk::TreeModel::Row row = *it;
      row[columns.name] = "---";
      for (g_waypoint_list::const_iterator w = i->begin(); w!=i->end(); w++){
        Gtk::TreeModel::iterator it = store->append();
        Gtk::TreeModel::Row row = *it;
        row[columns.name] = w->name;
//        row[columns.time] = w->t;
      }
    }
  }

};

#endif
