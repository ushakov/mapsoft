#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <sys/time.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <gtkmm.h>
#include <gtkmm/accelmap.h>

#include "workplane.h"
#include "layerlist.h"
#include "generic_dialog.h"
#include "action_manager.h"
#include "geo_io/io.h"
#include "layers/layer_geomap.h"
#include "layers/layer_trk.h"
#include "layers/layer_wpt.h"
#include "utils/log.h"

#include "gred/rubber.h"
#include "gred/dthread_viewer.h"


#define ACCEL_FILE ".mapsoft/accel"

class Mapview : public Gtk::Window {
public:

    DThreadViewer viewer;
    Rubber        rubber;
    Workplane     workplane;

    LayerList   ll_wpt, ll_trk, ll_map; /// Gtk::TreeView for layers
    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;
    Gtk::RadioAction::Group mode_group;
    Gtk::Statusbar  statusbar;
    GenericDialog   gend;

    std::vector<boost::shared_ptr<LayerGeoMap> > map_layers;
    std::vector<boost::shared_ptr<LayerTRK> > trk_layers;
    std::vector<boost::shared_ptr<LayerWPT> > wpt_layers;

    std::vector<boost::shared_ptr<geo_data> > data;

    g_map reference;
    bool have_reference;

private:
    Gtk::FileSelection file_sel_load;
    Gtk::FileSelection file_sel_save;

    boost::shared_ptr<ActionManager> action_manager;

    iPoint click_start;

public:

    Mapview () :
	file_sel_load ("Load file:"),
	file_sel_save ("Save as:"),
	have_reference(false),
        viewer(&workplane),
	rubber(&viewer)
    {

	/// window initialization
	signal_delete_event().connect_notify (
	  sigc::hide(sigc::mem_fun (this, &Mapview::exit)));
	set_default_size(640,480);

	/// events from load file selector
	file_sel_load.get_ok_button()->signal_clicked().connect (
	  sigc::mem_fun (this, &Mapview::load_file_sel));
	file_sel_load.get_ok_button()->signal_clicked().connect (
	  sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));
	file_sel_load.get_cancel_button()->signal_clicked().connect (
	  sigc::mem_fun (file_sel_load, &Gtk::Widget::hide));

