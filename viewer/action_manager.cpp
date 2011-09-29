#include "action_manager.h"
#include "mapview.h"

#include "actions/action_mode.h"
#include "actions/am_add_file.h"
#include "actions/am_save_all.h"
#include "actions/am_save_vis.h"
#include "actions/am_download.h"
#include "actions/am_quit.h"

#include "actions/am_add_wpt.h"
#include "actions/am_edit_wpt.h"
#include "actions/am_move_wpt.h"

#include "actions/am_delete_wpt.h"
#include "actions/am_delete_tpt.h"
#include "actions/am_edit_tpt.h"
#include "actions/am_move_tpt.h"
#include "actions/am_edit_track.h"
#include "actions/am_add_track.h"
#include "actions/am_mark_trk.h"
#include "actions/am_trk_filter.h"

#include "actions/am_save_image.h"
#include "actions/am_show_pt.h"

#define ADD_ACT(name, group) AddAction(new name(mapview),\
   std::string("Mode") + #name, group);


ActionManager::ActionManager (Mapview * mapview_)
    : mapview(mapview_)
{
    current_mode = 0;

    ADD_ACT(AddFile,         "File")
    ADD_ACT(SaveAll,         "File")
    ADD_ACT(SaveVis,         "File")
    ADD_ACT(Download,        "File")
    ADD_ACT(Quit,            "File")

    ADD_ACT(AddWaypoint,     "Waypoints")
    ADD_ACT(EditWaypoint,    "Waypoints")
    ADD_ACT(MoveWaypoint,    "Waypoints")
    ADD_ACT(DeleteWaypoint,  "Waypoints")

    ADD_ACT(AddTrack,        "Tracks")
    ADD_ACT(EditTrack,       "Tracks")
    ADD_ACT(EditTrackpoint,  "Tracks")
    ADD_ACT(MoveTrackpoint,  "Tracks")
    ADD_ACT(DeleteTrackpoint,"Tracks")
    ADD_ACT(MarkTrack,       "Tracks")
    ADD_ACT(FilterTrack,       "Tracks")

    ADD_ACT(ActionModeNone,  "Misc")
    ADD_ACT(SaveImage,       "Misc")
    ADD_ACT(ShowPt,          "Misc")

    mapview->actions->add(Gtk::Action::create("MenuFile", "_File"));
    mapview->actions->add(Gtk::Action::create("MenuWaypoints", "_Waypoints"));
    mapview->actions->add(Gtk::Action::create("MenuTracks", "_Tracks"));
    mapview->actions->add(Gtk::Action::create("MenuMisc", "_Misc"));

    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone(mapview)));
}

void
ActionManager::AddAction(ActionMode *action,
                         const std::string & id, const std::string & menu){
  modes.push_back(boost::shared_ptr<ActionMode>(action));
  int m = modes.size()-1;
  std::string  mname   = action->get_name();
  Gtk::StockID stockid = action->get_stockid();
  Gtk::AccelKey acckey = action->get_acckey();

  if (action->is_radio()) {
    // I do not know how to create empty editable AccelKey. So i use
    // these stupid ifs...
    if (acckey.is_null())
      mapview->actions->add(
        Gtk::RadioAction::create(mapview->mode_group, id, stockid, mname),
        sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));
    else
      mapview->actions->add(
        Gtk::RadioAction::create(mapview->mode_group, id, stockid, mname),
        acckey, sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));
  }
  else {
    if (acckey.is_null())
      mapview->actions->add(
        Gtk::Action::create(id, stockid, mname),
        sigc::mem_fun(action, &ActionMode::activate));
    else
      mapview->actions->add(
        Gtk::Action::create(id, stockid, mname),
        acckey, sigc::mem_fun(action, &ActionMode::activate));
  }

  mapview->ui_manager->add_ui_from_string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='Menu" + menu + "'>"
    "      <menuitem action='" + id + "'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>"
  );
}