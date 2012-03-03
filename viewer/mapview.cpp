#include <cstdlib>
#include "mapview.h"
#include "utils/log.h"

#include "images/gps_download.h"
#include "images/gps_upload.h"

using namespace std;

Mapview::Mapview () :
    have_reference(false),
    divert_refresh(false),
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
      sigc::mem_fun (this, &Mapview::refresh));


    /// events from layer lists
    wpt_ll.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::layer_edited));
    trk_ll.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::layer_edited));
    map_ll.store->signal_row_changed().connect (
      sigc::mem_fun (this, &Mapview::layer_edited));

    viewer.set_bgcolor(0xB3DEF5 /*wheat*/);

    /***************************************/
    // Add my icons 
    Glib::RefPtr<Gtk::IconFactory> icon_factory = Gtk::IconFactory::create();
    icon_factory->add_default();

    icon_factory->add(
      Gtk::StockID("gps_download"),
      Gtk::IconSet(
        Gdk::Pixbuf::create_from_data (idata_gps_download, Gdk::COLORSPACE_RGB,
        true /*alpha*/, 8 /*bps*/, 16 /*w*/, 16 /*h*/, 16*4 /*rowstride*/))
    );
    icon_factory->add(
      Gtk::StockID("gps_upload"),
      Gtk::IconSet(
        Gdk::Pixbuf::create_from_data (idata_gps_upload, Gdk::COLORSPACE_RGB,
        true /*alpha*/, 8 /*bps*/, 16 /*w*/, 16 /*h*/, 16*4 /*rowstride*/))
    );

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

    /// Main vbox: menu + main pand + statusbar
    Gtk::VBox * vbox = manage(new Gtk::VBox);
    vbox->pack_start(* ui_manager->get_widget("/MenuBar"), false, true, 0);
    vbox->pack_start(*paned, true, true, 0);
    vbox->pack_start(statusbar, false, true, 0);
    add (*vbox);

    filename="";
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
  int ch = 0; // 1 - non visible change, 2 - redraw needed

  int d=101;
  for (i  = wpt_ll.store->children().begin();
       i != wpt_ll.store->children().end(); i++){

    boost::shared_ptr<LayerWPT> layer = (*i)[wpt_ll.columns.layer];
    if (!layer) continue;
    bool act = (*i)[wpt_ll.columns.checked];
    if (workplane.get_layer_active(layer.get()) != act){
      workplane.set_layer_active(layer.get(), act);
      ch = 2;
    }
    if (workplane.get_layer_depth(layer.get()) != d){
      workplane.set_layer_depth(layer.get(), d);
      ch = 2;
    }
    string comm = (*i)[wpt_ll.columns.comm];
    if (comm!=layer->get_data()->comm){
      layer->get_data()->comm = comm;
      ch = 1;
    }
    d++;
  }

  for (i  = trk_ll.store->children().begin();
       i != trk_ll.store->children().end(); i++){

    boost::shared_ptr<LayerTRK> layer = (*i)[trk_ll.columns.layer];
    if (!layer) continue;
    bool act = (*i)[trk_ll.columns.checked];
    if (workplane.get_layer_active(layer.get()) != act){
      workplane.set_layer_active(layer.get(), act);
      ch = 2;
    }
    if (workplane.get_layer_depth(layer.get()) != d){
      workplane.set_layer_depth(layer.get(), d);
      ch = 2;
    }
    string comm = (*i)[trk_ll.columns.comm];
    if (comm!=layer->get_data()->comm){
      layer->get_data()->comm = comm;
      ch = 1;
    }
    d++;
  }

  for (i  = map_ll.store->children().begin();
       i != map_ll.store->children().end(); i++){

    boost::shared_ptr<LayerGeoMap> layer = (*i)[map_ll.columns.layer];
    if (!layer) continue;
    bool act = (*i)[map_ll.columns.checked];
    if (workplane.get_layer_active(layer.get()) != act){
      workplane.set_layer_active(layer.get(), act);
      ch = 2;
    }
    if (workplane.get_layer_depth(layer.get()) != d){
      workplane.set_layer_depth(layer.get(), d);
      ch = 2;
    }
    string comm = (*i)[map_ll.columns.comm];
    g_map_list * ml = layer->get_data();
    if (ml->size()==1){
      if (comm!=(*ml)[0].comm){
        (*ml)[0].comm = comm;
        ch = 1;
      }
    }
    else {
      if (comm!=ml->comm){
        ml->comm = comm;
        ch = 1;
      }
    }

    d++;
  }
  if (ch>0) set_changed();
  if (ch>1) refresh();
}


