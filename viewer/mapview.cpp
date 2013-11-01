#include <cstdlib>
#include "mapview.h"
#include "utils/log.h"

#include "images/gps_download.h"
#include "images/gps_upload.h"

using namespace std;

Mapview::Mapview () :
    have_reference(false),
    divert_refresh(false),
    viewer(&main_gobj),
    rubber(&viewer),
    srtm("",20),
    gobj_srtm(&srtm),
    panel_wpts(this),
    panel_trks(this),
    panel_maps(this)
{
    /// window initialization
    signal_delete_event().connect_notify (
      sigc::bind(sigc::hide(sigc::mem_fun (this, &Mapview::exit)),false));
    set_default_size(640,480);

    /// global keypress event
    signal_key_press_event().connect (
      sigc::mem_fun (this, &Mapview::on_key_press));
    /// viewer mouse button events
    viewer.signal_button_press_event().connect (
      sigc::mem_fun (this, &Mapview::on_button_press));
    viewer.signal_button_release_event().connect (
      sigc::mem_fun (this, &Mapview::on_button_release));
    viewer.signal_scroll_event().connect(
      sigc::mem_fun (this, &Mapview::on_scroll));

    /// refresh events from panels -> move to viewer?
    panel_wpts.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));
    panel_trks.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));
    panel_maps.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));
    panel_srtm.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));

    // panels mouse button events
    panel_wpts.set_events(Gdk::BUTTON_PRESS_MASK);
    panel_trks.set_events(Gdk::BUTTON_PRESS_MASK);
    panel_maps.set_events(Gdk::BUTTON_PRESS_MASK);

    panel_wpts.signal_button_press_event().connect(
      sigc::mem_fun (this, &Mapview::on_panel_button_press), false);
    panel_trks.signal_button_press_event().connect(
      sigc::mem_fun (this, &Mapview::on_panel_button_press), false);
    panel_maps.signal_button_press_event().connect(
      sigc::mem_fun (this, &Mapview::on_panel_button_press), false);

    main_gobj.push_back((Workplane *) &panel_srtm);
    main_gobj.push_back((Workplane *) &panel_maps);
    main_gobj.push_back((Workplane *) &panel_trks);
    main_gobj.push_back((Workplane *) &panel_wpts);

    /// events from layer lists
    panel_wpts.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::panel_edited));
    panel_trks.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::panel_edited));
    panel_maps.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::panel_edited));

    /// events from viewer
    viewer.signal_busy().connect(
      sigc::mem_fun (this, &Mapview::show_busy_mark));
    viewer.signal_idle().connect(
      sigc::mem_fun (this, &Mapview::hide_busy_mark));

    viewer.set_bgcolor(0xB3DEF5 /*wheat*/);

    /***************************************/
    // Add my icons 
    Glib::RefPtr<Gtk::IconFactory> icon_factory = Gtk::IconFactory::create();
    icon_factory->add_default();

#define ADD_ICON(size, name) \
    icon_factory->add(\
      Gtk::StockID(#name),\
      Gtk::IconSet(\
        Gdk::Pixbuf::create_from_data (idata_##name, Gdk::COLORSPACE_RGB,\
        true /*alpha*/, 8 /*bps*/, size /*w*/, size /*h*/, (size)*4 /*rowstride*/))\
    );

    ADD_ICON(16, gps_download);
    ADD_ICON(16, gps_upload);

    /***************************************/
    /// Menues
    actions = Gtk::ActionGroup::create();

    ui_manager = Gtk::UIManager::create();
    ui_manager->insert_action_group(actions);

    add_accel_group(ui_manager->get_accel_group());
    char *home=getenv("HOME");
    if (home) Gtk::AccelMap::load(string(home) + "/" + ACCEL_FILE);

    // create actions + build menu
    action_manager.reset (new ActionManager(this));

    popup_wpts = (Gtk::Menu *)ui_manager->get_widget("/PopupWPTs");
    popup_trks = (Gtk::Menu *)ui_manager->get_widget("/PopupTRKs");
    popup_maps = (Gtk::Menu *)ui_manager->get_widget("/PopupMAPs");

    /***************************************/

    /// Panels (Notebook)
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
    panels->set_scrollable(false);
    panels->set_size_request(150,-1);

    /// Main pand: Viewer + Panels
    Gtk::HPaned * paned = manage(new Gtk::HPaned);
    paned->pack1(viewer, Gtk::EXPAND | Gtk::FILL);
    paned->pack2(*panels, Gtk::FILL);

    /// Status hbox: busy_icon + statusbar
    busy_icon = manage(new Gtk::Image());
    busy_icon->set_tooltip_text("Viewer acivity");
    busy_icon->set_size_request(20,16);
    Gtk::HBox * st_box = manage(new Gtk::HBox);
    st_box->pack_start(*busy_icon, false, true, 0);
    st_box->pack_start(statusbar, true, true, 0);

    /// Main vbox: menu + main pand + statusbar
    Gtk::VBox * vbox = manage(new Gtk::VBox);
    vbox->pack_start(*ui_manager->get_widget("/MenuBar"), false, true, 0);
    vbox->pack_start(*paned, true, true, 0);
    vbox->pack_start(*st_box, false, true, 0);
    add (*vbox);

    filename="";
    set_changed(false);
    statusbar.push("Welcome to mapsoft viewer!",0);

    show_all();
}

