#ifndef MAPVIEW_H
#define MAPVIEW_H

// Класс, связывающий все компоненты вьюера в одно окно

#include "mapview_data.h"

#include "rubber.h"
#include "viewer.h"

#include "data_list.h"
#include "data_list_am.h"

#include "menu_data.h"
#include "menubar.h"
#include "menubar_am.h"

#include "statusbar.h"

class Mapview : public Gtk::Window{
  public:
    boost::shared_ptr<MapviewData> mapview_data;
    boost::shared_ptr<Rubber>      rubber;
    boost::shared_ptr<Viewer>      viewer;
    boost::shared_ptr<ViewerAM>    viewer_am;
    boost::shared_ptr<DataList>    data_list;
    boost::shared_ptr<DataListAM>  data_list_am;
    boost::shared_ptr<MenuData>    menu_data;
    boost::shared_ptr<MenuBar>     menubar;
    boost::shared_ptr<MenuBarAM>   menubar_am;
    boost::shared_ptr<StatusBar>   status_bar;

  Mapview(){
    // создадим все компоненты
    mapview_data.reset(new MapviewData());
    rubber.reset(new Rubber);
    viewer.reset(new Viewer(mapview_data->workplane, rubber));
    viewer_am.reset(new ViewerAM(viewer, this));
    data_list.reset(new DataList(mapview_data));
    data_list_am.reset(new DataListAM(data_list, this));
    menudata.reset(new MenuData());
    menubar.reset(new MenuBar(menudata));
    menubar_am.reset(new MenuBarAM(menubar, this));
    statusbar.reset(new StatusBar());

    // запакуем все GTK-компоненты в окно
    guint drawing_padding = 5;

    // ScrolledWindow scrw <- data_list
    Gtk::ScrolledWindow * scrw = manage(new Gtk::ScrolledWindow);
    scrw->add(data_list);
    scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrw->set_size_request(128,-1);

    // HPaned paned <- viewer + scrw
    Gtk::HPaned * paned = manage(new Gtk::HPaned);
    paned->pack1(*viewer, Gtk::EXPAND | Gtk::FILL);
    paned->pack2(*scrw, Gtk::FILL);

    // VBox vbox <- menubar + paned + status_bar
    Gtk::VBox * vbox = manage(new Gtk::VBox);
    vbox->pack_start(*menubar, false, true, 0);
    vbox->pack_start(*paned, true, true, drawing_padding);
    vbox->pack_start(*status_bar, false, true, 0);
    add (*vbox);
    show_all();
  }
};

#endif