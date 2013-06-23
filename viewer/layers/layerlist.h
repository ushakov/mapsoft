#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <gtkmm.h>
#include "img_io/gobj_map.h"
#include "img_io/gobj_wpt.h"
#include "img_io/gobj_trk.h"

#include "../workplane.h"

/*********************************************************************
Three children of Gtk::TreeView are defined here: WptLL, TrkLL, MapLL

Interface:
    void add_gobj (const boost::shared_ptr<GObjWPT> layer);
    void remove_gobj (const GObjWPT * L);
    void clear();
    bool upd_wp (Workplane & wp, int & d);
    bool upd_comm(GObjWPT * sel_gobj=NULL, bool dir=true);

Columns:
    checked, comm, layer
    data -- used only as data storage

*********************************************************************/

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

typedef LayerTabCols<GObjWPT, g_waypoint_list> WptLLCols;
typedef LayerTabCols<GObjTRK, g_track>         TrkLLCols;
typedef LayerTabCols<GObjMAP, g_map_list>      MapLLCols;

/*********************************************************************/

class WptLL : public Gtk::TreeView {
public:
    WptLL () {
	store = Gtk::ListStore::create(columns);
	set_model(store);
	append_column_editable("V", columns.checked);
	append_column_editable("Layer", columns.comm);
	set_enable_search(false);
        set_headers_visible(false);
        set_reorderable(false);
    }

    void add_gobj (const boost::shared_ptr<GObjWPT> layer,
                    const boost::shared_ptr<g_waypoint_list> data) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
        // note: signal_row_changed() is emitted three times from here:
	row[columns.checked] = true;
	row[columns.comm]    = layer->get_data()->comm;
        row[columns.weight]  = Pango::WEIGHT_NORMAL;
	row[columns.layer]   = layer;
        row[columns.data]    = data;
    }

    void remove_gobj (const GObjWPT * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjWPT> gobj = (*i)[columns.layer];
        if (gobj.get() != L) continue;
        store->erase(i);
        break;
      }
    }

    void get_data(geo_data & world, bool visible){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
         if (visible && !(*i)[columns.checked]) continue;
         boost::shared_ptr<GObjWPT> layer = (*i)[columns.layer];
         world.wpts.push_back(*(layer->get_data()));
      }
    }

    /* find first active object */
    GObjWPT * find_gobj() const{
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjWPT> gobj = (*i)[columns.layer];
        return gobj.get();
      }
      return NULL;
    }

    /* find waypoint in all gobjs */
    int find_wpt(const iPoint & p, GObjWPT ** gobj, int radius = 3) const{
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjWPT> current_gobj = (*i)[columns.layer];
        *gobj = current_gobj.get();
        int d = current_gobj->find_waypoint(p, radius);
        if (d >= 0) return d;
      }
      *gobj = NULL;
      return -1;
    }

    /* find waypoints in a rectangular area */
    std::map<GObjWPT*, std::vector<int> > find_wpts(const iRect & r){
      std::map<GObjWPT*, std::vector<int> > ret;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjWPT> gobj = (*i)[columns.layer];
        std::vector<int> pts = gobj->find_waypoints(r);
        if (pts.size()>0)
          ret.insert(pair<GObjWPT*, std::vector<int> >(gobj.get(), pts));
      }
      return ret;
    }


    void clear() {store->clear();}

    // Update workplane data (visibility and depth)
    //   d is a starting depth of layers
    // Return: change
    // Depth of last layer+1 returned in d

    bool upd_wp (Workplane & wp, int & d){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjWPT> layer = (*i)[columns.layer];
        if (!layer) continue;
        // update visibility
        bool act = (*i)[columns.checked];
        if (wp.get_gobj_active(layer.get()) != act){
          wp.set_gobj_active(layer.get(), act);
          ret = true;
        }
        // update depth
        if (wp.get_gobj_depth(layer.get()) != d){
          wp.set_gobj_depth(layer.get(), d);
          ret = true;
        }
        d++;
      }
      return ret;
    }

    bool upd_comm(GObjWPT * sel_gobj=NULL, bool dir=true){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
                string comm = (*i)[columns.comm];
        boost::shared_ptr<GObjWPT> layer = (*i)[columns.layer];
        if (!layer) continue;
        // select layer if sel_gobj!=NULL
        if (sel_gobj && sel_gobj!=layer.get()) continue;
        if (comm!=layer->get_data()->comm){
          if (dir) layer->get_data()->comm = comm;
          else (*i)[columns.comm] = layer->get_data()->comm;
          ret = true;
        }
      }
      return ret;
    }

    Glib::RefPtr<Gtk::ListStore> store;
    WptLLCols columns;
};



