#ifndef LIST_STORE_FOR_WPT_H
#define LIST_STORE_FOR_WPT_H

// преобразование набора точек в GTK::ListStore и обратно

#include <gtkmm.h>
#include "../libgeo/geo_data.h"

//класс, описывающий поля точки в Gtk::TreeModel
class WPT_ListColumns : public Gtk::TreeModelColumnRecord {
  public:
    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> comm;
    Gtk::TreeModelColumn<Time>          t;
    Gtk::TreeModelColumn<double>        x;
    Gtk::TreeModelColumn<double>        y;
    Gtk::TreeModelColumn<double>        z;
    Gtk::TreeModelColumn<double>        prox_dist;
    Gtk::TreeModelColumn<wptSymb>       symb;
    Gtk::TreeModelColumn<int>           displ;
    Gtk::TreeModelColumn<wptMapDispl>   map_displ;
    Gtk::TreeModelColumn<wptPtDir>      pt_dir;
    Gtk::TreeModelColumn<int>           font_size;
    Gtk::TreeModelColumn<int>           font_style;
    Gtk::TreeModelColumn<int>           size;
    Gtk::TreeModelColumn<Color>         color;
    Gtk::TreeModelColumn<Color>         bgcolor;

    WPT_ListColumns() {
      add(name);
      add(comm);
      add(t);
      add(x);
      add(y);
      add(z);
      add(prox_dist);
      add(symb);
      add(displ);
      add(map_displ);
      add(pt_dir);
      add(font_size);
      add(font_style);
      add(size);
      add(color);
      add(bgcolor);
    }
};

// процедуры преобразования Gtk::ListStore <-> g_waypoint_list
Glib::RefPtr<Gtk::ListStore> gtk_liststore_from_wpts(const g_waypoint_list & w){
  WPT_ListColumns columns;
  Glib::RefPtr<Gtk::ListStore>  store   = Gtk::ListStore::create(columns);
  for (g_waypoint_list::const_iterator pt = w.begin(); pt!=w.end(); pt++){
    Gtk::TreeModel::iterator it = store->append();
    (*it)[columns.name]       = pt->name;
    (*it)[columns.comm]       = pt->comm;
    (*it)[columns.t]          = pt->t;
    (*it)[columns.x]          = pt->x;
    (*it)[columns.y]          = pt->y;
    (*it)[columns.z]          = pt->z;// if (fabs(pt->z)>1e4) (*it)[columns.z] = double(nan);
    (*it)[columns.prox_dist]  = pt->prox_dist;
    (*it)[columns.symb]       = pt->symb;
    (*it)[columns.displ]      = pt->displ;
    (*it)[columns.map_displ]  = pt->map_displ;
    (*it)[columns.pt_dir]     = pt->pt_dir;
    (*it)[columns.font_size]  = pt->font_size;
    (*it)[columns.font_style] = pt->font_style;
    (*it)[columns.size]       = pt->size;
    (*it)[columns.color]      = pt->color;
    (*it)[columns.bgcolor]    = pt->bgcolor;
  }
  return store;
}
g_waypoint_list gtk_liststore_to_wpts(const Glib::RefPtr<Gtk::ListStore> & ls){
  g_waypoint_list ret;
  WPT_ListColumns columns;
  for (Gtk::TreeModel::Children::iterator it = ls->children().begin(); it!=ls->children().end(); it++){
    g_waypoint pt;
    Glib::ustring s;
    s             = (*it)[columns.name]; pt.name=s;
    s             = (*it)[columns.comm]; pt.comm=s;
    pt.t          = (*it)[columns.t];
    pt.x          = (*it)[columns.x];
    pt.y          = (*it)[columns.y];
    pt.z          = (*it)[columns.z]; 
    pt.prox_dist  = (*it)[columns.prox_dist];
    pt.symb       = (*it)[columns.symb];
    pt.displ      = (*it)[columns.displ];
    pt.map_displ  = (*it)[columns.map_displ];
    pt.pt_dir     = (*it)[columns.pt_dir];
    pt.font_size  = (*it)[columns.font_size];
    pt.font_style = (*it)[columns.font_style];
    pt.size       = (*it)[columns.size];
    pt.color      = (*it)[columns.color];
    pt.bgcolor    = (*it)[columns.bgcolor];
    ret.push_back(pt);
  }
  return ret;
}

// класс TreeView для показа и редактирования waypoints
class WPT_List : public Gtk::TreeView{
  Glib::RefPtr<Gtk::ListStore> store;
  WPT_ListColumns columns;

  public:
  WPT_List(const g_waypoint_list & w){
    store = gtk_liststore_from_wpts(w);
    set_model(store);
    append_column_editable("name", columns.name);
    append_column_editable("comm", columns.comm);
    append_column_numeric_editable("lon",  columns.x, "%10.6f");
    append_column_numeric_editable("lat",  columns.y, "%8.6f");
    append_column_numeric_editable("h",    columns.z, "%6.1f");
    append_column_editable("time",       columns.t);
    append_column_editable("prox_dist",  columns.prox_dist);
    append_column_editable("symb",       columns.symb);
    append_column_editable("displ",      columns.displ);
    append_column_editable("map_displ",  columns.map_displ);
    append_column_editable("pt_dir",     columns.pt_dir);
    append_column_editable("font_size",  columns.font_size);
    append_column_editable("font_style", columns.font_style);
    append_column_editable("size",       columns.size);
    append_column_editable("color",      columns.color);
    append_column_editable("bgcolor",    columns.bgcolor);
    set_headers_visible(true);
    set_enable_search(false);
    set_reorderable();
  }
};

#endif
