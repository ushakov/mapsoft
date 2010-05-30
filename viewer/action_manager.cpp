#include "action_manager.h"

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

ActionManager::ActionManager (Mapview * mapview_)
    : mapview(mapview_)
{
    current_mode = 0;
    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone));
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
}