class TrkLL : public Gtk::TreeView {
public:
    TrkLL () {
	store = Gtk::ListStore::create(columns);
	set_model(store);
	append_column_editable("V", columns.checked);
	append_column_editable("Layer", columns.comm);
	set_enable_search(false);
        set_headers_visible(false);
        set_reorderable(false);
    }

    void add_gobj (const boost::shared_ptr<GObjTRK> layer,
                    const boost::shared_ptr<g_track> data) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
        // note: signal_row_changed() is emitted three times from here:
	row[columns.checked] = true;
	row[columns.comm]    = layer->get_data()->comm;
        row[columns.weight]  = Pango::WEIGHT_NORMAL;
	row[columns.layer]   = layer;
        row[columns.data]    = data;
    }

    void remove_gobj (const GObjTRK * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjTRK> gobj = (*i)[columns.layer];
        if (gobj.get() != L) continue;
        store->erase(i);
        break;
      }
    }

    void get_data(geo_data & world, bool visible){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
         if (visible && !(*i)[columns.checked]) continue;
         boost::shared_ptr<GObjTRK> layer = (*i)[columns.layer];
         world.trks.push_back(*(layer->get_data()));
      }
    }

    /* find first active object */
    GObjTRK * find_gobj() const{
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjTRK> gobj = (*i)[columns.layer];
        return gobj.get();
      }
      return NULL;
    }

    /* find track points in a rectangular area */
    std::map<GObjTRK*, std::vector<int> > find_tpts(const iRect & r){
      std::map<GObjTRK*, std::vector<int> > ret;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjTRK> gobj = (*i)[columns.layer];
        std::vector<int> pts = gobj->find_trackpoints(r);
        if (pts.size()>0)
          ret.insert(pair<GObjTRK*, std::vector<int> >(gobj.get(), pts));
      }
      return ret;
    }

    /* find trackpoint in all gobjs */
    // segment=true: find track point, returns its number 0..size()-1
    // segment=true: find track segment, return its
    //               first point 0..size()-2
    int find_tpt(const iPoint & p, GObjTRK ** gobj,
                 const bool segment = false, int radius = 3) const{
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjTRK> current_gobj = (*i)[columns.layer];
        *gobj = current_gobj.get();
        int d;
        if (segment) d = current_gobj->find_track(p, radius);
        else d = current_gobj->find_trackpoint(p, radius);
        if (d >= 0) return d;
      }
      *gobj = NULL;
      return -1;
    }


    void clear() {store->clear();}

    bool upd_wp (Workplane & wp, int & d){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjTRK> layer = (*i)[columns.layer];
        if (!layer) continue;
        // update visibility
        bool act = (*i)[columns.checked];
        if (wp.get_gobj_active(layer.get()) != act){
          wp.set_gobj_active(layer.get(), act);
          ret = true;
        }
        // update depth
        if (wp.get_gobj_depth(layer.get()) != d){
          wp.set_gobj_depth(layer.get(), d);
          ret = true;
        }
        d++;
      }
      return ret;
    }

    bool upd_comm(GObjTRK * sel_gobj=NULL, bool dir=true){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        string comm = (*i)[columns.comm];
        boost::shared_ptr<GObjTRK> layer = (*i)[columns.layer];
        if (!layer) continue;
        // select layer if sel_gobj!=NULL
        if (sel_gobj && sel_gobj!=layer.get()) continue;
        if (comm!=layer->get_data()->comm){
          if (dir) layer->get_data()->comm = comm;
          else (*i)[columns.comm] = layer->get_data()->comm;
          ret = true;
        }
      }
      return ret;
    }

    Glib::RefPtr<Gtk::ListStore> store;
    TrkLLCols columns;
};

