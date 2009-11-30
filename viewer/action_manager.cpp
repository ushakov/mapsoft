#include "action_manager.h"

#include "action_mode.h"
#include "am_edit_wpt.h"
#include "am_add_wpt.h"
#include "am_move_wpt.h"
#include "am_delete_wpt.h"
#include "am_delete_tpt.h"
#include "am_edit_tpt.h"
#include "am_move_tpt.h"
#include "am_edit_track.h"
#include "am_add_track.h"
#include "am_mark_trk.h"
#include "am_make_tiles.h"

ActionManager::ActionManager (Mapview * state_)
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
    modes.push_back(boost::shared_ptr<ActionMode>(new MarkTrack(state)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MakeTiles(state)));
}

