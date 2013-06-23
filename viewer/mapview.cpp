#include <cstdlib>
#include "mapview.h"
#include "utils/log.h"

#include "images/gps_download.h"
#include "images/gps_upload.h"

#define DEPTH_SRTM 1000
#define DEPTH_DATA0 100

using namespace std;

Mapview::Mapview () :
    have_reference(false),
    divert_refresh(false),
    viewer(&workplane),
    rubber(&viewer),
    srtm("",20),
    gobj_srtm(&srtm),
    cnv(get_myref())
{

    /// layer drawing options (set before Action constructors)
    layer_options.put("trk_draw_dots", "");
    layer_options.put("trk_draw_arrows", "");
    layer_options.put("trk_draw_v1", 0);
    layer_options.put("trk_draw_v2", 10);
    layer_options.put("trk_draw_h1", 0);
    layer_options.put("trk_draw_h2", 1000);

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

    /// events from workplane -> move to viewer?
    layer_wpts.gobj.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));
    layer_trks.gobj.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));
    layer_maps.gobj.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));
    layer_srtm.gobj.signal_refresh.connect (
      sigc::mem_fun (this, &Mapview::refresh));

    workplane.push_back(layer_srtm.get_gobj());
    workplane.push_back(layer_maps.get_gobj());
    workplane.push_back(layer_trks.get_gobj());
    workplane.push_back(layer_wpts.get_gobj());

    /// events from layer lists
    layer_wpts.panel.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::layer_edited));
    layer_trks.panel.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::layer_edited));
    layer_maps.panel.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::layer_edited));

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

    /***************************************/

    dataview = manage(new DataView(this));

    /// Main pand: Viewer + DataView
    Gtk::HPaned * paned = manage(new Gtk::HPaned);
    paned->pack1(viewer, Gtk::EXPAND | Gtk::FILL);
    paned->pack2(*dataview, Gtk::FILL);

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
Mapview::layer_edited (const Gtk::TreeModel::Path& path,
                   const Gtk::TreeModel::iterator& iter) {
  update_layers();
}

