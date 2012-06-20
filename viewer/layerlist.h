#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <gtkmm.h>
#include "layers/layer_geomap.h"
#include "layers/layer_wpt.h"
#include "layers/layer_trk.h"

/*********************************************************************/

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
    Gtk::TreeModelColumn<boost::shared_ptr<LayerGeoMap> > layer;
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
	row[columns.comm]    = data->comm;
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
	row[columns.comm]    = data->comm;
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

    void add_layer (const boost::shared_ptr<LayerGeoMap> layer,
                    const boost::shared_ptr<g_map_list> data) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
	row[columns.checked] = true;
	if (data->size() == 1){
          row[columns.comm] = (*data)[0].comm;
          row[columns.weight] = Pango::WEIGHT_NORMAL;
        }
	else{
          row[columns.comm] = data->comm;
          row[columns.weight] = Pango::WEIGHT_BOLD;
        }
	row[columns.layer]   = layer;
	row[columns.data]    = data;
    }

    void del_layer (const LayerGeoMap * L){
      Gtk::TreeNodeChildren::const_iterator i;
      for (i  = store->children().begin();
           i != store->children().end(); i++){
        boost::shared_ptr<LayerGeoMap> layer = (*i)[columns.layer];
        if (layer.get() != L) continue;
        store->erase(i);
        break;
      }
    }

    Glib::RefPtr<Gtk::ListStore> store;
    MapLLCols columns;
};

/*********************************************************************/

#endif /* LAYERLIST_H */
