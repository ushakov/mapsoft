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
    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddWaypoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditWaypoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveWaypoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new DeleteWaypoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditTrackpoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveTrackpoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new DeleteTrackpoint(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditTrack(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddTrack(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MarkTrack(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MakeTiles(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new GotoLL(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new SaveImage(mapview)));
    modes.push_back(boost::shared_ptr<ActionMode>(new ShowPt(mapview)));

    // make all modes!
    for (int m = 0; m < modes.size(); ++m) {
        Glib::RefPtr<Gtk::RadioAction> mode_action =
            Gtk::RadioAction::create(mapview->mode_group,
              "Mode" + boost::lexical_cast<std::string>(m), modes[m]->get_name());
        mapview->actions->add(mode_action,
          sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));
    }

    Glib::ustring ui =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='MenuModes'>";
    for (int m = 0; m < modes.size(); ++m) {
        ui += "<menuitem action='Mode"
            + boost::lexical_cast<std::string>(m) + "'/>";
    }
    ui +=
        "    </menu>"
        "  </menubar>"
        "</ui>";
    mapview->ui_manager->add_ui_from_string(ui);
}

