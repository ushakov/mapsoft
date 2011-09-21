#include "mapview.h"

LayerListButtons::LayerListButtons(){
  up   = manage(new Gtk::Button);
  down = manage(new Gtk::Button);
  save = manage(new Gtk::Button);
  del  = manage(new Gtk::Button);
  jump = manage(new Gtk::Button);

  Gtk::IconSize isize=Gtk::ICON_SIZE_MENU;
  up->set_image(*manage(new Gtk::Image(Gtk::Stock::GO_UP, isize)));
  down->set_image(*manage(new Gtk::Image(Gtk::Stock::GO_DOWN, isize)));
  save->set_image(*manage(new Gtk::Image(Gtk::Stock::SAVE, isize)));
  del->set_image(*manage(new Gtk::Image(Gtk::Stock::DELETE, isize)));
  jump->set_image(*manage(new Gtk::Image(Gtk::Stock::JUMP_TO, isize)));

  up->set_tooltip_text("Move up");
  down->set_tooltip_text("Move down");
  save->set_tooltip_text("Save selected data");
  del->set_tooltip_text("Delete selected data");
  jump->set_tooltip_text("Jump to selected data");

  add(*up);
  add(*down);
  add(*save);
  add(*del);
  add(*jump);
}

DataView::DataView (Mapview * M) : mapview(M) {

  /// button sets
  LayerListButtons * wpt_bu   = manage(new LayerListButtons);
  LayerListButtons * trk_bu   = manage(new LayerListButtons);
  LayerListButtons * map_bu   = manage(new LayerListButtons);

  wpt_bu->del->signal_clicked().connect(
    sigc::mem_fun(this, &DataView::layer_del));
  trk_bu->del->signal_clicked().connect(
    sigc::mem_fun(this, &DataView::layer_del));
  map_bu->del->signal_clicked().connect(
    sigc::mem_fun(this, &DataView::layer_del));

  wpt_bu->jump->signal_clicked().connect(
    sigc::mem_fun(this, &DataView::layer_jump));
  trk_bu->jump->signal_clicked().connect(
    sigc::mem_fun(this, &DataView::layer_jump));
  map_bu->jump->signal_clicked().connect(
    sigc::mem_fun(this, &DataView::layer_jump));

  /// scrollwindows with layerlists
  Gtk::ScrolledWindow * scr_wpt = manage(new Gtk::ScrolledWindow);
  Gtk::ScrolledWindow * scr_trk = manage(new Gtk::ScrolledWindow);
  Gtk::ScrolledWindow * scr_map = manage(new Gtk::ScrolledWindow);
  scr_wpt->add(M->wpt_ll);
  scr_trk->add(M->trk_ll);
  scr_map->add(M->map_ll);
  scr_wpt->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  scr_trk->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  scr_map->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  /// Vboxes with scrollwindows and buttons
  Gtk::VBox * wpt_vbox = manage(new Gtk::VBox);
  Gtk::VBox * trk_vbox = manage(new Gtk::VBox);
  Gtk::VBox * map_vbox = manage(new Gtk::VBox);

  wpt_vbox->pack_start(*scr_wpt, true, true);
  trk_vbox->pack_start(*scr_trk, true, true);
  map_vbox->pack_start(*scr_map, true, true);

  wpt_vbox->pack_end(*wpt_bu, false, false);
  trk_vbox->pack_end(*trk_bu, false, false);
  map_vbox->pack_end(*map_bu, false, false);

  trk_bu->save->set_tooltip_text("Save selected tracks");
  wpt_bu->save->set_tooltip_text("Save selected waypoints");
  map_bu->save->set_tooltip_text("Save selected maps");
  trk_bu->del->set_tooltip_text("Delete selected tracks");
  wpt_bu->del->set_tooltip_text("Delete selected waypoints");
  map_bu->del->set_tooltip_text("Delete selected maps");
  trk_bu->jump->set_tooltip_text("Jump to selected track");
  wpt_bu->jump->set_tooltip_text("Jump to selected waypoints");
  map_bu->jump->set_tooltip_text("Jump to selected map");

  /// append vboxes to the Notebook
  append_page(*wpt_vbox, "WPT");
  append_page(*trk_vbox, "TRK");
  append_page(*map_vbox, "MAP");
  set_scrollable(false);
  set_size_request(150,-1);
}

void
DataView::layer_del(){
  Gtk::TreeModel::iterator it;
  switch (get_current_page()){
    case 0: // WPT
      it = mapview->wpt_ll.get_selection()->get_selected();
      if (!it) break;
      mapview->workplane.remove_layer(
        it->get_value(mapview->wpt_ll.columns.layer).get());
      mapview->wpt_ll.store->erase(it);
      break;
    case 1: // TRK
      it = mapview->trk_ll.get_selection()->get_selected();
      if (!it) break;
      mapview->workplane.remove_layer(
        it->get_value(mapview->trk_ll.columns.layer).get());
      mapview->trk_ll.store->erase(it);
    break;
    case 2: // MAP
      it = mapview->map_ll.get_selection()->get_selected();
      if (!it) break;
      mapview->workplane.remove_layer(
        it->get_value(mapview->map_ll.columns.layer).get());
      mapview->map_ll.store->erase(it);
    break;
  }
  mapview->refresh();
}

void
DataView::layer_jump(){
  Gtk::TreeModel::iterator it;
  switch (get_current_page()){
    case 0: // WPT
      it = mapview->wpt_ll.get_selection()->get_selected();
      if (it) mapview->goto_wgs(
        (*it->get_value(mapview->wpt_ll.columns.layer)->get_data())[0]);
      break;
    case 1: // TRK
      it = mapview->trk_ll.get_selection()->get_selected();
      if (it) mapview->goto_wgs(
        (*it->get_value(mapview->trk_ll.columns.layer)->get_data())[0]);
    break;
    case 2: // MAP
      it = mapview->map_ll.get_selection()->get_selected();
      if (it) mapview->goto_wgs(
        (*it->get_value(mapview->map_ll.columns.layer)->get_data())[0].center());
    break;
  }
  mapview->refresh();
}
