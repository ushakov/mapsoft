#ifndef FILE_LIST_H
#define FILE_LIST_H

// Gtk::Widget, показывающий список файлов из MapviewData

class FileListColumns : public Gtk::TreeModelColumnRecord {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    FileListColumns() {  
      add(name);
    }
};


class FileList : public Gtk::TreeView{
  // ссылка на данные
  boost::shared_ptr<MapviewData> mapview_data;
 
  Glib::RefPtr<Gtk::ListStore> store;
  FileListColumns columns;

  public:
  FileList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
    // подцепим сигнал, чтоб обнавляться, когда меняются данные...
    mapview_data->signal_refresh.connect(sigc::mem_fun(*this, &FileList::refresh));
    store = Gtk::ListStore::create(columns);
    set_model(store);
    append_column("files:", columns.name);
    set_headers_visible(false);
//    store->set_sort_column(columns.name, Gtk::SORT_ASCENDING);
    set_enable_search(false);
    refresh();
  }

  // обновить данные
  void refresh(){
    store->clear();
    int n=0;
    for (MapviewData::iterator i = mapview_data->begin(); i!=mapview_data->end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      Gtk::TreeModel::Row row = *it;
      row[columns.name] = i->name;
//      if (i == mapview_data->current_file) set_cursor(Gtk::TreePath(n));
      n++;
    }
  }


};

#endif
