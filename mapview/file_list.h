#ifndef FILE_LIST_H
#define FILE_LIST_H

// Gtk::Widget, показывающий список файлов из MapviewData


class FileListColumns : public Gtk::TreeModelColumnRecord {
  public:
    FileListColumns() {  add(text);}
    Gtk::TreeModelColumn<Glib::ustring> text;
};


class FileList : public Gtk::TreeView{
  // ссылка на данные
  boost::shared_ptr<MapviewData> mapview_data;
 
  //список файлов
  Glib::RefPtr<Gtk::ListStore> store;
  FileListColumns columns;

  public:

  FileList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
      // подцепим сигнал
      mapview_data->signal_refresh.connect(sigc::mem_fun(*this, &FileList::refresh));

      store = Gtk::ListStore::create(columns);
      set_model(store);
      append_column_editable("files", columns.text);
      store->set_sort_column(columns.text, Gtk::SORT_ASCENDING);
      set_enable_search(false);
      refresh();
  }

  // обновить данные
  void refresh(){
//    store.clear();
    for (MapviewData::iterator i = mapview_data->begin(); i!=mapview_data->end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      Gtk::TreeModel::Row row = *it;
      row[columns.text] = i->name;
    }
  }

};

#endif
