#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <sys/time.h>
#include <boost/shared_ptr.hpp>
#include <gtkmm.h>

#include <viewer/workplane.h>
#include <viewer/viewer.h>
#include <layers/layer_geomap.h>
#include <layers/layer_geodata.h>
#include <viewer/layerlist.h>

#include <viewer/action_manager.h>

class MapviewState {
public:
    boost::shared_ptr<Workplane> workplane;
	
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

	state.workplane.reset(new Workplane(256));
	viewer.reset(new Viewer(state.workplane));
	action_manager.reset (new ActionManager (&state));

	//load file selector
	file_sel_load.get_ok_button()->signal_clicked().connect (sigc::mem_fun (this, &Mapview::load_file_sel));
	file_sel_load.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));
	file_sel_load.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));
	
 	//save file selector
 	file_sel_save.get_ok_button()->signal_clicked().connect (sigc::mem_fun (this, &Mapview::save_file_sel));
 	file_sel_save.get_ok_button()->signal_clicked().connect (sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));
 	file_sel_save.get_cancel_button()->signal_clicked().connect (sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));
	
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
	
	//status_bar
	status_bar = new Gtk::Statusbar;
	status_bar->push("Welcome to mapsoft viewer!",0);
	
	guint drawing_padding = 5;
	
	Gtk::VBox * vbox = manage(new Gtk::VBox);
	vbox->pack_start(*menubar, false, true, 0);
	
	Gtk::HPaned * paned = manage(new Gtk::HPaned);
	paned->pack1(*viewer, Gtk::EXPAND | Gtk::FILL);
	
	Gtk::ScrolledWindow * scrw = manage(new Gtk::ScrolledWindow);
	scrw->add(layer_list);
	scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrw->set_size_request(128,-1);
	paned->pack2(*scrw, Gtk::FILL);

	vbox->pack_start(*paned, true, true, drawing_padding);
	vbox->pack_start(*status_bar, false, true, 0);
	add (*vbox);
	signal_key_press_event().connect (sigc::mem_fun (this, &Mapview::on_keypress));

	// connect events from layer list
	layer_list.store->signal_row_changed().connect (sigc::mem_fun (this, &Mapview::layer_edited));

	// connect events from viewer
	viewer->signal_motion_notify_event().connect (sigc::mem_fun (this, &Mapview::pointer_moved));
	viewer->signal_button_press_event().connect (sigc::mem_fun (this, &Mapview::mouse_button_pressed));
	viewer->signal_button_release_event().connect (sigc::mem_fun (this, &Mapview::mouse_button_released));
	
	show_all();
    }

    void layer_edited (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	std::cout << "layer_edited at " << path.to_string() << std::endl;
//	for (int k = 0; k < data.size(); ++k) {
//	    for (int i = 0; i < data[k]->maps.size(); ++i) {
//		std::cout << "map " << i << "/" << k << ": " << data[k]->maps[i].file << std::endl;
//	    }
//	}
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
    

//     void clear_data(Viewer * v) {
// 	g_print ("Clear all data");
// 	status_bar->push("Clear all data", 0);
// 	world.clear();
// 	v->clear_cache();
//     }

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
	status_bar->push("Loading...", 0);
	boost::shared_ptr<geo_data> world (new geo_data);
	state.data.push_back(world);
	io::in(selected_filename, *(world.get()), Options());
	std::cout <<"Loaded " << selected_filename << " to world at " << world.get() << std::endl;

	if (world->maps.size() > 0) {
	    // we are loading maps: if we already have reference, use it
	    boost::shared_ptr<LayerGeoMap> map_layer(new LayerGeoMap(world.get()));
	    if (have_reference) {
		map_layer->set_ref(reference);
	    } else {
		map_layer->set_ref();
		reference = map_layer->get_ref();
		have_reference = true;
	    }
	    state.map_layers.push_back(map_layer);
	    add_layer(map_layer.get(), 100, "Maps: " + selected_filename);
	    viewer->set_window_origin((map_layer->range().TLC() + map_layer->range().BRC())/2);
	}
	
	if (world->wpts.size() > 0 || world->trks.size() > 0) {
	    // we are loading geodata: if we already have reference, use it
	    boost::shared_ptr<LayerGeoData> layer_gd(new LayerGeoData(world.get()));
	    if (have_reference) {
		layer_gd->set_ref(reference);
	    } else {
		layer_gd->set_ref();
		reference = layer_gd->get_ref();
		have_reference = true;
	    }
	    state.data_layers.push_back(layer_gd);
	    add_layer(layer_gd.get(), 0, "Data: " + selected_filename);
	    viewer->set_window_origin(layer_gd->range().TLC());
	}
	refresh();
	status_bar->pop();
    }
    
     void save_file_sel() {
 	std::string selected_filename;
 	selected_filename = file_sel_save.get_filename();
 	g_print ("Saving file: %s\n", selected_filename.c_str());
 	status_bar->push("Saving...", 0);

        geo_data world;

        for (int i=0; i<state.data.size()-1; i++){
          world.wpts.insert( world.wpts.end(), state.data[i].get()->wpts.begin(), state.data[i].get()->wpts.end());
          world.trks.insert( world.trks.end(), state.data[i].get()->trks.begin(), state.data[i].get()->trks.end());
          world.maps.insert( world.maps.end(), state.data[i].get()->maps.begin(), state.data[i].get()->maps.end());
        }
  
        if (state.data.size()<1) return;
 	io::out(selected_filename, world, Options());
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
            int nom = state.workplane->get_scale_nom();
            int denom = state.workplane->get_scale_denom();
            if (denom/nom > 1) state.workplane->set_scale_denom(denom/2);
            else state.workplane->set_scale_nom(nom*2);

            int n = state.workplane->get_scale_nom()*denom;
            int dn  = state.workplane->get_scale_denom()*nom;

            Point<int> wcenter = viewer->get_window_origin() + viewer->get_window_size()/2; 
            Point<int> origin = (wcenter*n)/dn-viewer->get_window_size()/2;

            viewer->set_window_origin(origin);                                                         
	    viewer->refresh();
	    std::cerr << " scale: " 
		      << state.workplane->get_scale_nom() << ":" 
		      << state.workplane->get_scale_denom() <<  std::endl;
	    return true;                                                                     
	}                                                                                  
	case 45:                                                                           
	case 65453: // -                                                                   
	{                                                                                  
            int nom = state.workplane->get_scale_nom();
            int denom = state.workplane->get_scale_denom();
            if (denom/nom >= 1) state.workplane->set_scale_denom(denom*2);
            else state.workplane->set_scale_nom(nom/2);

            int n = state.workplane->get_scale_nom()*denom;
            int dn  = state.workplane->get_scale_denom()*nom;

            Point<int> wcenter = viewer->get_window_origin() + viewer->get_window_size()/2;
            Point<int> origin = (wcenter*n)/dn-viewer->get_window_size()/2;

            viewer->set_window_origin(origin);                                                         
	    viewer->refresh();
	    std::cerr << " scale: " 
		      << state.workplane->get_scale_nom() << ":" 
		      << state.workplane->get_scale_denom() <<  std::endl;
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
#ifdef DEBUG_MAPVIEW
	std::cerr << "press: " << event->x << "," << event->y << " " << event->button << std::endl;
#endif
	if (event->button == 1) {
	    drag_pos = Point<int> ((int)event->x, (int)event->y);
	    gettimeofday (&click_started, NULL);
	    return true;
	}
    }

    virtual bool
    mouse_button_released (GdkEventButton * event) {
#ifdef DEBUG_MAPVIEW
	std::cerr << "release: " << event->x << "," << event->y << " " << event->button << std::endl;
#endif
	if (event->button == 1) {
	    struct timeval click_ended;
	    gettimeofday (&click_ended, NULL);
	    int d = (click_ended.tv_sec - click_started.tv_sec) * 1000 + (click_ended.tv_usec - click_started.tv_usec) / 1000; // in ms
	    if (d < 100) {
		Point<int> p(int(event->x), int(event->y));
		p += viewer->get_window_origin();
		p *= state.workplane->get_scale_denom();
		p /= state.workplane->get_scale_nom();
#ifdef DEBUG_MAPVIEW
		std::cerr << "click at: " << p.x << "," << p.y << " " << event->button << std::endl;
#endif
		action_manager->click(p);
		return true;
	    }
	}
    }

    virtual bool
    pointer_moved (GdkEventMotion * event) {
	Point<int> pos ((int) event->x, (int) event->y);
#ifdef DEBUG_MAPVIEW
	std::cerr << "motion: " << pos << (event->is_hint? " hint ":"") << std::endl;
#endif

	if (!(event->state & Gdk::BUTTON1_MASK) || !event->is_hint) return false;

	Point<int> shift = pos - drag_pos;
	Point<int> window_origin = viewer->get_window_origin();
	window_origin -= shift;
	viewer->set_window_origin(window_origin);
	drag_pos = pos;

	return true;
    }
    
    virtual ~Mapview() {
	state.workplane.reset();
	viewer.reset();
	delete status_bar;
    }

private:
    boost::shared_ptr<Viewer> viewer;
    MapviewState state;

    LayerList layer_list;
    Gtk::FileSelection file_sel_load;
    Gtk::FileSelection file_sel_save;
    Gtk::Statusbar * status_bar;
    Gtk::Widget * menubar;

    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;

    g_map reference;
    bool have_reference;

    Point<int> drag_pos;

    boost::shared_ptr<ActionManager> action_manager;
    Gtk::RadioAction::Group mode_group;

    struct timeval click_started;
};


#endif /* MAPVIEW_H */
