#ifndef ACTION_MODE_H
#define ACTION_MODE_H

#include "2d/point.h"
#include <gtkmm.h>

#define GETW(x,y)\
  y=0; builder->get_widget(Glib::ustring(x), y); assert (y);
#define GETWD(x,y)\
  y=0; builder->get_widget_derived(Glib::ustring(x), y); assert (y);

class Mapview;
class LayerTRK;
class LayerWPT;
class LayerGeoMap;

// Interface.
class ActionMode {
public:

    Mapview * mapview;
    ActionMode(Mapview * mapview_) : mapview(mapview_) { }

    // Returns name, stock id, accel key, tooltip for the mode.
    virtual std::string   get_name() = 0;
    virtual Gtk::StockID  get_stockid() { return Gtk::StockID(); }
    virtual Gtk::AccelKey get_acckey()  { return Gtk::AccelKey(""); }
    // for non-radio modes only activate method is called.
    virtual bool is_radio() { return true; }

    // Activates this mode.
    virtual void activate() { };

    // Abandons any action in progress and deactivates mode.
    // no need to reset rubber and generic_dialog
    virtual void abort() { };

    // Sends user click. Coordinates are in workplane's discrete system.
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) { };

    // find waypoint, returns its number 0..size()-1
    int find_wpt(const iPoint & p, LayerWPT ** layer,
                 int radius=3) const;
    // segment=true: find track point, returns its number 0..size()-1
    // segment=true: find track segment, return its
    //               first point 0..size()-2
    int find_tpt(const iPoint & p, LayerTRK ** layer,
                 const bool segment = false, int radius=3) const;
    int find_map(const iPoint & p, LayerGeoMap ** layer) const;

    // find first active layer
    LayerWPT * find_wpt_layer() const;
    LayerGeoMap * find_map_layer() const;

};


class ActionModeNone : public ActionMode {
public:
    ActionModeNone (Mapview * mapview) : ActionMode(mapview) { }
    virtual std::string get_name() { return "None"; }
    virtual void activate() { }
    virtual void abort() { }
    virtual void handle_click(iPoint p, const Gdk::ModifierType & state) { }
};


#endif /* ACTION_MODE_H */
