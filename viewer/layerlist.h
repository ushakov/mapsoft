#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <gtkmm.h>
#include "layers/layer_map.h"
#include "layers/layer_wpt.h"
#include "layers/layer_trk.h"

#include "workplane.h"

/*********************************************************************
Three children of Gtk::TreeView are defined here: WptLL, TrkLL, MapLL

Interface:
    void add_layer (const boost::shared_ptr<LayerWPT> layer);
    void del_layer (const LayerWPT * L);
    void clear();
    bool upd_wp (Workplane & wp, int & d);
    bool upd_comm(LayerWPT * sel_layer=NULL, bool dir=true);

Columns:
    checked, comm, layer
    data -- used only as data storage

*********************************************************************/

class WptLLCols : public Gtk::TreeModelColumnRecord {
public:
    WptLLCols() {
	add(checked);
	add(comm);
	add(layer);
	add(data);
    }
    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<std::string> comm;
    Gtk::TreeModelColumn<boost::shared_ptr<LayerWPT> > layer;
    Gtk::TreeModelColumn<boost::shared_ptr<g_waypoint_list> > data;
};

class TrkLLCols : public Gtk::TreeModelColumnRecord {
public:
    TrkLLCols() {
	add(checked);
	add(comm);
	add(layer);
	add(data);
    }
    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<std::string> comm;
    Gtk::TreeModelColumn<boost::shared_ptr<LayerTRK> > layer;
    Gtk::TreeModelColumn<boost::shared_ptr<g_track> > data;
};

class MapLLCols : public Gtk::TreeModelColumnRecord {
public:
    MapLLCols() {
	add(checked);
	add(comm);
	add(weight);
	add(layer);
	add(data);
    }
    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<std::string> comm;
    Gtk::TreeModelColumn<Pango::Weight> weight;
    Gtk::TreeModelColumn<boost::shared_ptr<LayerMAP> > layer;
    Gtk::TreeModelColumn<boost::shared_ptr<g_map_list> > data;
};

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

    void add_layer (const boost::shared_ptr<LayerWPT> layer,
                    const boost::shared_ptr<g_waypoint_list> data) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
        // note: signal_row_changed() is emitted three times from here:
	row[columns.checked] = true;
	row[columns.comm]    = layer->get_data()->comm;
	row[columns.layer]   = layer;
        row[columns.data]    = data;
    }

    void del_layer (const LayerWPT * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerWPT> layer = (*i)[columns.layer];
        if (layer.get() != L) continue;
        store->erase(i);
        break;
      }
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
        boost::shared_ptr<LayerWPT> layer = (*i)[columns.layer];
        if (!layer) continue;
        // update visibility
        bool act = (*i)[columns.checked];
        if (wp.get_layer_active(layer.get()) != act){
          wp.set_layer_active(layer.get(), act);
          ret = true;
        }
        // update depth
        if (wp.get_layer_depth(layer.get()) != d){
          wp.set_layer_depth(layer.get(), d);
          ret = true;
        }
        d++;
      }
      return ret;
    }

    bool upd_comm(LayerWPT * sel_layer=NULL, bool dir=true){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
                string comm = (*i)[columns.comm];
        boost::shared_ptr<LayerWPT> layer = (*i)[columns.layer];
        if (!layer) continue;
        // select layer if sel_layer!=NULL
        if (sel_layer && sel_layer!=layer.get()) continue;
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

    void add_layer (const boost::shared_ptr<LayerTRK> layer,
                    const boost::shared_ptr<g_track> data) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
        // note: signal_row_changed() is emitted three times from here:
	row[columns.checked] = true;
	row[columns.comm]    = layer->get_data()->comm;
	row[columns.layer]   = layer;
        row[columns.data]    = data;
    }

    void del_layer (const LayerTRK * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerTRK> layer = (*i)[columns.layer];
        if (layer.get() != L) continue;
        store->erase(i);
        break;
      }
    }

    void clear() {store->clear();}

    bool upd_wp (Workplane & wp, int & d){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerTRK> layer = (*i)[columns.layer];
        if (!layer) continue;
        // update visibility
        bool act = (*i)[columns.checked];
        if (wp.get_layer_active(layer.get()) != act){
          wp.set_layer_active(layer.get(), act);
          ret = true;
        }
        // update depth
        if (wp.get_layer_depth(layer.get()) != d){
          wp.set_layer_depth(layer.get(), d);
          ret = true;
        }
        d++;
      }
      return ret;
    }

    bool upd_comm(LayerTRK * sel_layer=NULL, bool dir=true){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        string comm = (*i)[columns.comm];
        boost::shared_ptr<LayerTRK> layer = (*i)[columns.layer];
        if (!layer) continue;
        // select layer if sel_layer!=NULL
        if (sel_layer && sel_layer!=layer.get()) continue;
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

    void add_layer (const boost::shared_ptr<LayerMAP> layer,
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

    void del_layer (const LayerMAP * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerMAP> layer = (*i)[columns.layer];
        if (layer.get() != L) continue;
        store->erase(i);
        break;
      }
    }

    void clear() {store->clear();}

    bool upd_wp (Workplane & wp, int & d){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerMAP> layer = (*i)[columns.layer];
        if (!layer) continue;
        // update visibility
        bool act = (*i)[columns.checked];
        if (wp.get_layer_active(layer.get()) != act){
          wp.set_layer_active(layer.get(), act);
          ret = true;
        }
        // update depth
        if (wp.get_layer_depth(layer.get()) != d){
          wp.set_layer_depth(layer.get(), d);
          ret = true;
        }
        d++;
      }
      return ret;
    }

    bool upd_comm(LayerMAP * sel_layer=NULL, bool dir=true){
      bool ret=false;
      Gtk::TreeNodeChildren::const_iterator i;
      for (i = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerMAP> layer = (*i)[columns.layer];
        if (!layer) continue;
        // select layer if sel_layer!=NULL
        if (sel_layer && sel_layer!=layer.get()) continue;
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
