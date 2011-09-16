#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <sys/time.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <gtkmm.h>
#include <gtkmm/accelmap.h>

#include "workplane.h"
#include "layerlist.h"
#include "dataview.h"
#include "widgets.h"
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
    boost::shared_ptr<ActionManager> action_manager;
    iPoint click_start;

public:

    Mapview () :
	have_reference(false),
        viewer(&workplane),
	rubber(&viewer)
    {

	/// window initialization
	signal_delete_event().connect_notify (
	  sigc::hide(sigc::mem_fun (this, &Mapview::exit)));
	set_default_size(640,480);

        /// global keypress event
        signal_key_press_event().connect (
          sigc::mem_fun (this, &Mapview::on_key_press));
        /// viewer mouse button events
        viewer.signal_button_press_event().connect (
          sigc::mem_fun (this, &Mapview::on_button_press));
        viewer.signal_button_release_event().connect (
          sigc::mem_fun (this, &Mapview::on_button_release));
        /// events from workplane -> move to viewer?
        workplane.signal_refresh.connect (
        sigc::mem_fun (viewer, &DThreadViewer::redraw));

	/// events from layer lists
	ll_wpt.store->signal_row_changed().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), &ll_wpt));
	ll_trk.store->signal_row_changed().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), &ll_trk));
	ll_map.store->signal_row_changed().connect (
	  sigc::bind(sigc::mem_fun (this, &Mapview::layer_edited), &ll_map));

        viewer.set_bgcolor(0xB3DEF5 /*wheat*/);

	/***************************************/
	/// Menues
	actions = Gtk::ActionGroup::create();

	ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(actions);

	add_accel_group(ui_manager->get_accel_group());
        Gtk::AccelMap::load(std::string(getenv("HOME")) + "/" + ACCEL_FILE);

        // create actions + build menu
	action_manager.reset (new ActionManager(this));

	/***************************************/

        /// Layerlists and DataView
        ll_wpt.set_dep_base(1000);
        ll_trk.set_dep_base(2000);
        ll_map.set_dep_base(3000);
        DataView * dw = manage(new DataView(this));

        /// Main pand: Viewer + DataView
	Gtk::HPaned * paned = manage(new Gtk::HPaned);
	paned->pack1(viewer, Gtk::EXPAND | Gtk::FILL);
	paned->pack2(*dw, Gtk::FILL);

        /// Main vbox: menu + main pand + statusbar
	guint drawing_padding = 5;
	Gtk::VBox * vbox = manage(new Gtk::VBox);
	vbox->pack_start(* ui_manager->get_widget("/MenuBar"), false, true, 0);
	vbox->pack_start(*paned, true, true, drawing_padding);
	vbox->pack_start(statusbar, false, true, 0);
	add (*vbox);

	statusbar.push("Welcome to mapsoft viewer!",0);
	show_all();
    }

    virtual ~Mapview() { }

    void layer_edited (const Gtk::TreeModel::Path& path,
                       const Gtk::TreeModel::iterator& iter,
                       LayerList * layer_list) {

        int dep=layer_list->get_dep_base();

        Gtk::TreeNodeChildren::const_iterator i;
        for (i  = layer_list->store->children().begin();
             i != layer_list->store->children().end(); i++){
          LayerGeo * layer = (*i)[layer_list->columns.layer];
          int active = (*i)[layer_list->columns.checked];

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

    void add_file(std::string selected_filename) {
	g_print ("Loading: %s\n", selected_filename.c_str());
	statusbar.push("Loading...", 0);

	boost::shared_ptr<geo_data> world(new geo_data);
	io::in(selected_filename, *(world.get()), Options());
        int pos = selected_filename.rfind('/');
        std::string shortname=selected_filename.substr(pos+1);
        add_world(world, shortname);
//	LOG() << "Loaded " << selected_filename << " to world at " << world.get();
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
            ll_map.add_layer(map_layer.get(), world.get(), name);
	}
	if (world->trks.size() > 0) {
	    // we are loading tracks: if we already have reference, use it
	    boost::shared_ptr<LayerTRK> trk_layer(new LayerTRK(world.get()));
            if (!have_reference) new_ref = trk_layer->get_myref();
            else trk_layer->set_ref(reference);
	    trk_layers.push_back(trk_layer);
            workplane.add_layer(trk_layer.get(), 1);
            ll_trk.add_layer(trk_layer.get(), world.get(), name);
	}
	if (world->wpts.size() > 0) {
	    // we are loading waypoints: if we already have reference, use it
	    boost::shared_ptr<LayerWPT> wpt_layer(new LayerWPT(world.get()));
            if (!have_reference) new_ref = wpt_layer->get_myref();
            else wpt_layer->set_ref(reference);
	    wpt_layers.push_back(wpt_layer);
            workplane.add_layer(wpt_layer.get(), 1);
            ll_wpt.add_layer(wpt_layer.get(), world.get(), name);
	}

        if (new_ref.size()){
           workplane.set_ref(new_ref);
           reference=new_ref;
           have_reference=true;
        }

	if (scroll && have_reference){
          // scroll to the first trackpoint or waypoint or map center
          dPoint new_orig;

          std::vector<g_map_list>::const_iterator mli = world->maps.begin();
          while (mli!=world->maps.end()){
            if (mli->size() > 0){
              new_orig = (*mli)[0].center();
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
