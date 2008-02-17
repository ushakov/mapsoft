#ifndef FILE_LIST_H
#define FILE_LIST_H

// Gtk::Widget, показывающий список файлов из MapviewData

class FileListColumns : public Gtk::TreeModelColumnRecord {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<bool> vis;
    FileListColumns() {  
      add(vis);
      add(name);
    }
};


class FileList : public Gtk::TreeView{
  // ссылка на данные
  boost::shared_ptr<MapviewData> mapview_data;
 
  Glib::RefPtr<Gtk::ListStore> store;
  bool do_refresh;

  public:
  FileListColumns columns;

  FileList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_), do_refresh(true){
    // подцепим сигнал, чтоб обновляться, когда меняются данные...
    mapview_data->signal_refresh.connect(sigc::mem_fun(*this, &FileList::refresh));

    store = Gtk::ListStore::create(columns);

    set_model(store);
    append_column_editable("", columns.vis);
    append_column_editable("files:", columns.name);
    set_headers_visible(false);
    set_enable_search(false);
    set_reorderable();
    refresh();
  }

  // обновить данные
  // проблема: хочется позволять менять активный файл и через mapview_data, 
  // и через интерфейс. Надо не зациклить сигналы.
  // get_selection()->select() вызывает такой же сигнал, как и gui
  // при этом все игналы должны испускаться:
  //  refresh от mapview_data должен быть пойман WPTList и К
  //  select от file_list должен быть пойман am и направлен в mapview_data, 
  void refresh(){
    if (!do_refresh){do_refresh=true; return;}
    store->clear();
    for (MapviewData::iterator i = mapview_data->begin(); i!=mapview_data->end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      (*it)[columns.name] = i->name;
      if (i==mapview_data->active_file){
        do_refresh=false;
        get_selection()->select(*it);
      }
    }
    /*
    for (Gtk::TreeModel::Children::iterator it = store->children().begin(); 
         it!=store->children().end(); it++){
      if ((*it)[columns.name]==mapview_data->active_file->name) 
        get_selection()->select(*it);
    }*/
  }


};

#endif
