#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <sys/time.h>
#include <boost/shared_ptr.hpp>
#include <gtkmm.h>

#include "../libgeo_io/io.h"
#include "../viewer/workplane.h"
#include "../viewer/viewer.h"
#include "../layers/layer_geomap.h"
#include "../layers/layer_geodata.h"
#include "../viewer/layerlist.h"
#include "../viewer/generic_dialog.h"

#include "../viewer/action_manager.h"
#include "../utils/log.h"

class MapviewState {
public:
    Gtk::Statusbar statusbar;
    std::vector<boost::shared_ptr<LayerGeoMap> > map_layers;
    std::vector<boost::shared_ptr<LayerGeoData> > data_layers;
    std::vector<boost::shared_ptr<geo_data> > data;
};

class Mapview : public Gtk::Window {
public:
    Mapview () :
	file_sel_load ("Load file:"),
	file_sel_save ("Save as:"),
	have_reference(false)
    {
	// window initialization
	signal_delete_event().connect (sigc::mem_fun (this, &Mapview::on_delete));
	set_default_size(640,480);

	action_manager.reset (new ActionManager (&state, &viewer));

	//load file selector
	file_sel_load.get_ok_button()->signal_clicked().connect (sigc::mem_fun (this, &Mapview::load_file_sel));
	file_sel_load.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));
	file_sel_load.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));
	
 	//save file selector
 	file_sel_save.get_ok_button()->signal_clicked().connect (sigc::mem_fun (this, &Mapview::save_file_sel));
 	file_sel_save.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));
 	file_sel_save.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));

        viewer.signal_button_press_event().connect (sigc::mem_fun (this, &Mapview::on_button_press));
        viewer.signal_button_release_event().connect (sigc::mem_fun (this, &Mapview::on_button_release));
        signal_key_press_event().connect (sigc::mem_fun (this, &Mapview::on_key_press));
	
	/***************************************/
	//start building menus

	actions = Gtk::ActionGroup::create();
	actions->add(Gtk::Action::create("MenuFile", "_File"));
	actions->add(Gtk::Action::create("MenuGeodata", "_Geodata"));
	actions->add(Gtk::Action::create("MenuHelp", "_Help"));
	
	actions->add(Gtk::Action::create("Add", Gtk::Stock::ADD), sigc::mem_fun(file_sel_load, &Gtk::Widget::show));
	actions->add(Gtk::Action::create("Clear", Gtk::Stock::CLEAR));
	actions->add(Gtk::Action::create("Save", Gtk::Stock::SAVE), sigc::mem_fun(file_sel_save, &Gtk::Widget::show));
	actions->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT), sigc::mem_fun(this, &Gtk::Widget::hide));

	// make all modes!

	actions->add(Gtk::Action::create("MenuModes", "_Modes"));
	std::vector<std::string> modes = action_manager->get_mode_list();
	for (int m = 0; m < modes.size(); ++m) {
	    Glib::RefPtr<Gtk::RadioAction> mode_action =
		Gtk::RadioAction::create(mode_group,
					 "Mode" + boost::lexical_cast<std::string>(m),
					 modes[m]);
	    actions->add(mode_action, sigc::bind (sigc::mem_fun(*this, &Mapview::on_mode_change), m));
	}
	
	ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(actions);
	add_accel_group(ui_manager->get_accel_group());

	Glib::ustring ui =
	    "<ui>"
	    "  <menubar name='MenuBar'>"
	    "    <menu action='MenuFile'>"
	    "      <menuitem action='Quit'/>"
	    "    </menu>"
	    "    <menu action='MenuGeodata'>"
	    "      <menuitem action='Add'/>"
	    "      <menuitem action='Save'/>"
	    "      <menuitem action='Clear'/>"
	    "    </menu>"
	    "    <menu action='MenuHelp'>"
	    "    </menu>"
	    "    <menu action='MenuModes'>";
	for (int m = 0; m < modes.size(); ++m) {
	    ui += "<menuitem action='Mode"
		+ boost::lexical_cast<std::string>(m) + "'/>";
	}
	ui +=
	    "    </menu>"
	    "  </menubar>"
	    "</ui>";
	ui_manager->add_ui_from_string(ui);
	menubar = ui_manager->get_widget("/MenuBar");
	
	state.statusbar.push("Welcome to mapsoft viewer!",0);
	
	guint drawing_padding = 5;
	
	Gtk::VBox * vbox = manage(new Gtk::VBox);
	vbox->pack_start(*menubar, false, true, 0);
	
	Gtk::HPaned * paned = manage(new Gtk::HPaned);
	paned->pack1(viewer, Gtk::EXPAND | Gtk::FILL);
	
	Gtk::ScrolledWindow * scrw = manage(new Gtk::ScrolledWindow);
	scrw->add(layer_list);
	scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrw->set_size_request(128,-1);
	paned->pack2(*scrw, Gtk::FILL);

	vbox->pack_start(*paned, true, true, drawing_padding);
	vbox->pack_start(state.statusbar, false, true, 0);
	add (*vbox);

	// connect events from layer list
