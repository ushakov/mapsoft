#include <cstdlib>
#include "mapview.h"
#include "utils/log.h"

//#include "images/gps_download.h"
//#include "images/gps_upload.h"

using namespace std;

Mapview::Mapview () :
    have_reference(false),
    viewer(&main_gobj),
    rubber(&viewer),
    panel_wpts(this),
    panel_trks(this),
    panel_maps(this),
    action_manager(this)
{
    /// window initialization
    signal_delete_event().connect_notify (
      sigc::bind(sigc::hide(sigc::mem_fun (this, &Mapview::exit)),false));
    set_default_size(640,480);

    /// global keypress event -- send all keys to the viewer first:
    signal_key_press_event().connect (
      sigc::mem_fun (&viewer, &DThreadViewer::on_key_press));

    // TODO: move to the viewer
    main_gobj.signal_redraw_me().connect(
    sigc::mem_fun(&viewer, &DThreadViewer::draw));

    // mapview wants to know about data change only to set a star in
    // the window title.
    panel_wpts.signal_data_changed().connect(
      sigc::bind(sigc::mem_fun(this, &Mapview::set_changed), true));
    panel_trks.signal_data_changed().connect(
      sigc::bind(sigc::mem_fun(this, &Mapview::set_changed), true));
    panel_maps.signal_data_changed().connect(
      sigc::bind(sigc::mem_fun(this, &Mapview::set_changed), true));

    main_gobj.push_back((Workplane *) &panel_srtm);
    main_gobj.push_back((Workplane *) &panel_maps);
    main_gobj.push_back((Workplane *) &panel_trks);
    main_gobj.push_back((Workplane *) &panel_wpts);
    main_gobj.connect_signals();

    /// events from viewer
    viewer.signal_busy().connect(
      sigc::mem_fun (&spanel, &StatusPanel::set_busy));
    viewer.signal_idle().connect(
      sigc::mem_fun (&spanel, &StatusPanel::set_idle));

    viewer.set_bgcolor(0xB3DEF5 /*wheat*/);

    /***************************************/

    /// Build panels (Notebook)
    panels = manage(new Gtk::Notebook());
    /// scrollwindows with layerlists
    Gtk::ScrolledWindow * scr_wpt = manage(new Gtk::ScrolledWindow);
    Gtk::ScrolledWindow * scr_trk = manage(new Gtk::ScrolledWindow);
    Gtk::ScrolledWindow * scr_map = manage(new Gtk::ScrolledWindow);
    scr_wpt->add(panel_wpts);
    scr_trk->add(panel_trks);
    scr_map->add(panel_maps);
    scr_wpt->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scr_trk->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scr_map->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    /// append pages to the Notebook
    panels->append_page(*scr_wpt, "WPT");
    panels->append_page(*scr_trk, "TRK");
    panels->append_page(*scr_map, "MAP");
    panels->append_page(panel_srtm, "SRTM");
    panels->set_scrollable(false);
    panels->set_size_request(150,-1);

    /// Build main paned: Viewer + Panels
    Gtk::HPaned * paned = manage(new Gtk::HPaned);
    paned->pack1(viewer, Gtk::EXPAND | Gtk::FILL);
    paned->pack2(*panels, Gtk::FILL);

    /// Build main vbox: menu + main pand + statusbar
    Gtk::VBox * vbox = manage(new Gtk::VBox);
    vbox->pack_start(*action_manager.get_main_menu(), false, true, 0);
    vbox->pack_start(*paned, true, true, 0);
    vbox->pack_start(spanel, false, true, 0);
    add (*vbox);

    filename="";
    set_changed(false);
    spanel.message("Welcome to mapsoft viewer!");

    show_all();
}

bool
Mapview::get_changed() const{
  return changed;
}
void
Mapview::set_changed(const bool c){
  changed=c;
  set_title(string("mapsoft: ") + (c?"*":"") + filename);
}
string
Mapview::get_filename() const{
  return filename;
}

void
Mapview::set_filename(const string & f){
  filename=f;
  set_changed(false);
}

void
Mapview::add_files(const list<string> & files) {
  geo_data world;
  list<string>::const_iterator i;
  for (i=files.begin(); i!=files.end(); i++){
    spanel.message("Load " + *i);
    try {io::in(*i, world);}
    catch (MapsoftErr e) {dlg_err.call(e);}
  }
  viewer.start_waiting();
  add_world(world, true);
  viewer.stop_waiting();
}

void
Mapview::load_file(const string & file, bool force) {
  if (!force && get_changed()){
    dlg_ch_conf.call(
      sigc::bind(sigc::mem_fun(this, &Mapview::load_file), file, true));
    return;
  }
  geo_data world;
  try {io::in(file, world);}
  catch (MapsoftErr e) {dlg_err.call(e);}
  spanel.message("Open " + file);

  viewer.start_waiting();
  clear_world();
  add_world(world, true);
  viewer.stop_waiting();
  if (io::testext(file, ".xml")) filename = file;
  set_changed(false);
}

void
Mapview::new_file(bool force) {
  if (!force && get_changed()){
    dlg_ch_conf.call(
      sigc::bind(sigc::mem_fun (this, &Mapview::new_file), true));
    return;
  }
  filename = "";
  spanel.message("New file");
  viewer.start_waiting();
  clear_world();
  viewer.stop_waiting();
  set_changed(false);
}

void
Mapview::add_world(const geo_data & world, bool scroll) {
  dPoint p(2e3,2e3);
  for (vector<g_map_list>::const_iterator i=world.maps.begin();
       i!=world.maps.end(); i++){
    boost::shared_ptr<g_map_list> data(new g_map_list(*i));
    panel_maps.add(data);
    if (i->size() > 0) p=(*i)[0].center();
  }
  for (vector<g_waypoint_list>::const_iterator i=world.wpts.begin();
       i!=world.wpts.end(); i++){
    boost::shared_ptr<g_waypoint_list> data(new g_waypoint_list(*i));
    panel_wpts.add(data);
    if (i->size() > 0) p=(*i)[0];
  }
  for (vector<g_track>::const_iterator i=world.trks.begin();
       i!=world.trks.end(); i++){
    boost::shared_ptr<g_track> data(new g_track(*i));
    panel_trks.add(data);
    if (i->size() > 0) p=(*i)[0];
  }
  set_changed();
  if (scroll && (p.x<1e3)) goto_wgs(p);
}

void
Mapview::clear_world() {
  panel_wpts.remove_all();
  panel_trks.remove_all();
  panel_maps.remove_all();
  panel_srtm.show(false);
  have_reference = false;
}

geo_data
Mapview::get_world(bool visible){
  geo_data world;
  panel_maps.get_data(world, visible);
  panel_trks.get_data(world, visible);
  panel_wpts.get_data(world, visible);
  return world;
}

void
Mapview::set_ref(const g_map & ref){
  if (ref.size()==0) return;
  main_gobj.set_ref(ref);
  have_reference=true;
}

void
Mapview::goto_wgs(dPoint p){
  if (!have_reference) return;
  get_cnv()->bck(p);
  viewer.set_center(p);
}

void
Mapview::exit(bool force) {
  if (!force && get_changed()){
    dlg_ch_conf.call(
      sigc::bind(sigc::mem_fun (this, &Mapview::exit), true));
    return;
  }
  char *home=getenv("HOME");
  if (home) Gtk::AccelMap::save(string(home) + "/" + ACCEL_FILE);
  g_print ("Exiting...\n");
  hide_all();
}
