#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <gtkmm/accelmap.h>

#define ACCEL_FILE ".mapsoft/accel"

class Mapview;

#include "action_mode.h"

class ActionManager {
public:
    ActionManager(Mapview * mapview_);

    // clear rubber, abort current mode
    void clear_state();

    // do clear_state, activate a new mode, put mode name to the statusbar
    void set_mode (int new_mode);

    void click (iPoint p, int button, const Gdk::ModifierType & state) {
      if (button == 3) clear_state();
      else modes[current_mode]->handle_click(p, state);
    }

    Gtk::Widget * get_main_menu() { return ui_manager->get_widget("/MenuBar"); }

private:

    // Menus
    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;
    Gtk::Menu *popup_trks, *popup_wpts, *popup_maps, *popup_srtm;

    // used in the constructor
    void AddAction(ActionMode *action, const std::string & id, const std::string & menu);
    void AddSep(const std::string & menu);

    Mapview * mapview;
    std::vector<boost::shared_ptr<ActionMode> > modes;
    int current_mode;

    // Control menu popups for the right panel
    bool on_panel_button_press (GdkEventButton * event);
};


#endif /* ACTION_MANAGER_H */