void
Mapview::panel_edited (const Gtk::TreeModel::Path& path,
                   const Gtk::TreeModel::iterator& iter) {
  update_gobjs();
}

void
Mapview::update_gobjs() {
  if (divert_refresh) return;
  Gtk::TreeNodeChildren::const_iterator i;

  // update layer depth and visibility in workplane
  bool ch = false;
  ch = panel_wpts.upd_wp() || ch;
  ch = panel_trks.upd_wp() || ch;
  ch = panel_maps.upd_wp() || ch;
  if (ch) refresh();

  // update comments in data
  ch = false;
  ch = panel_wpts.upd_comm() || ch;
  ch = panel_trks.upd_comm() || ch;
  ch = panel_maps.upd_comm() || ch;
  if (ch) set_changed();
}


void
Mapview::on_mode_change (int m) {
  rubber.clear();
  statusbar.push(action_manager->get_mode_name(m),0);
  action_manager->set_mode(m);
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
    statusbar.push("Load " + *i);
    try {io::in(*i, world);}
    catch (MapsoftErr e) {dlg_err.call(e);}
  }
  add_world(world, true);
}

void
Mapview::load_file(const string & file, bool force) {
  if (!force && get_changed()){
    dlg_ch_conf.call(
      sigc::bind(sigc::mem_fun(this, &Mapview::load_file), file, true));
    return;
  }
  clear_world();
  statusbar.push("Open " + file);
  geo_data world;
  try {io::in(file, world);}
  catch (MapsoftErr e) {dlg_err.call(e);}
  add_world(world, true);
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
  statusbar.push("New file");
  clear_world();
  set_changed(false);
  refresh();
}

void
Mapview::add_world(const geo_data & world, bool scroll) {
  divert_refresh=true;
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
  divert_refresh=false;
  update_gobjs();
}

void
Mapview::clear_world() {
  divert_refresh=true;
  panel_wpts.remove_all();
  panel_trks.remove_all();
  panel_maps.remove_all();
  have_reference = false;
  divert_refresh=false;
  update_gobjs();
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

void
Mapview::refresh() {
  if (!divert_refresh) viewer.redraw();
}

bool
Mapview::on_key_press(GdkEventKey * event) {
    VLOG(2) << "key_press: " << event->keyval << "";
    switch (event->keyval) {
      case 43:
      case 61:
      case 65451: // + =
        viewer.rescale(2.0);
        return true;
      case 45:
      case 95:
      case 65453: // _ -
        viewer.rescale(0.5);
        return true;
      case 'r':
      case 'R': // refresh
        refresh();
        return true;
    }
    return false;
}

bool
Mapview::on_button_press (GdkEventButton * event) {
  if (event->button == 1) {
    click_start = viewer.get_origin();
    return true;
  }
  return false;
}

bool
Mapview::on_button_release (GdkEventButton * event) {
  if (event->button == 3) {
    rubber.clear();
    action_manager->clear_state();
    return true;
  }
  if (event->button == 1) {
    iPoint p;
    Gdk::ModifierType state;
    viewer.get_window()->get_pointer(p.x,p.y,state);
    if (pdist(click_start, viewer.get_origin()) > 5) return true;
    action_manager->click(p+click_start, state);
    return true;
  }
  return false;
}

bool
Mapview::on_panel_button_press (GdkEventButton * event) {
  if (event->button == 3) {
    Gtk::Menu * M = 0;
    switch (panels->get_current_page()){
      case 0: M = popup_wpts; break;
      case 1: M = popup_trks; break;
      case 2: M = popup_maps; break;
    }
    if (M) M->popup(event->button, event->time);
    return true;
  }
  return false;
}


bool
Mapview::on_scroll(GdkEventScroll * event) {
  double scale = event->direction ? 0.5:2.0;
  viewer.rescale(scale, iPoint(event->x, event->y));
  return true;
}

void
Mapview::show_busy_mark(void){
  if (is_realized())
    busy_icon->set(Gtk::Stock::MEDIA_RECORD,Gtk::ICON_SIZE_MENU);
}

void
Mapview::hide_busy_mark(void){
  if (is_realized())
    busy_icon->clear();
}

void
Mapview::show_srtm(bool show){
  bool state = panel_srtm.exists(&gobj_srtm);

  if (state && !show){
    statusbar.push("SRTM OFF", 0);
    panel_srtm.remove_gobj(&gobj_srtm);
    refresh();
  }
  else if (!state && show){
    statusbar.push("SRTM ON", 0);
    panel_srtm.add_gobj(&gobj_srtm, 0);
  }
}

