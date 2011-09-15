#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <gtkmm.h>
#include "layers/layer_geo.h"

class LayerListColumns : public Gtk::TreeModelColumnRecord {
public:
    LayerListColumns() {
	add(checked);
	add(text);
	add(layer);
	add(world);
    }

    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<Glib::ustring> text;
    Gtk::TreeModelColumn<LayerGeo *> layer;
    Gtk::TreeModelColumn<geo_data *> world;
};


class LayerList : public Gtk::TreeView {
public:
    LayerList () {
	store = Gtk::ListStore::create(columns);
	set_model(store);
	append_column_editable("V", columns.checked);
	append_column_editable("Layer", columns.text);
	set_enable_search(false);
        set_headers_visible(false);
        set_reorderable(true);
        dep_base=1;
    }

    void add_layer (LayerGeo * layer, geo_data * world, Glib::ustring name) {
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
        // note: signal_row_changed() is emitted three times from here:
	row[columns.layer] = layer;
	row[columns.world] = world;
	row[columns.checked] = true;
	row[columns.text] = name;
    }

    Glib::RefPtr<Gtk::ListStore> store;
    LayerListColumns columns;
    int get_dep_base() const{ return dep_base;}
    void set_dep_base(const int dep){ dep_base = dep;}

private:
    int dep_base;
};

#endif /* LAYERLIST_H */
