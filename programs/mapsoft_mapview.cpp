#include <gtkmm.h>

//#include <boost/shared_ptr.hpp>

//#include <io_new/io.h>
//#define DEBUG_VIEWER
//#define DEBUG_GOOGLE
//#define DEBUG_LAYER_MAP
#define DEBUG_JPEG

#include "viewer.h"
#include <point.h>
#include "workplane.h"
#include "layer_grid.h"
//#include "layer_wait.h"
#include "layer_map.h"
//#include "layer_geodata.h"

#include "../geo_io/io.h"

//int sc = 1;
geo_data world;

//const std::string google_dir = "/home/ushakov/devel/maps/google";
//const std::string google_dir = "/d/MAPS/GOOGLE";
//const std::string data_file  = "./track.plt";

LayerMap     ml(&world.maps);

Gtk::Statusbar  * status_bar = NULL;

bool google_downloading = false;

void clear_data(Viewer * v) {
   g_print ("Clear all data");
   status_bar->push("Clear all data", 0);
   world.clear();
//   dl.set_scale(sc);
   v->clear_cache();
}

void load_file(Gtk::FileSelection * file_selector, Viewer * v) {
   std::string selected_filename;
   selected_filename = file_selector->get_filename();
   g_print ("Loading: %s\n", selected_filename.c_str());
   status_bar->push("Loading...", 0);
   io::in(selected_filename, world, Options());
//   dl.set_scale(sc);
   ml.set_ref();
   v->clear_cache();
}

void save_file(Gtk::FileSelection * file_selector) {
   std::string selected_filename;
   selected_filename = file_selector->get_filename();
   g_print ("Saving file: %s\n", selected_filename.c_str());
   status_bar->push("Saving...", 0);
   io::out(selected_filename, world, Options());
}

gboolean on_delete (GdkEventAny * e, Gtk::Window * win){
  win->hide();
  return true;
}

gboolean on_keypress ( GdkEventKey * event, Workplane * w, Viewer * v ) {

    switch (event->keyval) {
    case 43:                                                                           
    case 65451: // +                                                                   
    {                                                                                  
      v->scale_inc();                                                                     
	std::cerr << " scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
      return true;                                                                     
    }                                                                                  
    case 45:                                                                           
    case 65453: // -                                                                   
    {                                                                                  
      v->scale_dec();                                                                     
	std::cerr << " scale: " 
                  << v->scale_nom() << ":" 
                  << v->scale_denom() <<  std::endl;
      return true;                                                                     
    }                                                                                  
    }
    return false;
}


int
main(int argc, char **argv)
{
    Gtk::Main kit (argc, argv);

    // main window
    Gtk::Window win (Gtk::WINDOW_TOPLEVEL);
    win.signal_delete_event().connect (sigc::bind (sigc::ptr_fun (&on_delete), &win));
    win.set_default_size(640,480);

    //чтение файлов из командной строки:
    for(int i=1;i<argc;i++){
      io::in(std::string(argv[i]), world, Options());
    }
//    dl.set_scale(sc);
    ml.set_ref();

    //viewer/workplane/layers
    Workplane w(256,0);
    LayerGrid l1(200.0,200.0,0xFF000080);

    w.add_layer(&ml,200);
//    w.add_layer(&l1,100);
//    w.add_layer(&dl,50);

    Viewer viewer(w);
    viewer.set_window_origin((ml.range().TLC() + ml.range().BRC())/2);

    //load file selector
    Gtk::FileSelection file_sel_load ("Load file:");
    file_sel_load.get_ok_button()->signal_clicked().connect (sigc::bind<0> (sigc::bind<0>(sigc::ptr_fun (&load_file), &file_sel_load), &viewer));
    file_sel_load.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));
    file_sel_load.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));

    //save file selector
    Gtk::FileSelection file_sel_save ("Save as:");
    file_sel_save.get_ok_button()->signal_clicked().connect (sigc::bind<0> (sigc::ptr_fun (&save_file), &file_sel_save));
    file_sel_save.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));
    file_sel_save.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));

    /***************************************/
    //start building menus
    Gtk::MenuBar main_menu;
    Gtk::MenuItem mmenu_file ("_File",    true);
    Gtk::MenuItem mmenu_geod ("_Geodata", true);
    Gtk::MenuItem mmenu_help ("_Help",    true);

    // menu items
    Gtk::MenuItem menu_add   ("_Add",   true);
    Gtk::MenuItem menu_clear ("_Clear", true);
    Gtk::MenuItem menu_save  ("_Save",  true);
    Gtk::MenuItem menu_exit  ("_Exit",  true);
    

    menu_exit.signal_activate().connect  (sigc::mem_fun (win, &Gtk::Widget::hide));
    menu_add.signal_activate().connect   (sigc::mem_fun (file_sel_load,  &Gtk::Widget::show));
    menu_clear.signal_activate().connect (sigc::bind<0> (sigc::ptr_fun (&clear_data), &viewer));
    menu_save.signal_activate().connect  (sigc::mem_fun (file_sel_save,  &Gtk::Widget::show));

    // file menu
    Gtk::Menu file_menu;
    file_menu.append(menu_exit);

    // geodata menu
    Gtk::Menu geod_menu;
    geod_menu.append(menu_add);
    geod_menu.append(menu_clear);
    geod_menu.append(menu_save);

    // main menu items
    mmenu_help.set_right_justified ();
    mmenu_file.set_submenu(file_menu);
    mmenu_geod.set_submenu(geod_menu);
    
    // main menu
    main_menu.append(mmenu_file);
    main_menu.append(mmenu_geod);
    main_menu.append(mmenu_help);
    /***************************************/

    //status_bar
    status_bar = new Gtk::Statusbar;
    status_bar->push("Welcome to mapsoft viewer!",0);

    guint drawing_padding = 5;

    // vbox
    Gtk::VBox vbox (false, 0);
    vbox.pack_start(main_menu,   false, false, 0);
    vbox.pack_start(viewer,      true,  true, drawing_padding);
    vbox.pack_start(*status_bar, false, false, 0);
    //
    win.add (vbox);
    win.signal_key_press_event().connect (sigc::bind<1> ( sigc::bind<1> (sigc::ptr_fun (&on_keypress), &w) , &viewer));

    win.show_all();
    kit.run(win);
}
