#include <viewer/action_manager.h>

#include <viewer/action_mode.h>
#include <viewer/am_edit_wpt.h>
#include <viewer/am_add_wpt.h>
#include <viewer/am_move_wpt.h>

ActionManager::ActionManager (MapviewState * state_)
    : state(state_)
{
    current_mode = 0;
    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditWaypoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddWaypoint(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveWaypoint(state)));
}

