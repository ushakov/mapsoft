#ifndef MAPVIEW_PARTS_H
#define MAPVIEW_PARTS_H

// �����, ��������� ��� ����������� ���������� ������ �
// ����������� �� � ���� ����
//
// ������ �� ���� ����� ������ ���� ActionManager'��,
// ���� ��� ������� �� ����� ������������ ������.

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
    boost::shared_ptr<MapviewData> mapview_data; // ��������� �� ����� ���������� � workplane'��
    boost::shared_ptr<Rubber>      rubber;       // "������" - xor-�����, ����������� � �����
    boost::shared_ptr<Viewer>      viewer;       // ������������ gtk-widget, ������������ workplane � rubber
    boost::shared_ptr<FileList>    file_list;    // ������������ gtk-widget, ������������ ����� �� mapview_data
    boost::shared_ptr<WPTSList>    wpts_list;    // ������������ gtk-widget, ������������ ����� �������� ����� �� mapview_data
    boost::shared_ptr<TRKSList>    trks_list;    // ������������ gtk-widget, ������������ ����� �������� ����� �� mapview_data
    boost::shared_ptr<MAPSList>    maps_list;    // ������������ gtk-widget, ������������ ����� �������� ����� �� mapview_data
    boost::shared_ptr<MenuBar>     menubar;      // ���� (�������, ��� �� ������ ���� ��������� � ����� ������ :))
    boost::shared_ptr<StatusBar>   statusbar;    // gtk-widget ������������ ������ �����

  Mapview(){
    // �������� ��� ����������
    mapview_data.reset(new MapviewData());
    rubber.reset(new Rubber());
    viewer.reset(new Viewer(mapview_data->workplane, rubber));
    file_list.reset(new FileList(mapview_data));
    wpts_list.reset(new WPTSList(mapview_data));
    trks_list.reset(new TRKSList(mapview_data));
    maps_list.reset(new MAPSList(mapview_data));
    menubar.reset(new MenuBar());
    statusbar.reset(new StatusBar());

    // �������� ��� GTK-���������� � ����
    guint drawing_padding = 5;

    // ��� ������ - � ScrolledWindow
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

    // ������ �����, ������, ���� - � Notebook wtm_tabs
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
