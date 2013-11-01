#ifndef VIEWER_PANEL_H
#define VIEWER_PANEL_H

#include <gtkmm.h>
#include "workplane.h"

/* TreeModelColumnRecord for WPT/TRK/MAP panels. */

template <typename Tl, typename Td>
class LayerTabCols : public Gtk::TreeModelColumnRecord {
public:
    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<std::string> comm;
    Gtk::TreeModelColumn<Pango::Weight> weight;
    Gtk::TreeModelColumn<boost::shared_ptr<Tl> > layer;
    Gtk::TreeModelColumn<boost::shared_ptr<Td> > data;

    LayerTabCols() {
      add(checked); add(comm); add(weight); add(layer); add(data);
    }
};

/* Abstract class for WPT/TRK/MAP panels,
 * child of Gtk::TreeView and Workplane.
 */

template <typename Tl, typename Td>
class Panel : public Gtk::TreeView, public Workplane {
public:

  Panel() {
    store = Gtk::ListStore::create(columns);
    set_model(store);
    append_column_editable("V", columns.checked);
    int comm_cell_n = append_column_editable("V", columns.comm);

    Gtk::TreeViewColumn* comm_column = get_column(comm_cell_n - 1);
    Gtk::CellRendererText* comm_cell =
      (Gtk::CellRendererText*)get_column_cell_renderer(comm_cell_n - 1);
    if (comm_column && comm_cell)
      comm_column->add_attribute(
        comm_cell->property_weight(), columns.weight);

    set_enable_search(false);
    set_headers_visible(false);
    set_reorderable(false);
  }

  virtual void add(const boost::shared_ptr<Td> data) = 0;

  /* Remove all objects */
  void remove_all() {
    store->clear();
    Workplane::clear();
    Workplane::signal_refresh.emit();
  }

  /* Remove selected object */
  void remove_selected(){
    Gtk::TreeModel::iterator it = get_selection()->get_selected();
    if (!it) return;
    Workplane::remove_gobj(it->get_value(columns.layer).get());
    store->erase(it);
    upd_wp();
  }

  /* Remove object */
  void remove(Tl * L){
    Gtk::TreeNodeChildren::const_iterator i;
    for (i  = store->children().begin();
         i != store->children().end(); i++){
      boost::shared_ptr<Tl> gobj = (*i)[columns.layer];
      if (gobj.get() != L) continue;
      store->erase(i);
      break;
    }
    Workplane::remove_gobj(L);
    upd_wp();
  }

  /* Hide/Show all */
  void show_all(bool show=true){
    Gtk::TreeNodeChildren::const_iterator i;
    for (i  = store->children().begin();
         i != store->children().end(); i++){
      (*i)[columns.checked] = show;
    }
    upd_wp();
  }

  /* Invert visibility */
  void invert_all(){
    Gtk::TreeNodeChildren::const_iterator i;
    for (i  = store->children().begin();
         i != store->children().end(); i++){
      (*i)[columns.checked] = !(*i)[columns.checked];
    }
    upd_wp();
  }

  /* Join visible/all objects */
  void join(bool visible) {
    boost::shared_ptr<Td> newd(new Td);
    Gtk::TreeNodeChildren::const_iterator
      i = store->children().begin();
    while (i != store->children().end()){
      if (visible && !(*i)[columns.checked]) {
        i++; continue;
      }

      Td * curr = i->get_value(columns.layer)->get_data();
      if (!curr){ i++; continue; }

      newd->insert(newd->end(), curr->begin(), curr->end());
      if (newd->size()) newd->comm = "JOIN";
      else newd->comm = curr->comm;

      Workplane::remove_gobj(i->get_value(columns.layer).get());
      i = store->erase(i);
    }
    if (newd->size()) add(newd);
  }

  /* Find selected object */
  Tl* find_selected() {
    Gtk::TreeModel::iterator it =
      get_selection()->get_selected();
    if (!it) return NULL;
    return it->get_value(columns.layer).get();
  }

  /* Find first visible object */
  Tl* find_first() const {
    Gtk::TreeNodeChildren::const_iterator i;
    for (i  = store->children().begin();
         i != store->children().end(); i++){
      if (!(*i)[columns.checked]) continue;
      boost::shared_ptr<Tl> gobj = (*i)[columns.layer];
      return gobj.get();
    }
    return NULL;
  }

  /* Get all/visible data */
  void get_data(geo_data & world, bool visible) const {
    Gtk::TreeNodeChildren::const_iterator i;
    for (i  = store->children().begin();
         i != store->children().end(); i++){
       if (visible && !(*i)[columns.checked]) continue;
       boost::shared_ptr<Tl> layer = (*i)[columns.layer];
       world.push_back(*(layer->get_data()));
    }
  }

  /* Get selected data */
  void get_sel_data(geo_data & world) {
    Gtk::TreeNodeChildren::iterator i =
      get_selection()->get_selected();
    if (!i) return;
    boost::shared_ptr<Tl> layer = (*i)[columns.layer];
    world.push_back(*(layer->get_data()));
  }

  /* move object (up/down/top/bottom) */
  void move(bool up, bool onestep){
    Gtk::TreeModel::iterator it1, it2;
    it1 = it2 = get_selection()->get_selected();
    if (!it1) return;
    if (up && (it1 == store->children().begin())) return;

    if (!onestep){
      if (up) it2--; else it2++;
      if (!it2) return;
    }
    else{
      if (up) it2 = store->children().begin();
      else {it2 = store->children().end(); it2--;}
    }
    store->iter_swap(it1, it2);
    upd_wp();
  }

  /* Update visibility and depths of Workplane objects
     according to TreeView */
  bool upd_wp(){
    bool ret=false;
    Gtk::TreeNodeChildren::const_iterator i;
    int d=1;
    for (i = store->children().begin();
         i != store->children().end(); i++){
      boost::shared_ptr<Tl> layer = (*i)[columns.layer];
      if (!layer) continue;
      // update visibility
      bool act = (*i)[columns.checked];
      if (get_gobj_active(layer.get()) != act){
        set_gobj_active(layer.get(), act);
        ret = true;
      }
      // update depth
      if (get_gobj_depth(layer.get()) != d){
        set_gobj_depth(layer.get(), d);
        ret = true;
      }
      d++;
    }
    Workplane::signal_refresh.emit();
    return ret;
  }

  // get/set options
  Options & get_opt(){ return opts;}

  void set_opt(const Options & o){
    opts = o;
    Gtk::TreeNodeChildren::const_iterator i;
    for (i = store->children().begin();
         i != store->children().end(); i++){
      i->get_value(columns.layer)->set_opt(o);
      refresh_gobj(i->get_value(columns.layer).get(), false);
    }
    Workplane::signal_refresh.emit();
  }

  /* */
  Glib::RefPtr<Gtk::ListStore> store;
  LayerTabCols<Tl, Td> columns;
  Options opts;
};

#endif
