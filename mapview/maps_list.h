#ifndef MAPS_LIST_H
#define MAPS_LIST_H

// Gtk::Widget, показывающий карты активного файла из MapviewData

class MAPSListColumns : public Gtk::TreeModelColumnRecord {
  public:
    MAPSListColumns() {  add(comm); add(file);}
    Gtk::TreeModelColumn<Glib::ustring> file;
    Gtk::TreeModelColumn<Glib::ustring> comm;
};


class MAPSList : public Gtk::TreeView{
  // ссылка на данные
  boost::shared_ptr<MapviewData> mapview_data;

  Glib::RefPtr<Gtk::ListStore> store;
  MAPSListColumns columns;

  public:
  MAPSList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
    // подцепим сигнал
    mapview_data->signal_refresh.connect(sigc::mem_fun(*this, &MAPSList::refresh));
    store = Gtk::ListStore::create(columns);
    set_model(store);
    set_headers_visible(false);
    append_column_editable("", columns.comm);
    append_column_editable("", columns.file);
    store->set_sort_column(columns.file, Gtk::SORT_ASCENDING);
    set_enable_search(false);
    add_events(Gdk::BUTTON_PRESS_MASK);
    refresh();
  }

  // обновить данные
  void refresh(){
    store->clear();
    if (mapview_data->current_file == mapview_data->end()) return;
    for (std::vector<g_map>::const_iterator 
        i = mapview_data->current_file->maps.begin();
        i!= mapview_data->current_file->maps.end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      Gtk::TreeModel::Row row = *it;
      row[columns.file] = i->file;
      row[columns.comm] = i->comm;
    }
  }

};

#endif
