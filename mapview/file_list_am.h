#ifndef FILE_LIST_AM_H
#define FILE_LIST_AM_H

// ActionManager для FileList
// загрузка/сохранение/удаление файлов

#include "mapview.h"

class FileListAM{
  private:

  boost::shared_ptr<FileList> file_list;
  boost::shared_ptr<Mapview>  mapview;
  Gtk::FileSelection file_load_sel;
  Gtk::FileSelection file_save_sel;
  Gtk::Menu *popup;

  public:
  FileListAM(boost::shared_ptr<FileList> file_list_, 
             boost::shared_ptr<Mapview>  mapview_):
    file_list(file_list_), mapview(mapview_),
    file_load_sel ("Load file:"), file_save_sel ("Save as:"){


    // connect events from file_list
    file_list->signal_button_press_event().connect
      (sigc::mem_fun (this, &FileListAM::mouse_button_pressed));

//    file_list->signal_cursor_changed().connect
//      (sigc::mem_fun (this, &FileListAM::cursor_changed));

//    file_list->signal_row_activated().connect
//      (sigc::mem_fun (this, &FileListAM::row_activated));

    //load file selector
    file_load_sel.get_ok_button()->signal_clicked().connect (sigc::mem_fun (this, &FileListAM::load_file));
    file_load_sel.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_load_sel, &Gtk::Widget::hide));
    file_load_sel.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_load_sel, &Gtk::Widget::hide));

    //save file selector
    file_save_sel.get_ok_button()->signal_clicked().connect (sigc::mem_fun (this, &FileListAM::save_file));
    file_save_sel.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_save_sel, &Gtk::Widget::hide));
    file_save_sel.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_save_sel, &Gtk::Widget::hide));

    // creating actions
    mapview->actiongroup->add( Gtk::Action::create("file_list_menu", "File"));
    mapview->actiongroup->add( Gtk::Action::create("file_list_new", Gtk::Stock::NEW),
      sigc::mem_fun (this, &FileListAM::new_file));
    mapview->actiongroup->add( Gtk::Action::create("file_list_load", Gtk::Stock::ADD),
      sigc::mem_fun(file_load_sel, &Gtk::Widget::show));
    mapview->actiongroup->add( Gtk::Action::create("file_list_save", Gtk::Stock::SAVE_AS),
      sigc::mem_fun(file_save_sel, &Gtk::Widget::show));

    // adding actions to menues
    mapview->uimanager->add_ui_from_string(
      "<ui>"
      "  <menubar action='menubar'>"
      "    <menu action='file_list_menu'>"
      "      <menuitem action='file_list_new'/>"
      "      <menuitem action='file_list_load'/>"
      "      <menuitem action='file_list_save'/>"
      "      <menuitem action='mapview_quit'/>"
      "    </menu>"
      "  </menubar>"
      "  <popup action='file_list_popup'>"
      "    <menuitem action='file_list_new'/>"
      "    <menuitem action='file_list_load'/>"
      "    <menuitem action='file_list_save'/>"
      "    <menuitem action='mapview_quit'/>"
      "  </popup>"
      "</ui>"
    );
    // creating popup menu
    popup = dynamic_cast<Gtk::Menu*>(mapview->uimanager->get_widget("/file_list_popup"));
  }

  bool mouse_button_pressed(GdkEventButton* event){
    VLOG(2) << "press: " << event->x << "," << event->y << " " << event->button;
    if (event->button == 3){
      if(popup) popup->popup(event->button, event->time);
      return true;
    }
    return false;
  }

  void load_file() {
    mapview->statusbar.push("Loading file " + file_load_sel.get_filename());
    mapview->mapview_data->load_file(file_load_sel.get_filename());
  }
  void save_file() {
    mapview->statusbar.push("Saving file " + file_save_sel.get_filename());
    mapview->mapview_data->save_file(file_save_sel.get_filename());
  }
  void new_file() {
    mapview->statusbar.push("Creating new file");
    mapview->mapview_data->new_file();
  }
  void cursor_changed() {
    Gtk::TreeModel::Path p;
    Gtk::TreeViewColumn *c;
    file_list->get_cursor(p,c);
    mapview->mapview_data->set_current_file(p.front());
    std::cerr << "file n " << p.front() << " selected\n";
  }

//  void row_activated(const TreeModel::Path& p, TreeViewColumn* c) {
//    mapview->mapview_data->set_current_file(p.front());
//    std::cerr << "file n " << p.front() << " selected\n";
// }


};

#endif
