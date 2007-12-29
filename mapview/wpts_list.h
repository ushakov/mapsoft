#ifndef WPTS_LIST_H
#define WPTS_LIST_H

// Gtk::Widget, показывающий waypoints активного файла из MapviewData

class WPTSListColumns : public Gtk::TreeModelColumnRecord {
  public:
    WPTSListColumns() {  add(text);}
    Gtk::TreeModelColumn<Glib::ustring> text;
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
    mapview_data->signal_refresh.connect(sigc::mem_fun(*this, &WPTSList::refresh));
    store = Gtk::ListStore::create(columns);
    set_model(store);
    append_column_editable("", columns.text);
    store->set_sort_column(columns.text, Gtk::SORT_ASCENDING);
    set_enable_search(false);
    add_events(Gdk::BUTTON_PRESS_MASK);
    refresh();
  }
  // обновить данные
  void refresh(){
    store->clear();
    if (mapview_data->current_file == mapview_data->end()) return;
    for (std::vector<g_waypoint_list>::const_iterator
        i = mapview_data->current_file->wpts.begin();
        i!= mapview_data->current_file->wpts.begin(); i++){
      Gtk::TreeModel::iterator it = store->append();
      Gtk::TreeModel::Row row = *it;
      row[columns.text] = "---";
      for (g_waypoint_list::const_iterator w = i->begin(); w!=i->end(); i++){
        Gtk::TreeModel::iterator it = store->append();
        Gtk::TreeModel::Row row = *it;
        row[columns.text] = w->name;
      }
    }
  }

};

#endif
