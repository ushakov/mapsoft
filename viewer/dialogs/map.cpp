#include "map.h"
#include <cassert>

DlgMap::DlgMap(){
  add_button (Gtk::Stock::OK,     Gtk::RESPONSE_OK);
  add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  signal_response().connect(
      sigc::hide(sigc::mem_fun(this, &DlgMap::hide_all)));

  // Labels
  Gtk::Label *l_comm = manage(new Gtk::Label("Map name:",  Gtk::ALIGN_RIGHT));
  Gtk::Label *l_file = manage(new Gtk::Label("File name:", Gtk::ALIGN_RIGHT));
  l_comm->set_padding(3,0);
  l_file->set_padding(3,0);

  // Entries
  comm  = manage(new Gtk::Entry);
  file  = manage(new Gtk::Entry);

  // Table
  Gtk::Table *table = manage(new Gtk::Table(2,2));
            //  widget    l  r  t  b  x       y
  table->attach(*l_comm,  0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*comm,    1, 2, 0, 1, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*l_file,  0, 1, 1, 2, Gtk::FILL, Gtk::SHRINK, 3, 3);
  table->attach(*file,    1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::SHRINK, 3, 3);

  get_vbox()->add(*table);
}

void
DlgMap::dlg2map(g_map * map) const{
  assert(map);
  map->comm = comm->get_text();
  map->file = file->get_text();
}
void
DlgMap::map2dlg(const g_map * map){
  assert(map);
  comm->set_text(map->comm);
  file->set_text(map->file);
}
