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
    file_list->signal_button_press_event().connect_notify
      (sigc::mem_fun (this, &FileListAM::mouse_button_pressed)); // for popup menu

    file_list->get_selection()->signal_changed().connect
      (sigc::mem_fun (this, &FileListAM::set_active_file)); // for change of act.file in mapsoft_data

//    file_list->get_column_cell_renderer(0)->signal_edited().connect(sigc::mem_fun (this, &FileListAM::change_filename)); 

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
    mapview->actiongroup->add( Gtk::Action::create("file_list_save", Gtk::Stock::SAVE),
      sigc::mem_fun(this, &FileListAM::save_file));
    mapview->actiongroup->add( Gtk::Action::create("file_list_save_as", Gtk::Stock::SAVE_AS),
      sigc::mem_fun(file_save_sel, &Gtk::Widget::show));
    mapview->actiongroup->add( Gtk::Action::create("file_list_get_usb", "from USB port"),
      sigc::mem_fun(this, &FileListAM::get_from_usb));
    mapview->actiongroup->add( Gtk::Action::create("file_list_put_usb", "to USB port"),
      sigc::mem_fun(this, &FileListAM::put_to_usb));
    mapview->actiongroup->add( Gtk::Action::create("file_list_delete", Gtk::Stock::DELETE),
      sigc::mem_fun(this, &FileListAM::delete_file));

    // adding actions to menues
    mapview->uimanager->add_ui_from_string(
      "<ui>"
      "  <menubar action='menubar'>"
      "    <menu action='file_list_menu'>"
      "      <menuitem action='file_list_new'/>"
      "      <menuitem action='file_list_load'/>"
      "      <menuitem action='file_list_get_usb'/>"
      "      <menuitem action='mapview_quit'/>"
      "    </menu>"
      "  </menubar>"
      "  <popup action='file_list_popup'>"
      "    <menuitem action='file_list_new'/>"
      "    <menuitem action='file_list_load'/>"
      "    <menuitem action='file_list_save'/>"
      "    <menuitem action='file_list_save_as'/>"
      "    <menuitem action='file_list_get_usb'/>"
      "    <menuitem action='file_list_put_usb'/>"
      "    <menuitem action='file_list_delete'/>"
      "    <menuitem action='mapview_quit'/>"
      "  </popup>"
      "</ui>"
    );
    // creating popup menu
    popup = dynamic_cast<Gtk::Menu*>(mapview->uimanager->get_widget("/file_list_popup"));
  }

  void mouse_button_pressed(GdkEventButton* event){
    VLOG(2) << "press: " << event->x << "," << event->y << " " << event->button;
    if (event->button == 3)  if(popup) popup->popup(event->button, event->time);
  }


  void load_file() {
    mapview->statusbar.push("Loading file " + file_load_sel.get_filename());
    mapview->mapview_data->load_file(file_load_sel.get_filename());
  }
  void save_file_as() {
    mapview->statusbar.push("Saving file " + file_save_sel.get_filename());
    mapview->mapview_data->save_active_file(file_save_sel.get_filename());
  }
  void save_file() {
    mapview->statusbar.push("Saving file");
    mapview->mapview_data->save_active_file();
  }
  void delete_file() {
    mapview->statusbar.push("Deleting file");
    mapview->mapview_data->delete_active_file();
  }
  void new_file() {
    mapview->statusbar.push("Creating new file");
    mapview->mapview_data->new_file();
  }
  void get_from_usb() {
    mapview->statusbar.push("Aquiring data from USB port");
    mapview->mapview_data->get_from_usb();
  }
  void put_to_usb() {
    mapview->statusbar.push("Sending data to USB port");
    mapview->mapview_data->put_active_to_usb();
  }
  void set_active_file() {
    Gtk::TreeModel::iterator i = file_list->get_selection()->get_selected();
    if (!i) return;
    Glib::ustring name = (*i)[file_list->columns.name];
    mapview->mapview_data->set_active_file(name, false);
    std::cerr << "file " << name << " selected\n";
  }
  void change_filename() {
    Gtk::TreeModel::iterator i = file_list->get_selection()->get_selected();
    if (!i) return;
    Glib::ustring name = (*i)[file_list->columns.name];
    mapview->mapview_data->change_active_file_name(name);
    std::cerr << "filename changed\n";
  }
  void toggle_visibility(){
    // TODO
  }
  void change_order(){
    // TODO
  }

};

#endif
