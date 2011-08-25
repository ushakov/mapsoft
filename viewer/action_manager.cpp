#include "action_manager.h"
#include "mapview.h"

#include "actions/action_mode.h"
#include "actions/am_edit_wpt.h"
#include "actions/am_add_wpt.h"
#include "actions/am_move_wpt.h"
#include "actions/am_delete_wpt.h"
#include "actions/am_delete_tpt.h"
#include "actions/am_edit_tpt.h"
#include "actions/am_move_tpt.h"
#include "actions/am_edit_track.h"
#include "actions/am_add_track.h"
#include "actions/am_mark_trk.h"
#include "actions/am_make_tiles.h"
#include "actions/am_goto_ll.h"
#include "actions/am_save_image.h"
#include "actions/am_show_pt.h"

ActionManager::ActionManager (Mapview * mapview_)
    : mapview(mapview_)
{
    current_mode = 0;

    AddAction(new ActionModeNone(mapview));
    AddAction(new AddWaypoint(mapview));
    AddAction(new EditWaypoint(mapview));
    AddAction(new MoveWaypoint(mapview));
    AddAction(new DeleteWaypoint(mapview));
    AddAction(new EditTrackpoint(mapview));
    AddAction(new MoveTrackpoint(mapview));
    AddAction(new DeleteTrackpoint(mapview));
    AddAction(new EditTrack(mapview));
    AddAction(new AddTrack(mapview));
    AddAction(new MarkTrack(mapview));
    AddAction(new MakeTiles(mapview));
    AddAction(new GotoLL(mapview));
    AddAction(new SaveImage(mapview));
    AddAction(new ShowPt(mapview));
}

void
ActionManager::AddAction(ActionMode *action){
  modes.push_back(boost::shared_ptr<ActionMode>(action));
  int m = modes.size()-1;

  std::string mid   = "Mode" + boost::lexical_cast<std::string>(m);
  std::string mname = action->get_name();

  mapview->actions->add(
    Gtk::RadioAction::create(mapview->mode_group, mid, mname),
    sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));

  mapview->ui_manager->add_ui_from_string(
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='MenuModes'>"
    "      <menuitem action='" + mid + "'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>"
  );

}