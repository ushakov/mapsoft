#ifndef TRKS_LIST_H
#define TRKS_LIST_H

// Gtk::Widget, показывающий треки активного файла из MapviewData

class TRKSListColumns : public Gtk::TreeModelColumnRecord {
  public:
    TRKSListColumns() {  add(comm);}
    Gtk::TreeModelColumn<Glib::ustring> comm;
};


class TRKSList : public Gtk::TreeView{
  // ссылка на данные
  boost::shared_ptr<MapviewData> mapview_data;

  Glib::RefPtr<Gtk::ListStore> store;
  TRKSListColumns columns;
 
  public:
  TRKSList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
    // подцепим сигнал
    mapview_data->signal_refresh_data.connect(sigc::mem_fun(*this, &TRKSList::refresh));
    store = Gtk::ListStore::create(columns);
    set_model(store);
    set_headers_visible(false);
    append_column_editable("", columns.comm);
    store->set_sort_column(columns.comm, Gtk::SORT_ASCENDING);
    set_enable_search(false);
    add_events(Gdk::BUTTON_PRESS_MASK);
    refresh();
  }

  // обновить данные
  void refresh(){
    store->clear();
    if (mapview_data->active_file == mapview_data->end()) return;
    for (std::vector<g_track>::const_iterator
        i = mapview_data->active_file->trks.begin();
        i!= mapview_data->active_file->trks.end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      Gtk::TreeModel::Row row = *it;
      row[columns.comm] = i->comm;
    }
  }

};

#endif
