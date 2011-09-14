#include "action_manager.h"
#include "mapview.h"

#include "actions/action_mode.h"
#include "actions/am_add_file.h"
#include "actions/am_save_all.h"
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

#include "actions/am_make_tiles.h"
#include "actions/am_save_image.h"
#include "actions/am_show_pt.h"

ActionManager::ActionManager (Mapview * mapview_)
    : mapview(mapview_)
{
    current_mode = 0;

    AddAction(new AddFile(mapview),         "File");
    AddAction(new SaveAll(mapview),         "File");
    AddAction(new Download(mapview),        "File");
    AddAction(new Quit(mapview),            "File");

    AddAction(new AddWaypoint(mapview),     "Waypoints");
    AddAction(new EditWaypoint(mapview),    "Waypoints");
    AddAction(new MoveWaypoint(mapview),    "Waypoints");
    AddAction(new DeleteWaypoint(mapview),  "Waypoints");

    AddAction(new AddTrack(mapview),        "Tracks");
    AddAction(new EditTrack(mapview),       "Tracks");
    AddAction(new EditTrackpoint(mapview),  "Tracks");
    AddAction(new MoveTrackpoint(mapview),  "Tracks");
    AddAction(new DeleteTrackpoint(mapview),"Tracks");
    AddAction(new MarkTrack(mapview),       "Tracks");

    AddAction(new ActionModeNone(mapview),  "Misc");
    AddAction(new SaveImage(mapview),       "Misc");
    AddAction(new MakeTiles(mapview),       "Misc");
    AddAction(new ShowPt(mapview),          "Misc");

    mapview->actions->add(Gtk::Action::create("MenuWaypoints", "_Waypoints"));
    mapview->actions->add(Gtk::Action::create("MenuTracks", "_Tracks"));
    mapview->actions->add(Gtk::Action::create("MenuMisc", "_Misc"));

    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone(mapview)));
}

void
ActionManager::AddAction(ActionMode *action, const std::string & menu){
  modes.push_back(boost::shared_ptr<ActionMode>(action));
  int m = modes.size()-1;
  std::string  mid     = "Mode" + boost::lexical_cast<std::string>(m);
  std::string  mname   = action->get_name();
  Gtk::StockID stockid = action->get_stockid();
  Gtk::AccelKey acckey = action->get_acckey();

  if (action->is_radio()) {
    mapview->actions->add(
      Gtk::RadioAction::create(mapview->mode_group, mid, stockid, mname),
      acckey, sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));
  }
  else {
    mapview->actions->add(
      Gtk::Action::create(mid, stockid, mname),
      acckey, sigc::mem_fun(action, &ActionMode::activate));
  }

  mapview->ui_manager->add_ui_from_string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='Menu" + menu + "'>"
    "      <menuitem action='" + mid + "'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>"
  );
}