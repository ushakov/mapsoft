#include <viewer/action_manager.h>

#include <viewer/action_mode.h>
#include <viewer/am_edit_wpt.h>
#include <viewer/am_add_wpt.h>
#include <viewer/am_move_wpt.h>
#include <viewer/am_delete_wpt.h>
#include <viewer/am_delete_tpt.h>
#include <viewer/am_edit_tpt.h>
#include <viewer/am_move_tpt.h>
#include <viewer/am_edit_track.h>
#include <viewer/am_add_track.h>

ActionManager::ActionManager (MapviewState * state_)
    : state(state_)
{
    current_mode = 0;
    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddWaypoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditWaypoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveWaypoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new DeleteWaypoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditTrackpoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveTrackpoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new DeleteTrackpoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditTrack(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddTrack(state)));
}

