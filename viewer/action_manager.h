#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

class Mapview;

#include "actions/action_mode.h"

class ActionManager {
public:
    ActionManager(Mapview * mapview_);

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

    std::string get_mode_name (int mode) {
      return modes[mode]->get_name();
    }


    void clear_state () {
	modes[current_mode]->abort();
    }

    void click (iPoint p, const Gdk::ModifierType & state) {
	modes[current_mode]->handle_click(p, state);
    }

private:

    // used in constructor
    void AddAction(ActionMode *action, const std::string & menu, bool radio = true);

    Mapview      * mapview;
    std::vector<boost::shared_ptr<ActionMode> > modes;
    int current_mode;
};


#endif /* ACTION_MANAGER_H */