class MapLL : public Gtk::TreeView {
public:
    MapLL () {
	store = Gtk::ListStore::create(columns);
	set_model(store);
	append_column_editable("V", columns.checked);
	int comm_cell_n = append_column_editable("V", columns.comm);

        Gtk::TreeViewColumn* comm_column = get_column(comm_cell_n - 1);
        Gtk::CellRendererText* comm_cell =
          (Gtk::CellRendererText*)get_column_cell_renderer(comm_cell_n - 1);
        if (comm_column && comm_cell)
          comm_column->add_attribute(comm_cell->property_weight(), columns.weight);

	set_enable_search(false);
        set_headers_visible(false);
        set_reorderable(false);
    }

    void add_gobj (const boost::shared_ptr<GObjMAP> layer,
                    const boost::shared_ptr<g_map_list> data) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
	row[columns.checked] = true;
	row[columns.layer]   = layer;
	row[columns.data]    = data;
	if (layer->get_data()->size() == 1){
          row[columns.comm] = (*layer->get_data())[0].comm;
          row[columns.weight] = Pango::WEIGHT_NORMAL;
        }
	else{
          row[columns.comm] = layer->get_data()->comm;
          row[columns.weight] = Pango::WEIGHT_BOLD;
        }
    }

    void remove_gobj (const GObjMAP * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjMAP> gobj = (*i)[columns.layer];
        if (gobj.get() != L) continue;
        store->erase(i);
        break;
      }
    }

    void get_data(geo_data & world, bool visible){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
         if (visible && !(*i)[columns.checked]) continue;
         boost::shared_ptr<GObjMAP> layer = (*i)[columns.layer];
         world.maps.push_back(*(layer->get_data()));
      }
    }

    /* find first active object */
    GObjMAP * find_gobj() const{
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjMAP> gobj = (*i)[columns.layer];
        return gobj.get();
      }
      return NULL;
    }

    /* find map */
    int find_map(const iPoint & p, GObjMAP ** gobj) const{
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        if (!(*i)[columns.checked]) continue;
        boost::shared_ptr<GObjMAP> current_gobj = (*i)[columns.layer];
        *gobj = current_gobj.get();
        int d = current_gobj->find_map(p);
        if (d >= 0) return d;
      }
      *gobj = NULL;
      return -1;
    }

    void clear() {store->clear();}

    bool upd_wp (Workplane & wp, int & d){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjMAP> layer = (*i)[columns.layer];
        if (!layer) continue;
        // update visibility
        bool act = (*i)[columns.checked];
        if (wp.get_gobj_active(layer.get()) != act){
          wp.set_gobj_active(layer.get(), act);
          ret = true;
        }
        // update depth
        if (wp.get_gobj_depth(layer.get()) != d){
          wp.set_gobj_depth(layer.get(), d);
          ret = true;
        }
        d++;
      }
      return ret;
    }

    bool upd_comm(GObjMAP * sel_gobj=NULL, bool dir=true){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<GObjMAP> layer = (*i)[columns.layer];
        if (!layer) continue;
        // select layer if sel_gobj!=NULL
        if (sel_gobj && sel_gobj!=layer.get()) continue;
        // update comment
        string comm = (*i)[columns.comm];
        g_map_list * ml = layer->get_data();
        if (ml->size()==1){
          if (comm!=(*ml)[0].comm){
            if (dir) (*ml)[0].comm = comm;
            else (*i)[columns.comm] = (*ml)[0].comm;
            ret = true;
          }
        }
        else {
          if (comm!=ml->comm){
            if (dir) ml->comm = comm;
            else (*i)[columns.comm] = ml->comm;
            ret = true;
          }
        }
      }
      return ret;
    }

    Glib::RefPtr<Gtk::ListStore> store;
    MapLLCols columns;
};

/*********************************************************************/

#endif /* LAYERLIST_H */
