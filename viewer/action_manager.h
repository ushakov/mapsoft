#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include <viewer/action_mode.h>

class MapviewState;

class ActionManager {
public:
    ActionManager(MapviewState * state_);

    std::vector<std::string> get_mode_list () {
	std::vector<std::string> r;
	for (int i = 0; i < modes.size(); ++i) {
	    r.push_back(modes[i]->get_name());
	}
	return r;
    }

    void set_mode (int new_mode) {
	clear_state();
	current_mode = new_mode;
	modes[current_mode]->activate();
    }

    void clear_state () {
	modes[current_mode]->abort();
    }
    
    void click (Point<int> p) {
	modes[current_mode]->handle_click(p);
    }

private:
    MapviewState * state;
    std::vector<boost::shared_ptr<ActionMode> > modes;
    int current_mode;
};


#endif /* ACTION_MANAGER_H */