void
Mapview::update_ll_comm(LayerWPT * layer) {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = wpt_ll.store->children().begin();
       i != wpt_ll.store->children().end(); i++){
    boost::shared_ptr<LayerWPT> l = (*i)[wpt_ll.columns.layer];
    if (layer != l.get())  continue;
    (*i)[wpt_ll.columns.comm] = l->get_data()->comm;
  }
}

void
Mapview::update_ll_comm(LayerTRK * layer) {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = trk_ll.store->children().begin();
       i != trk_ll.store->children().end(); i++){
    boost::shared_ptr<LayerTRK> l = (*i)[trk_ll.columns.layer];
    if (layer != l.get())  continue;
    (*i)[trk_ll.columns.comm] = l->get_data()->comm;
  }
}

void
Mapview::update_ll_comm(LayerGeoMap * layer) {
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = map_ll.store->children().begin();
       i != map_ll.store->children().end(); i++){
    boost::shared_ptr<LayerGeoMap> l = (*i)[map_ll.columns.layer];
    if (layer != l.get())  continue;
    g_map_list * ml = layer->get_data();
    if (ml->size()==1){
      (*i)[map_ll.columns.comm] = (*ml)[0].comm;
    }
    else {
      (*i)[map_ll.columns.comm] = ml->comm;
    }
  }
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
  set_title((c?"*":"") + filename);
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
Mapview::add_file(string file) {
  g_print ("Add data from: %s\n", file.c_str());
  geo_data world;
  io::in(file, world, Options());
  add_world(world, true);
}

void
Mapview::load_file(string file) {

  if (get_changed()){
    // TODO ...
  }
  filename = file;
  clear_world();
  g_print ("Load file: %s\n", file.c_str());
  geo_data world;
  io::in(file, world, Options());
  add_world(world, true);
  set_changed(false);
}

void
Mapview::new_file() {

  if (get_changed()){
    // TODO ...
  }
  filename = "";
  clear_world();
  g_print ("New file\n");
  set_changed(false);
  refresh();
}

void
Mapview::add_wpts(const boost::shared_ptr<g_waypoint_list> data) {
  // note correct order:
  // - put layer to the workplane
  // - set layer/or mapview ref (layer ref is set through workplane)
  // - put layer to LayerList (layer_edited call, workplane refresh)
  // depth is set to 1 to evoke refresh!
  boost::shared_ptr<LayerWPT> layer(new LayerWPT(data.get()));
  workplane.add_layer(layer.get(), 100);
  set_changed();
  // if we already have reference, use it
  if (!have_reference) set_ref(layer->get_myref());
  else layer->set_ref(reference);
  wpt_ll.add_layer(layer, data);
}
void
Mapview::add_trks(const boost::shared_ptr<g_track> data) {
  boost::shared_ptr<LayerTRK> layer(new LayerTRK(data.get()));
  workplane.add_layer(layer.get(), 100);
  set_changed();
  // if we already have reference, use it
  if (!have_reference) set_ref(layer->get_myref());
  else layer->set_ref(reference);
  trk_ll.add_layer(layer, data);
}
void
Mapview::add_maps(const boost::shared_ptr<g_map_list> data) {
  boost::shared_ptr<LayerGeoMap> layer(new LayerGeoMap(data.get()));
  workplane.add_layer(layer.get(), 100);
  set_changed();
  // for maps always reset reference
  set_ref(layer->get_myref());
  map_ll.add_layer(layer, data);
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
  set_ref(reference);
  divert_refresh=false;
  update_layers();
}

void
Mapview::clear_world() {
  divert_refresh=true;
  workplane.clear();
  map_ll.store->clear();
  wpt_ll.store->clear();
  trk_ll.store->clear();
  have_reference = false;
  divert_refresh=false;
  update_layers();
}

