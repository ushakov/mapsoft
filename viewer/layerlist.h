#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <gtkmm.h>
#include <layer.h>

class LayerListColumns : public Gtk::TreeModelColumnRecord {
public:
    LayerListColumns() {
	add(checked);
	add(depth);
	add(text);
	add(layer);
    }
    
    Gtk::TreeModelColumn<bool> checked;
    Gtk::TreeModelColumn<int> depth;
    Gtk::TreeModelColumn<Glib::ustring> text;
    Gtk::TreeModelColumn<Layer *> layer;
};


class LayerList : public Gtk::TreeView {
public:
    LayerList () {
	store = Gtk::ListStore::create(columns);
	set_model(store);
	append_column_editable("V", columns.checked);
	append_column_editable("D", columns.depth);
	append_column_editable("Layer", columns.text);
	store->set_sort_column(columns.depth, Gtk::SORT_ASCENDING);
	set_enable_search(false);
    }

    void add_layer (Layer * layer, int depth, Glib::ustring name) {
	std::cout << "LL: add layer " << layer << " at depth " << depth << " named " << name << std::endl;
	Gtk::TreeModel::iterator it = store->append();
	Gtk::TreeModel::Row row = *it;
	row[columns.checked] = true;
	row[columns.depth] = depth;
	row[columns.text] = name;
	row[columns.layer] = layer;
    }

    Glib::RefPtr<Gtk::ListStore> store;
    LayerListColumns columns;
};

#endif /* LAYERLIST_H */
