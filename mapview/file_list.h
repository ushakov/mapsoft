#ifndef FILE_LIST_H
#define FILE_LIST_H

// Gtk::Widget, ������������ ������ ������ �� MapviewData

#include <gtkmm.h>

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
  // ������ �� ������
  boost::shared_ptr<MapviewData> mapview_data;
 
  Glib::RefPtr<Gtk::ListStore> store;

  public:
  FileListColumns columns;

  FileList(boost::shared_ptr<MapviewData> mapview_data_):
    mapview_data(mapview_data_){
    // �������� ������, ���� �����������, ����� �������� ������...
    mapview_data->signal_refresh_files.connect(sigc::mem_fun(*this, &FileList::refresh));

    store = Gtk::ListStore::create(columns);

    set_model(store);
    append_column_editable("", columns.vis);
    append_column_editable("files:", columns.name);
    set_headers_visible(false);
    set_enable_search(false);
    set_reorderable();
    refresh();
  }

  // �������� ������
  // ��������: ������� ��������� ������ �������� ���� � ����� mapview_data, 
  // � ����� ���������. ���� �� ��������� �������.
  // get_selection()->select() �������� ����� �� ������, ��� � gui
  // ��� ���� ��� ������ ������ �����������:
  //  refresh �� mapview_data ������ ���� ������ WPTList � �
  //  select �� file_list ������ ���� ������ am � ��������� � mapview_data, 

  void refresh(){
    store->clear();
    for (MapviewData::iterator i = mapview_data->begin(); i!=mapview_data->end(); i++){
      Gtk::TreeModel::iterator it = store->append();
      (*it)[columns.name] = i->name;
      if (i==mapview_data->active_file)  get_selection()->select(*it);
    }
  }


};

#endif
