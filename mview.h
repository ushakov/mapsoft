#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <sys/time.h>
#include <boost/shared_ptr.hpp>
#include <gtkmm.h>

#include <libgeo_io/io.h>
#include <viewer/workplane.h>
#include <viewer/viewer.h>
#include <layers/layer_geomap.h>
#include <layers/layer_geodata.h>
#include <viewer/layerlist.h>

//#include <viewer/action_manager.h>
#include <utils/log.h>

class MapviewState {
public:
    boost::shared_ptr<Workplane> workplane;
    boost::shared_ptr<Rubber>    rubber;
	
    std::vector<boost::shared_ptr<LayerGeoMap> > map_layers;
    std::vector<boost::shared_ptr<LayerGeoData> > data_layers;
    std::vector<boost::shared_ptr<geo_data> > data;
};

class Mapview : public Gtk::Window {
public:
    Mapview () : have_reference(false) {
	// window initialization
	signal_delete_event().connect (sigc::mem_fun (this, &Mapview::on_delete));
	set_default_size(640,480);

	state.workplane.reset(new Workplane(256));
	state.rubber.reset(new Rubber);
	viewer.reset(new Viewer(state.workplane, state.rubber));


	add (*viewer);

	signal_key_press_event().connect (sigc::mem_fun (this, &Mapview::on_keypress));

	// connect events from layer list
//	layer_list.store->signal_row_changed().connect (sigc::mem_fun (this, &Mapview::layer_edited));

	// connect events from viewer
	viewer->signal_motion_notify_event().connect (sigc::mem_fun (this, &Mapview::pointer_moved));
	viewer->signal_button_press_event().connect (sigc::mem_fun (this, &Mapview::mouse_button_pressed));
	viewer->signal_button_release_event().connect (sigc::mem_fun (this, &Mapview::mouse_button_released));

	show_all();
    }

    void load_file(std::string selected_filename) {
        g_print ("Loading: %s\n", selected_filename.c_str());
	boost::shared_ptr<geo_data> world (new geo_data);
	state.data.push_back(world);

        io::in(selected_filename, *(world.get()), Options());
        LOG() << "Loaded " << selected_filename << " to world at " << world.get();

        if (!have_reference){ reference = convs::mymap(*world.get()); have_reference = true; }

        if (world->maps.size() > 0) {
            // we are loading maps: if we already have reference, use it
            boost::shared_ptr<LayerGeoMap> map_layer(new LayerGeoMap(world.get()));
            map_layer->set_ref(reference);
            state.map_layers.push_back(map_layer);
            add_layer(map_layer.get(), 100, "Maps: " + selected_filename);
            viewer->set_window_origin((map_layer->range().TLC() + map_layer->range().BRC())/2);
            g_print ("Setting origin: %d %d\n", map_layer->range().x, map_layer->range().y);
        }

        if (world->wpts.size() > 0 || world->trks.size() > 0) {
            // we are loading geodata: if we already have reference, use it

            boost::shared_ptr<LayerGeoData> layer_gd(new LayerGeoData(world.get()));
            layer_gd->set_ref(reference);
            state.data_layers.push_back(layer_gd);
            add_layer(layer_gd.get(), 0, "Data: " + selected_filename);
            viewer->set_window_origin(layer_gd->range().TLC() - viewer->get_window_size()/2);
            g_print ("Setting origin: %d %d\n", layer_gd->range().x, layer_gd->range().y);
        }
        refresh();
    }

    void layer_edited (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	VLOG(2) << "layer_edited at " << path.to_string();
	Gtk::TreeModel::Row row = *iter;
	bool need_refresh = false;
	
	Layer * layer = row[layer_list.columns.layer];
	if (!layer) return;
	int new_depth = row[layer_list.columns.depth];
	if (state.workplane->get_layer_depth (layer) != new_depth) {
	    state.workplane->set_layer_depth (layer, new_depth);
	    need_refresh = true;
	}

	int new_active = row[layer_list.columns.checked];
	if (new_active != state.workplane->get_layer_active (layer)) {
	    state.workplane->set_layer_active (layer, new_active);
	    need_refresh = true;
	}

	if (need_refresh) {
	    refresh();
	}
    }

    gboolean on_delete(GdkEventAny * e) {
	g_print ("Exiting...\n");
	hide();
	return true;
    }

    gboolean on_keypress(GdkEventKey * event) {

	switch (event->keyval) {
	case 43:
	case 65451: // +
	{
	    Point<int> wcenter = viewer->get_window_origin() + viewer->get_window_size()/2;
            Point<int> origin = wcenter*2 - viewer->get_window_size()/2;
            viewer->set_window_origin(origin);
	    (*state.workplane)*=2;
	    return true;
	}
	case 45:
	case 65453: // -
	{
	    Point<int> wcenter = viewer->get_window_origin() + viewer->get_window_size()/2;
            Point<int> origin = wcenter/2 - viewer->get_window_size()/2;
            viewer->set_window_origin(origin);
	    (*state.workplane)/=2;

	    return true;
	}
	case 'r':
	case 'R': // refresh
	{
	    viewer->refresh();
	    return true;
	}
	}
	return false;
    }

    void add_layer (Layer * layer, int depth, Glib::ustring name) {
	state.workplane->add_layer(layer, depth);
	layer_list.add_layer(layer, depth, name);
    }

    void refresh () {
	viewer->refresh();
    }

    virtual bool
    mouse_button_pressed (GdkEventButton * event) {
	VLOG(2) << "press: " << event->x << "," << event->y << " " << event->button;
	if (event->button == 1) {
	    drag_pos = Point<int> ((int)event->x, (int)event->y);
	    gettimeofday (&click_started, NULL);
	    return true;
	}
    }

    virtual bool
    mouse_button_released (GdkEventButton * event) {
	VLOG(2) << "release: " << event->x << "," << event->y << " " << event->button;
	if (event->button == 1) {
	    struct timeval click_ended;
	    gettimeofday (&click_ended, NULL);
	    int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 + (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
	    if (d < 250) {
		Point<int> p(int(event->x), int(event->y));
		p += viewer->get_window_origin();
		VLOG(2) << "click at: " << p.x << "," << p.y << " " << event->button;
		return true;
	    }
	}
    }

    virtual bool
    pointer_moved (GdkEventMotion * event) {
	Point<int> pos ((int) event->x, (int) event->y);
	VLOG(2) << "motion: " << pos << (event->is_hint? " hint ":"");

	if (!(event->state & Gdk::BUTTON1_MASK) || !event->is_hint) return false;

	Point<int> shift = pos - drag_pos;
	Point<int> window_origin = viewer->get_window_origin();
	window_origin -= shift;
	viewer->set_window_origin(window_origin);
	drag_pos = pos;

	// ask for more events
	get_pointer(pos.x, pos.y);
	return true;
    }

    virtual ~Mapview() {
	state.workplane.reset();
	viewer.reset();
    }

private:
    boost::shared_ptr<Viewer> viewer;
    MapviewState state;

    LayerList layer_list;
    Gtk::FileSelection file_sel_load;
    Gtk::FileSelection file_sel_save;
    Gtk::Widget * menubar;

    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;

    g_map reference;
    bool have_reference;

    Point<int> drag_pos;

    Gtk::RadioAction::Group mode_group;

    struct timeval click_started;
};


#endif /* MAPVIEW_H */