geo_data
Mapview::get_world(bool visible){
  geo_data world;

  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = wpt_ll.store->children().begin();
       i != wpt_ll.store->children().end(); i++){
     if (visible && !(*i)[wpt_ll.columns.checked]) continue;
     boost::shared_ptr<g_waypoint_list> w =
       (*i)[wpt_ll.columns.data];
     w->comm=(*i)[wpt_ll.columns.comm];
     world.wpts.push_back(*w);
  }
  for (i  = trk_ll.store->children().begin();
       i != trk_ll.store->children().end(); i++){
     if (visible && !(*i)[trk_ll.columns.checked]) continue;
     boost::shared_ptr<g_track> w =
       (*i)[trk_ll.columns.data];
     w->comm=(*i)[trk_ll.columns.comm];
     world.trks.push_back(*w);
  }
  for (i  = map_ll.store->children().begin();
       i != map_ll.store->children().end(); i++){
     if (visible && !(*i)[map_ll.columns.checked]) continue;
     boost::shared_ptr<g_map_list> w =
       (*i)[map_ll.columns.data];
     w->comm=(*i)[map_ll.columns.comm];
     world.maps.push_back(*w);
      }
  return world;
}


void
Mapview::set_ref(const g_map & ref){
  if (ref.size()==0) return;
  workplane.set_ref(ref);
  reference=ref;
  have_reference=true;
}
void
Mapview::goto_wgs(dPoint p){
  if (!have_reference) return;
  convs::map2pt cnv(reference, Datum("wgs84"), Proj("lonlat"));
  cnv.bck(p);
  viewer.set_center(p);
}

void
Mapview::exit() {
  if (get_changed()){
    // TODO ...
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




int
Mapview::find_wpt(const iPoint & p, LayerWPT ** layer,
                     int radius) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = wpt_ll.store->children().begin();
       i != wpt_ll.store->children().end(); i++){
    if (!(*i)[wpt_ll.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[wpt_ll.columns.layer];
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
  for (i  = trk_ll.store->children().begin();
       i != trk_ll.store->children().end(); i++){
    if (!(*i)[trk_ll.columns.checked]) continue;
    boost::shared_ptr<LayerTRK> current_layer=
      (*i)[trk_ll.columns.layer];
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
Mapview::find_map(const iPoint & p, LayerGeoMap ** layer) const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = map_ll.store->children().begin();
       i != map_ll.store->children().end(); i++){
    if (!(*i)[map_ll.columns.checked]) continue;
    boost::shared_ptr<LayerGeoMap> current_layer=
      (*i)[map_ll.columns.layer];
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
  for (i  = wpt_ll.store->children().begin();
       i != wpt_ll.store->children().end(); i++){
    if (!(*i)[wpt_ll.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[wpt_ll.columns.layer];

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
  for (i  = trk_ll.store->children().begin();
       i != trk_ll.store->children().end(); i++){
    if (!(*i)[trk_ll.columns.checked]) continue;
    boost::shared_ptr<LayerTRK> current_layer=
      (*i)[trk_ll.columns.layer];

    vector<int> pts = current_layer->find_trackpoints(r);
    if (pts.size()>0)
      ret.insert(pair<LayerTRK*, vector<int> >(current_layer.get(), pts));
  }
  return ret;
}

LayerWPT *
Mapview::find_wpt_layer() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = wpt_ll.store->children().begin();
       i != wpt_ll.store->children().end(); i++){
    if (!(*i)[wpt_ll.columns.checked]) continue;
    boost::shared_ptr<LayerWPT> current_layer=
      (*i)[wpt_ll.columns.layer];
    return current_layer.get();
  }
  return NULL;
}

LayerGeoMap *
Mapview::find_map_layer() const{
  Gtk::TreeNodeChildren::const_iterator i;
  for (i  = map_ll.store->children().begin();
       i != map_ll.store->children().end(); i++){
    if (!(*i)[map_ll.columns.checked]) continue;
    boost::shared_ptr<LayerGeoMap> current_layer=
      (*i)[map_ll.columns.layer];
    return current_layer.get();
  }
  return NULL;
}

