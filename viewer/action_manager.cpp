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

ActionManager::ActionManager (Mapview * state_, Viewer * viewer_)
    : state(state_), viewer(viewer_)
{
    current_mode = 0;
    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddWaypoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditWaypoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveWaypoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new DeleteWaypoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditTrackpoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MoveTrackpoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new DeleteTrackpoint(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new EditTrack(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new AddTrack(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MarkTrack(state, viewer)));
    modes.push_back(boost::shared_ptr<ActionMode>(new MakeTiles(state, viewer)));
}

