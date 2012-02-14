#ifndef DIALOGS_MAP_H
#define DIALOGS_MAP_H

#include <gtkmm.h>
#include <geo/geo_data.h>

// dialog for Map actions
class DlgMap : public Gtk::Dialog{
    Gtk::Entry *comm, *file;
  public:
    DlgMap();
    void dlg2map(g_map * map) const;
    void map2dlg(const g_map * map);
};

#endif
