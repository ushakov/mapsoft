#ifndef MAPVIEW_H
#define MAPVIEW_H

// �����, ��������� ��� ���������� ������ �
// ����������� �� � ���� ����
//
// ������ �� ���� ����� ������ ���� ActionManager'��,
// ���� ��� ������� �� ����� ������������ ������.

#include <gtkmm.h>
#include <boost/shared_ptr.hpp>

#include "mapview_data.h"

#include "rubber.h"
#include "viewer.h"
#include "viewer_am.h"

#include "data_list.h"
#include "data_list_am.h"

#include "menubar.h"
#include "statusbar.h"

class Mapview : public Gtk::Window{
  public:
    boost::shared_ptr<MapviewData> mapview_data; // ��������� �� ����� ���������� � workplane'��
    boost::shared_ptr<Rubber>      rubber;       // "������" - xor-�����, ����������� � �����
    boost::shared_ptr<Viewer>      viewer;       // ������������ gtk-widget, ������������ workplane � rubber
    boost::shared_ptr<ViewerAM>    viewer_am;    // ActionManager ��� viewer'�
    boost::shared_ptr<DataList>    data_list;    // ������������ gtk-widget, ������������ mapview_data
    boost::shared_ptr<DataListAM>  data_list_am; // ActionManager ��� data_list'�
    boost::shared_ptr<MenuBar>     menubar;      // ���� (�������, ��� �� ������ ���� ��������� � ����� ������ :))
    boost::shared_ptr<StatusBar>   statusbar;    // gtk-widget ������������ ������ �����

  Mapview(){
    // �������� ��� ����������
    mapview_data.reset(new MapviewData());
    rubber.reset(new Rubber());
    viewer.reset(new Viewer(mapview_data->workplane, rubber));
    viewer_am.reset(new ViewerAM(viewer, this));
    data_list.reset(new DataList(mapview_data));
    data_list_am.reset(new DataListAM(data_list, this));
    menubar.reset(new MenuBar());
    statusbar.reset(new StatusBar());

    // �������� ��� GTK-���������� � ����
    guint drawing_padding = 5;

    // ScrolledWindow scrw <- data_list
    Gtk::ScrolledWindow * scrw = manage(new Gtk::ScrolledWindow);
    scrw->add(*data_list);
    scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrw->set_size_request(128,-1);

    // HPaned paned <- viewer + scrw
    Gtk::HPaned * paned = manage(new Gtk::HPaned);
    paned->pack1(*viewer, Gtk::EXPAND | Gtk::FILL);
    paned->pack2(*scrw, Gtk::FILL);

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