	/// events from save file selector
	file_sel_save.get_ok_button()->signal_clicked().connect (
	  sigc::mem_fun (this, &Mapview::save_file_sel));
	file_sel_save.get_ok_button()->signal_clicked().connect (
	  sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));
	file_sel_save.get_cancel_button()->signal_clicked().connect (
	  sigc::mem_fun (file_sel_save, &Gtk::Widget::hide));

        /// keypress and mouse button press events
        viewer.signal_button_press_event().connect (
          sigc::mem_fun (this, &Mapview::on_button_press));
        viewer.signal_button_release_event().connect (
          sigc::mem_fun (this, &Mapview::on_button_release));
        signal_key_press_event().connect (
          sigc::mem_fun (this, &Mapview::on_key_press));

	/// events from layer lists
	ll_wpt.store->signal_row_changed().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), (int *)0, &ll_wpt));
	ll_trk.store->signal_row_changed().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), (int *)0, &ll_trk));
	ll_map.store->signal_row_changed().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), (int *)0, &ll_map));

	ll_wpt.store->signal_rows_reordered().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), &ll_wpt));
	ll_trk.store->signal_rows_reordered().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), &ll_trk));
	ll_map.store->signal_rows_reordered().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), &ll_map));

	/// events from workplane
	workplane.signal_refresh.connect (
	  sigc::mem_fun (viewer, &DThreadViewer::redraw));

	/***************************************/
	//start building menus
	actions = Gtk::ActionGroup::create();
	actions->add(Gtk::Action::create("MenuFile", "_File"));

	actions->add(Gtk::Action::create("Add", Gtk::Stock::ADD),
          sigc::mem_fun(file_sel_load, &Gtk::Widget::show));
	actions->add(Gtk::Action::create("Save", Gtk::Stock::SAVE),
          sigc::mem_fun(file_sel_save, &Gtk::Widget::show));
	actions->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT),
          sigc::mem_fun(this, &Mapview::exit));

	ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(actions);

	add_accel_group(ui_manager->get_accel_group());
        Gtk::AccelMap::load(std::string(getenv("HOME")) + "/" + ACCEL_FILE);

	ui_manager->add_ui_from_string(
	    "<ui>"
	    "  <menubar name='MenuBar'>"
	    "    <menu action='MenuFile'>"
	    "      <menuitem action='Add'/>"
	    "      <menuitem action='Save'/>"
	    "      <menuitem action='Quit'/>"
	    "    </menu>"
	    "  </menubar>"
	    "</ui>"
	);

        // create actions + build menu
	action_manager.reset (new ActionManager(this));

        /// pack widgets
	guint drawing_padding = 5;

	Gtk::VBox * vbox = manage(new Gtk::VBox);
	vbox->pack_start(* ui_manager->get_widget("/MenuBar"), false, true, 0);

	Gtk::HPaned * paned = manage(new Gtk::HPaned);
	paned->pack1(viewer, Gtk::EXPAND | Gtk::FILL);

	Gtk::ScrolledWindow * scrw = manage(new Gtk::ScrolledWindow);
	Gtk::VBox * right_vbox = manage(new Gtk::VBox);
        Gtk::Expander * exp_wpt = manage(new Gtk::Expander);
        Gtk::Expander * exp_trk = manage(new Gtk::Expander);
        Gtk::Expander * exp_map = manage(new Gtk::Expander);
        exp_wpt->set_label("Waypoints:");
        exp_trk->set_label("Tracks:");
        exp_map->set_label("Maps:");
	right_vbox->pack_start(* exp_wpt, false, true, 0);
	right_vbox->pack_start(* exp_trk, false, true, 0);
	right_vbox->pack_start(* exp_map, false, true, 0);
	scrw->add(*right_vbox);
	scrw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrw->set_size_request(128,-1);
       ll_wpt.set_dep_base(1000);
       ll_trk.set_dep_base(2000);
       ll_map.set_dep_base(3000);
	exp_wpt->add(ll_wpt);
	exp_trk->add(ll_trk);
	exp_map->add(ll_map);
	paned->pack2(*scrw, Gtk::FILL);

	vbox->pack_start(*paned, true, true, drawing_padding);
	vbox->pack_start(statusbar, false, true, 0);
	add (*vbox);

        viewer.set_bgcolor(0xB3DEF5 /*wheat*/);

	statusbar.push("Welcome to mapsoft viewer!",0);
	show_all();
    }

    virtual ~Mapview() { }

    void layer_edited (const Gtk::TreeModel::Path& path,
                       const Gtk::TreeModel::iterator& iter,
                       int new_order[],
                       LayerList * layer_list) {

//std::cerr << "LE " << path.front() << "\n";

        int dep=layer_list->get_dep_base();
        // note: there is an extra row in the layer when reordering occurs
        for (int n = 0; n<layer_list->size(); n++){
          if (new_order!= NULL) std::cerr << "O: " << new_order[n] << "\n";
        }

        Gtk::TreeNodeChildren::const_iterator i;
        for (i  = layer_list->store->children().begin();
             i != layer_list->store->children().end(); i++){
          LayerGeo * layer = (*i)[layer_list->columns.layer];
          int active = (*i)[layer_list->columns.checked];
          Gtk::TreeModel::Path path(i);

//if (layer) std::cerr << ">>> " << (int)layer << " " << path.front() << "\n";
          if (!layer) continue;
          workplane.set_layer_depth (layer, dep++);
          workplane.set_layer_active (layer, active);
        }
        refresh();
    }

    void on_mode_change (int m) {
	gend.deactivate();
	rubber.clear();
        statusbar.push(action_manager->get_mode_name(m),0);
	action_manager->set_mode(m);
    }

    void load_file_sel() {
	std::string selected_filename;
	selected_filename = file_sel_load.get_filename();
	add_file(selected_filename);
    }

    void add_file(std::string selected_filename) {
	g_print ("Loading: %s\n", selected_filename.c_str());
	statusbar.push("Loading...", 0);

	boost::shared_ptr<geo_data> world(new geo_data);
	io::in(selected_filename, *(world.get()), Options());
        int pos = selected_filename.rfind('/');
        add_world(world, selected_filename.substr(pos+1));
	LOG() << "Loaded " << selected_filename << " to world at " << world.get();
    }

    void add_world(const boost::shared_ptr<geo_data> world, const std::string & name, bool scroll=true) {
	data.push_back(world);
        g_map new_ref;
	if (world->maps.size() > 0) {
	    // we are loading maps: if we already have reference, use it
	    boost::shared_ptr<LayerGeoMap> map_layer(new LayerGeoMap(world.get()));
	    new_ref = map_layer->get_myref();
	    map_layers.push_back(map_layer);
            workplane.add_layer(map_layer.get(), 1);
            ll_map.add_layer(map_layer.get(), name);
	}
	if (world->trks.size() > 0) {
	    // we are loading tracks: if we already have reference, use it
	    boost::shared_ptr<LayerTRK> trk_layer(new LayerTRK(world.get()));
            if (!have_reference) new_ref = trk_layer->get_myref();
            else trk_layer->set_ref(reference);
	    trk_layers.push_back(trk_layer);
            workplane.add_layer(trk_layer.get(), 1);
            ll_trk.add_layer(trk_layer.get(), name);
	}
	if (world->wpts.size() > 0) {
	    // we are loading waypoints: if we already have reference, use it
	    boost::shared_ptr<LayerWPT> wpt_layer(new LayerWPT(world.get()));
            if (!have_reference) new_ref = wpt_layer->get_myref();
            else wpt_layer->set_ref(reference);
	    wpt_layers.push_back(wpt_layer);
            workplane.add_layer(wpt_layer.get(), 1);
            ll_wpt.add_layer(wpt_layer.get(), name);
	}

        if (new_ref.size()){
           workplane.set_ref(new_ref);
           reference=new_ref;
           have_reference=true;
        }

	if (scroll && have_reference){
          // scroll to the first trackpoint or waypoint or map center
          dPoint new_orig;

          std::vector<g_map>::const_iterator mli = world->maps.begin();
          while (mli!=world->maps.end()){
            if (mli->size() > 0){
              new_orig = mli->center();
              break;
            }
            mli++;
          }

          std::vector<g_waypoint_list>::const_iterator wli = world->wpts.begin();
          while (wli!=world->wpts.end()){
            if (wli->size() > 0){
              new_orig = (*wli)[0];
              break;
            }
            wli++;
          }
          std::vector<g_track>::const_iterator tli = world->trks.begin();
          while (tli!=world->trks.end()){
            if (tli->size() > 0){
              new_orig = (*tli)[0];
              break;
            }
            tli++;
          }
          convs::map2pt cnv(reference, Datum("wgs84"), Proj("lonlat"));
          cnv.bck(new_orig);
          viewer.set_center(new_orig);
        }

//	refresh();
//	statusbar.pop();
    }


     void save_file_sel() {
 	std::string selected_filename;
 	selected_filename = file_sel_save.get_filename();
 	g_print ("Saving file: %s\n", selected_filename.c_str());
 	statusbar.push("Saving...", 0);

        geo_data world;

        if (data.size()<1) return;

        for (int i=0; i<data.size(); i++){
          world.wpts.insert( world.wpts.end(), data[i].get()->wpts.begin(), data[i].get()->wpts.end());
          world.trks.insert( world.trks.end(), data[i].get()->trks.begin(), data[i].get()->trks.end());
          world.maps.insert( world.maps.end(), data[i].get()->maps.begin(), data[i].get()->maps.end());
        }

 	io::out(selected_filename, world, Options());
     }

    void exit() {
      Gtk::AccelMap::save(std::string(getenv("HOME")) + "/" + ACCEL_FILE);
      g_print ("Exiting...\n");
      hide_all();
    }

    void refresh () {
       viewer.redraw();
    }

    bool on_key_press(GdkEventKey * event) {
        VLOG(2) << "key_press: " << event->keyval << "";
        switch (event->keyval) {
          case 43:
          case 61:
          case 65451: // + =
            viewer.rescale(2.0);
            reference*=2;
            return true;
          case 45:
          case 95:
          case 65453: // _ -
            viewer.rescale(0.5);
            reference/=2;
            return true;
          case 'r':
          case 'R': // refresh
            refresh();
            return true;
        }
        return false;
    }


    bool on_button_press (GdkEventButton * event) {
      if (event->button == 1) {
        click_start = viewer.get_origin();
        return true;
      }
      return false;
    }

    bool on_button_release (GdkEventButton * event) {
      if (event->button == 3) {
        gend.deactivate();
        rubber.clear();
        action_manager->clear_state();
        return true;
      }
      if (event->button == 1) {
        iPoint p;
	Gdk::ModifierType state;
        viewer.get_window()->get_pointer(p.x,p.y,state);
        if (pdist(click_start, viewer.get_origin()) > 5) return true;
        p += viewer.get_origin();
        action_manager->click(p, state);
        return true;
      }
      return false;
    }

};


#endif /* MAPVIEW_H */