void
Mapview::update_layers() {
  if (divert_refresh) return;
  Gtk::TreeNodeChildren::const_iterator i;

  // update layer depth and visibility in workplane
  bool ch = false;
  int d=DEPTH_DATA0;
  ch = layer_wpts.panel.upd_wp(layer_wpts.gobj, d) || ch;
  ch = layer_trks.panel.upd_wp(layer_trks.gobj, d) || ch;
  ch = layer_maps.panel.upd_wp(layer_maps.gobj, d) || ch;
  if (ch) refresh();

  // update comments in data
  ch = false;
  ch = layer_wpts.panel.upd_comm() || ch;
  ch = layer_trks.panel.upd_comm() || ch;
  ch = layer_maps.panel.upd_comm() || ch;
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
Mapview::add_wpts(const boost::shared_ptr<g_waypoint_list> & data) {
  // note correct order:
  // - put layer to the workplane
  // - set layer/or mapview ref (layer ref is set through workplane)
  // - put layer to LayerList (layer_edited call, workplane refresh)
  // depth is set to DEPTH_DATA0 to evoke refresh!
  boost::shared_ptr<LayerWPT> layer(new LayerWPT(data.get()));
  layer_wpts.gobj.add_layer(layer.get(), DEPTH_DATA0);
  set_changed();
  // if we already have reference, use it
  if (!have_reference) set_ref(layer->get_myref());
//  else layer->set_cnv(&cnv);
  layer_wpts.panel.add_layer(layer, data);
}
void
Mapview::add_trks(const boost::shared_ptr<g_track> & data) {
  boost::shared_ptr<LayerTRK> layer(new LayerTRK(data.get(), layer_options));
  layer_trks.gobj.add_layer(layer.get(), DEPTH_DATA0);
  set_changed();
  // if we already have reference, use it
  if (!have_reference) set_ref(layer->get_myref());
//  else layer->set_cnv(&cnv);
  layer_trks.panel.add_layer(layer, data);
}
void
Mapview::add_maps(const boost::shared_ptr<g_map_list> & data) {
  boost::shared_ptr<LayerMAP> layer(new LayerMAP(data.get(), layer_options));
  layer_maps.gobj.add_layer(layer.get(), DEPTH_DATA0);
  set_changed();
  // for maps always reset reference
  set_ref(layer->get_myref());
  layer_maps.panel.add_layer(layer, data);
}

void
Mapview::add_world(const geo_data & world, bool scroll) {
  divert_refresh=true;
  dPoint p(2e3,2e3);
  for (vector<g_map_list>::const_iterator i=world.maps.begin();
       i!=world.maps.end(); i++){
    boost::shared_ptr<g_map_list> data(new g_map_list(*i));
    add_maps(data);
    if (i->size() > 0) p=(*i)[0].center();
  }
  for (vector<g_waypoint_list>::const_iterator i=world.wpts.begin();
       i!=world.wpts.end(); i++){
    boost::shared_ptr<g_waypoint_list> data(new g_waypoint_list(*i));
    add_wpts(data);
    if (i->size() > 0) p=(*i)[0];
  }
  for (vector<g_track>::const_iterator i=world.trks.begin();
       i!=world.trks.end(); i++){
    boost::shared_ptr<g_track> data(new g_track(*i));
    add_trks(data);
    if (i->size() > 0) p=(*i)[0];
  }
  if (scroll && (p.x<1e3)) goto_wgs(p);
  divert_refresh=false;
  update_layers();
}

void
Mapview::clear_world() {
  divert_refresh=true;
  layer_trks.gobj.clear();
  layer_wpts.gobj.clear();
  layer_maps.gobj.clear();
  layer_maps.panel.clear();
  layer_wpts.panel.clear();
  layer_trks.panel.clear();
  have_reference = false;
  divert_refresh=false;
  update_layers();
}

geo_data
Mapview::get_world(bool visible){
  geo_data world;

  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_wpts.panel.store->children().begin();
       i != layer_wpts.panel.store->children().end(); i++){
     if (visible && !(*i)[layer_wpts.panel.columns.checked]) continue;
     boost::shared_ptr<LayerWPT> layer = (*i)[layer_wpts.panel.columns.layer];
     world.wpts.push_back(*(layer->get_data()));
  }
  for (i  = layer_trks.panel.store->children().begin();
       i != layer_trks.panel.store->children().end(); i++){
     if (visible && !(*i)[layer_trks.panel.columns.checked]) continue;
     boost::shared_ptr<LayerTRK> layer = (*i)[layer_trks.panel.columns.layer];
     world.trks.push_back(*(layer->get_data()));
  }
  for (i  = layer_maps.panel.store->children().begin();
       i != layer_maps.panel.store->children().end(); i++){
     if (visible && !(*i)[layer_maps.panel.columns.checked]) continue;
     boost::shared_ptr<LayerMAP> layer = (*i)[layer_maps.panel.columns.layer];
     world.maps.push_back(*(layer->get_data()));
  }
  return world;
}


void
Mapview::set_ref(const g_map & ref){
  if (ref.size()==0) return;
  cnv = convs::map2wgs(ref);
  workplane.set_cnv(&cnv, ref.map_proj.val);
  have_reference=true;
  cnv_proj = ref.map_proj;
  proj_opts = ref.proj_opts;
}
void
Mapview::goto_wgs(dPoint p){
  if (!have_reference) return;
  cnv.bck(p);
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
    p += viewer.get_origin();
    action_manager->click(p, state);
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
  bool state = layer_srtm.gobj.exists(&gobj_srtm);

  if (state && !show){
    statusbar.push("SRTM OFF", 0);
    layer_srtm.gobj.remove_layer(&gobj_srtm);
    refresh();
  }
  else if (!state && show){
    statusbar.push("SRTM ON", 0);
    layer_srtm.gobj.add_layer(&gobj_srtm, DEPTH_SRTM);
    layer_srtm.gobj.set_cnv(&cnv);
    layer_srtm.gobj.refresh_layer(&gobj_srtm);
  }
}

