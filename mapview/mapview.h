#ifndef MAPVIEW_PARTS_H
#define MAPVIEW_PARTS_H

// Класс, создающий все управляемые компоненты вьюера и
// связывающий их в одно окно
//
// ссылка на этот класс дается всем ActionManager'ам,
// чтоб они творили со всеми компонентами разное.

#include <gtkmm.h>
#include <boost/shared_ptr.hpp>

#include "mapview_data.h"
#include "rubber.h"
#include "viewer.h"
#include "file_list.h"
#include "wpts_list.h"
#include "trks_list.h"
#include "maps_list.h"
#include "menubar.h"
#include "statusbar.h"

class Mapview : public Gtk::Window{
  public:
    boost::shared_ptr<MapviewData> mapview_data; // структура со всеми геоданными и workplane'ом
    boost::shared_ptr<Rubber>      rubber;       // "резина" - xor-линии, цепляющиеся к мышке
    boost::shared_ptr<Viewer>      viewer;       // интерфейсный gtk-widget, показывающий workplane и rubber
    boost::shared_ptr<FileList>    file_list;    // интерфейсный gtk-widget, показывающий файлы из mapview_data
    boost::shared_ptr<WPTSList>    wpts_list;    // интерфейсный gtk-widget, показывающий точки текущего файла из mapview_data
    boost::shared_ptr<TRKSList>    trks_list;    // интерфейсный gtk-widget, показывающий треки текущего файла из mapview_data
    boost::shared_ptr<MAPSList>    maps_list;    // интерфейсный gtk-widget, показывающий карты текущего файла из mapview_data
    boost::shared_ptr<MenuBar>     menubar;      // меню (кажется, это не должен быть интерфейс в нашем смысле :))
    boost::shared_ptr<StatusBar>   statusbar;    // gtk-widget показывающий разный текст

  Mapview(){
    // создадим все компоненты
    mapview_data.reset(new MapviewData());
    rubber.reset(new Rubber());
    viewer.reset(new Viewer(mapview_data->workplane, rubber));
    file_list.reset(new FileList(mapview_data));
    wpts_list.reset(new WPTSList(mapview_data));
    trks_list.reset(new TRKSList(mapview_data));
    maps_list.reset(new MAPSList(mapview_data));
    menubar.reset(new MenuBar());
    statusbar.reset(new StatusBar());

    // запакуем все GTK-компоненты в окно
    guint drawing_padding = 5;

    // Все списки - в ScrolledWindow
    Gtk::ScrolledWindow * file_list_scrw = manage(new Gtk::ScrolledWindow);
    file_list_scrw->add(*file_list);
    file_list_scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    file_list_scrw->set_size_request(200,100);

    Gtk::ScrolledWindow * wpts_list_scrw = manage(new Gtk::ScrolledWindow);
    wpts_list_scrw->add(*wpts_list);
    wpts_list_scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    wpts_list_scrw->set_size_request(200,-1);

    Gtk::ScrolledWindow * trks_list_scrw = manage(new Gtk::ScrolledWindow);
    trks_list_scrw->add(*trks_list);
    trks_list_scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    trks_list_scrw->set_size_request(200,-1);

    Gtk::ScrolledWindow * maps_list_scrw = manage(new Gtk::ScrolledWindow);
    maps_list_scrw->add(*maps_list);
    maps_list_scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    maps_list_scrw->set_size_request(200,-1);

    // Списки точек, треков, карт - в Notebook wtm_tabs
    Gtk::Notebook * wtm_tabs = manage(new Gtk::Notebook);
    wtm_tabs->append_page(*wpts_list_scrw, "waypoints", false);
    wtm_tabs->append_page(*trks_list_scrw, "tracks", false);
    wtm_tabs->append_page(*maps_list_scrw, "maps", false);

    // VPaned lists <- file_list, wtm_tabs
    Gtk::VPaned * lists = manage(new Gtk::VPaned);
    lists->pack1(*file_list_scrw, Gtk::FILL);
    lists->pack2(*wtm_tabs, Gtk::EXPAND |Gtk::FILL);

    // HPaned paned <- viewer + lists
    Gtk::HPaned * paned = manage(new Gtk::HPaned);
    paned->pack1(*viewer, Gtk::EXPAND | Gtk::FILL);
    paned->pack2(*lists, Gtk::FILL);

    // VBox vbox <- menubar + paned + statusbar
    Gtk::VBox * vbox = manage(new Gtk::VBox);
    vbox->pack_start(*menubar->get_widget(), false, true, 0);
    vbox->pack_start(*paned, true, true, drawing_padding);
    vbox->pack_start(*statusbar, false, true, 0);
    add(*vbox);
    set_default_size(640,480);

    show_all();
  }
};

#endif
