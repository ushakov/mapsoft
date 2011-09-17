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

    WptLL wpt_ll;
    TrkLL trk_ll;
    MapLL map_ll;

    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;
    Gtk::RadioAction::Group mode_group;
    Gtk::Statusbar  statusbar;
    GenericDialog   gend;

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
	wpt_ll.store->signal_row_changed().connect (
	  sigc::mem_fun (this, &Mapview::layer_edited));
	trk_ll.store->signal_row_changed().connect (
	  sigc::mem_fun (this, &Mapview::layer_edited));
	map_ll.store->signal_row_changed().connect (
	  sigc::mem_fun (this, &Mapview::layer_edited));

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
                       const Gtk::TreeModel::iterator& iter) {

      Gtk::TreeNodeChildren::const_iterator i;
      bool need_refresh = false;

      int d=1000;
      for (i  = wpt_ll.store->children().begin();
           i != wpt_ll.store->children().end(); i++){

        boost::shared_ptr<LayerWPT> layer = (*i)[wpt_ll.columns.layer];
        if (!layer) continue;
        bool act = (*i)[wpt_ll.columns.checked];
        if (workplane.get_layer_active(layer.get()) != act){
          workplane.set_layer_active(layer.get(), act);
          need_refresh=true;
        }
        if (workplane.get_layer_depth(layer.get()) != d){
          workplane.set_layer_depth(layer.get(), d);
          need_refresh=true;
        }
        d++;
      }

      d=2000;
      for (i  = trk_ll.store->children().begin();
           i != trk_ll.store->children().end(); i++){

        boost::shared_ptr<LayerTRK> layer = (*i)[trk_ll.columns.layer];
        if (!layer) continue;
        bool act = (*i)[trk_ll.columns.checked];
        if (workplane.get_layer_active(layer.get()) != act){
          workplane.set_layer_active(layer.get(), act);
          need_refresh=true;
        }
        if (workplane.get_layer_depth(layer.get()) != d){
          workplane.set_layer_depth(layer.get(), d);
          need_refresh=true;
        }
        d++;
      }

      d=3000;
      for (i  = map_ll.store->children().begin();
           i != map_ll.store->children().end(); i++){

        boost::shared_ptr<LayerGeoMap> layer = (*i)[map_ll.columns.layer];
        if (!layer) continue;
        bool act = (*i)[map_ll.columns.checked];
        if (workplane.get_layer_active(layer.get()) != act){
          workplane.set_layer_active(layer.get(), act);
          need_refresh=true;
        }
        if (workplane.get_layer_depth(layer.get()) != d){
          workplane.set_layer_depth(layer.get(), d);
          need_refresh=true;
        }
        d++;
      }
      if (need_refresh) refresh();
    }

    void on_mode_change (int m) {
      gend.deactivate();
      rubber.clear();
      statusbar.push(action_manager->get_mode_name(m),0);
      action_manager->set_mode(m);
    }

    void add_file(std::string file) {
      g_print ("Loading: %s\n", file.c_str());
      geo_data world;
      io::in(file, world, Options());
      add_world(world, true);
    }

    void add_wpts(const boost::shared_ptr<g_waypoint_list> data, bool scroll=false) {
      // note correct order:
      // - put layer to the workplane
      // - set layer/or mapview ref (layer ref is set through workplane)
      // - put layer to LayerList (+layer_edited coll, +workplane is refresh)
      boost::shared_ptr<LayerWPT> layer(new LayerWPT(data.get()));
      workplane.add_layer(layer.get(), 1000);
      // if we already have reference, use it
      if (!have_reference) set_ref(layer->get_myref());
      else layer->set_ref(reference);
      wpt_ll.add_layer(layer, data);
      if (scroll && (data->size()>0)) goto_wgs((*data)[0]);
//      refresh();
    }
    void add_trks(const boost::shared_ptr<g_track> data, bool scroll=false) {
      boost::shared_ptr<LayerTRK> layer(new LayerTRK(data.get()));
      workplane.add_layer(layer.get(), 2000);
      // if we already have reference, use it
      if (!have_reference) set_ref(layer->get_myref());
      else layer->set_ref(reference);
      trk_ll.add_layer(layer, data);
      if (scroll && (data->size()>0)) goto_wgs((*data)[0]);
//      refresh();
    }
    void add_maps(const boost::shared_ptr<g_map_list> data, bool scroll=false) {
      boost::shared_ptr<LayerGeoMap> layer(new LayerGeoMap(data.get()));
      workplane.add_layer(layer.get(), 3000);
      // for maps always reset reference
      set_ref(layer->get_myref());
      map_ll.add_layer(layer, data);
      if (scroll && (data->size()>0)) goto_wgs((*data)[0].center());
//      refresh();
    }
    void add_world(const geo_data & world, bool scroll=false) {
      dPoint p(2e3,2e3);
      for (std::vector<g_map_list>::const_iterator i=world.maps.begin();
           i!=world.maps.end(); i++){
        boost::shared_ptr<g_map_list> data(new g_map_list(*i));
        add_maps(data, false);
        if (i->size() > 0) p=(*i)[0].center();
      }
      for (std::vector<g_waypoint_list>::const_iterator i=world.wpts.begin();
           i!=world.wpts.end(); i++){
        boost::shared_ptr<g_waypoint_list> data(new g_waypoint_list(*i));
        add_wpts(data, false);
        if (i->size() > 0) p=(*i)[0];
      }
      for (std::vector<g_track>::const_iterator i=world.trks.begin();
           i!=world.trks.end(); i++){
        boost::shared_ptr<g_track> data(new g_track(*i));
        add_trks(data, false);
        if (i->size() > 0) p=(*i)[0];
      }
      if (scroll && (p.x<1e3)) goto_wgs(p);
    }

    void set_ref(const g_map & ref){
      if (ref.size()==0) return;
      workplane.set_ref(ref);
      reference=ref;
      have_reference=true;
    }
    void goto_wgs(dPoint p){
      if (!have_reference) return;
      convs::map2pt cnv(reference, Datum("wgs84"), Proj("lonlat"));
      cnv.bck(p);
      viewer.set_center(p);
    }

    void exit() {
      Gtk::AccelMap::save(std::string(getenv("HOME")) + "/" + ACCEL_FILE);
      g_print ("Exiting...\n");
      hide_all();
    }

    void refresh () {
std::cerr << "REFRESH\n";
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