/**********************************************************/

int
Mapview::find_wpt(const iPoint & p, LayerWPT ** layer,
                     int radius) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_wpts.panel.store->children().begin();
       i != layer_wpts.panel.store->children().end(); i++){
    if (!(*i)[layer_wpts.panel.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[layer_wpts.panel.columns.layer];
    *layer = current_layer.get();
    int d = current_layer->find_waypoint(p, radius);
    if (d >= 0) return d;
  }
  *layer = NULL;
  return -1;
}

int
Mapview::find_tpt(const iPoint & p, LayerTRK ** layer,
                     const bool segment, int radius) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_trks.panel.store->children().begin();
       i != layer_trks.panel.store->children().end(); i++){
    if (!(*i)[layer_trks.panel.columns.checked]) continue;
    boost::shared_ptr<LayerTRK> current_layer=
      (*i)[layer_trks.panel.columns.layer];
    *layer = current_layer.get();
    int d;
    if (segment) d = current_layer->find_track(p, radius);
    else d = current_layer->find_trackpoint(p, radius);
    if (d >= 0) return d;
  }
  *layer = NULL;
  return -1;
}

int
Mapview::find_map(const iPoint & p, LayerMAP ** layer) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_maps.panel.store->children().begin();
       i != layer_maps.panel.store->children().end(); i++){
    if (!(*i)[layer_maps.panel.columns.checked]) continue;
    boost::shared_ptr<LayerMAP> current_layer=
      (*i)[layer_maps.panel.columns.layer];
    *layer = current_layer.get();
    int d = current_layer->find_map(p);
    if (d >= 0) return d;
  }
  *layer = NULL;
  return -1;
}

map<LayerWPT*, vector<int> >
Mapview::find_wpts(const iRect & r){
  std::map<LayerWPT*, std::vector<int> > ret;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_wpts.panel.store->children().begin();
       i != layer_wpts.panel.store->children().end(); i++){
    if (!(*i)[layer_wpts.panel.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[layer_wpts.panel.columns.layer];

    vector<int> pts = current_layer->find_waypoints(r);
    if (pts.size()>0)
      ret.insert(pair<LayerWPT*, vector<int> >(current_layer.get(), pts));
  }
  return ret;
}

map<LayerTRK*, vector<int> >
Mapview::find_tpts(const iRect & r){
  std::map<LayerTRK*, std::vector<int> > ret;
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_trks.panel.store->children().begin();
       i != layer_trks.panel.store->children().end(); i++){
    if (!(*i)[layer_trks.panel.columns.checked]) continue;
    boost::shared_ptr<LayerTRK> current_layer=
      (*i)[layer_trks.panel.columns.layer];

    vector<int> pts = current_layer->find_trackpoints(r);
    if (pts.size()>0)
      ret.insert(pair<LayerTRK*, vector<int> >(current_layer.get(), pts));
  }
  return ret;
}

LayerWPT *
Mapview::find_wpt_layer() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_wpts.panel.store->children().begin();
       i != layer_wpts.panel.store->children().end(); i++){
    if (!(*i)[layer_wpts.panel.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[layer_wpts.panel.columns.layer];
    return current_layer.get();
  }
  return NULL;
}

LayerMAP *
Mapview::find_map_layer() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = layer_maps.panel.store->children().begin();
       i != layer_maps.panel.store->children().end(); i++){
    if (!(*i)[layer_maps.panel.columns.checked]) continue;
    boost::shared_ptr<LayerMAP> current_layer=
      (*i)[layer_maps.panel.columns.layer];
    return current_layer.get();
  }
  return NULL;
}

g_map
Mapview::get_myref() const {
//  return ref_ll(180*1200); // 1200pt/degree
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*1200));
  ret.push_back(g_refpoint(180, 0, 180*1200,90*1200));
  ret.push_back(g_refpoint(0,   0, 0, 90*1200));
  return ret;
}
