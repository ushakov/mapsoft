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
    }

    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<Glib::ustring> text;
    Gtk::TreeModelColumn<LayerGeo *> layer;
};


class LayerList : public Gtk::TreeView {
public:
    LayerList () {
	store = Gtk::ListStore::create(columns);
	set_model(store);
	append_column_editable("V", columns.checked);
	append_column("Layer", columns.text);
	set_enable_search(false);
        set_headers_visible(false);
        set_reorderable(true);
        dep_base=1;
        num=0;
    }

    void add_layer (LayerGeo * layer, Glib::ustring name) {
        num++;
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
        // note: signal_row_changed() is emitted three times from here:
	row[columns.layer] = layer;
	row[columns.checked] = true;
	row[columns.text] = name;
    }

    Glib::RefPtr<Gtk::ListStore> store;
    LayerListColumns columns;
    int get_dep_base() const{ return dep_base;}
    void set_dep_base(const int dep){ dep_base = dep;}
    int size() const {return num;}

    void on_rows_reordered (const Gtk::TreeModel::Path& path,
              const Gtk::TreeModel::iterator& iter, int* new_order){
      std::cerr << "LE>> " << path.front() << "\n";
        for (int i = 0; i<num; i++){
          if (new_order!= NULL) std::cerr << " O: " << new_order[i] << "\n";
        }
    }

private:
    int dep_base;
    int num;
};

#endif /* LAYERLIST_H */