// 	layer_list.add_events (Gdk::BUTTON_PRESS_MASK);
	layer_list.store->signal_row_changed().connect (sigc::mem_fun (this, &Mapview::layer_edited));
	layer_list.signal_button_press_event().connect_notify (sigc::mem_fun (this, &Mapview::configure_layer));


	gend = GenericDialog::get_instance();

	show_all();
    }

    void layer_edited (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	VLOG(2) << "layer_edited at " << path.to_string();
	Gtk::TreeModel::Row row = *iter;
	bool need_refresh = false;
	
	Layer * layer = row[layer_list.columns.layer];
	if (!layer) return;
	int new_depth = row[layer_list.columns.depth];
	if (viewer.workplane.get_layer_depth (layer) != new_depth) {
	    viewer.workplane.set_layer_depth (layer, new_depth);
	    need_refresh = true;
	}

	int new_active = row[layer_list.columns.checked];
	if (new_active != viewer.workplane.get_layer_active (layer)) {
	    viewer.workplane.set_layer_active (layer, new_active);
	    need_refresh = true;
	}

	if (need_refresh) {
	    refresh();
	}
    }

    void configure_layer (GdkEventButton* event) {
	LOG() << "Event: button=" << event->button;
	if (event->button != 3 || event->type != GDK_BUTTON_PRESS) {
	    return;
	}

	if (event->window != layer_list.get_bin_window()->gobj()) {
	    return;
	}

	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn *col = NULL;
	int cx, cy;
	if (!layer_list.get_path_at_pos(event->x, event->y,
				       path, col, cx, cy)) {
	    return;
	}
	LOG() << "Path=" << path.to_string();

	Gtk::TreeModel::iterator iter = layer_list.store->get_iter(path);
	Gtk::TreeModel::Row row = *iter;
	bool need_refresh = false;

	Layer * layer = row[layer_list.columns.layer];
	LOG() << "LAYER_CONFIG REQ: " << row[layer_list.columns.text] << " (" << layer << ")\n";
	if (!layer) return;
	Options opt = layer->get_config();
	if (opt.size() == 0) return;
	layer_to_configure = layer;

	current_connection = gend->signal_result().connect(sigc::mem_fun(this, &Mapview::layer_config_result));
	Glib::ustring name = row[layer_list.columns.text];
	gend->activate(name, opt);
    }

    void layer_config_result (int r) {
	if (r == 0) { // OK
	    assert(layer_to_configure);
	    layer_to_configure->set_config(gend->get_options());
	    std::cout << "LAYER_CONFIG: " << layer_to_configure << "\n";
	    viewer.workplane.refresh_layer(layer_to_configure);
	    refresh();
	} else {
	    // do nothing
	}
	layer_to_configure = NULL;
        current_connection.disconnect();
    }

    void on_mode_change (int m) {
	action_manager->set_mode(m);
    }

    void load_file_sel() {
	std::string selected_filename;
	selected_filename = file_sel_load.get_filename();
	load_file(selected_filename);
    }

    void load_file(std::string selected_filename) {
	g_print ("Loading: %s\n", selected_filename.c_str());
	state.statusbar.push("Loading...", 0);
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
	    viewer.set_window_origin((map_layer->range().TLC() + map_layer->range().BRC())/2);
	}
	
	if (world->wpts.size() > 0 || world->trks.size() > 0) {
	    // we are loading geodata: if we already have reference, use it

	    boost::shared_ptr<LayerGeoData> layer_gd(new LayerGeoData(world.get()));
	    layer_gd->set_ref(reference);
	    state.data_layers.push_back(layer_gd);
	    add_layer(layer_gd.get(), 0, "Data: " + selected_filename);
	    viewer.set_window_origin(layer_gd->range().TLC());
	}
	refresh();
	state.statusbar.pop();
    }

     void save_file_sel() {
 	std::string selected_filename;
 	selected_filename = file_sel_save.get_filename();
 	g_print ("Saving file: %s\n", selected_filename.c_str());
 	state.statusbar.push("Saving...", 0);

        geo_data world;

        if (state.data.size()<1) return;

        for (int i=0; i<state.data.size(); i++){
          world.wpts.insert( world.wpts.end(), state.data[i].get()->wpts.begin(), state.data[i].get()->wpts.end());
          world.trks.insert( world.trks.end(), state.data[i].get()->trks.begin(), state.data[i].get()->trks.end());
          world.maps.insert( world.maps.end(), state.data[i].get()->maps.begin(), state.data[i].get()->maps.end());
        }

 	io::out(selected_filename, world, Options());
     }

    gboolean on_delete(GdkEventAny * e) {
	g_print ("Exiting...\n");
	hide();
	return true;
    }

    void add_layer (Layer * layer, int depth, Glib::ustring name) {
       viewer.workplane.add_layer(layer, depth);
       layer_list.add_layer(layer, depth, name);
    }

    void refresh () {
       viewer.refresh();
    }

    virtual ~Mapview() {
    }

    bool on_key_press(GdkEventKey * event) {
        VLOG(2) << "key_press: " << event->keyval << "";
        switch (event->keyval) {
        case 43:
        case 61:
        case 65451: // + =
        {
          viewer.zoom_in(2);
          return true;
        }
        case 45:
        case 95:
        case 65453: // _ -
        {
          viewer.zoom_out(2);
          return true;
        }
        case 'r':
        case 'R': // refresh
        {
          refresh();
          return true;
        }
        }
        return false;
    }


    bool on_button_press (GdkEventButton * event) {
      if (event->button == 1) {
        gettimeofday (&click_started, NULL);
        return true;
      }
      return false;
    }

    bool on_button_release (GdkEventButton * event) {
      if (event->button == 1) {

        struct timeval click_ended;
        gettimeofday (&click_ended, NULL);
        int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 +
                (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
        if (d > 250) return true;

        Point<int> p(int(event->x), int(event->y));
        p += viewer.get_window_origin();
        VLOG(2) << "click at: " << p.x << "," << p.y << " " << event->button;
        action_manager->click(p);
        return true;
      }
      return false;
    }



private:
    Viewer        viewer;
    MapviewState  state;

    LayerList layer_list;
    Gtk::FileSelection file_sel_load;
    Gtk::FileSelection file_sel_save;
    Gtk::Statusbar * status_bar;
    Gtk::Widget * menubar;

    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;

    g_map reference;
    bool have_reference;

    boost::shared_ptr<ActionManager> action_manager;
    Gtk::RadioAction::Group mode_group;

    struct timeval click_started;

    GenericDialog * gend;
    sigc::connection current_connection;
    Layer * layer_to_configure;
};


#endif /* MAPVIEW_H */
