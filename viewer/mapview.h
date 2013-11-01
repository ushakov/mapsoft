#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <boost/shared_ptr.hpp>
#include <gtkmm.h>
#include <gtkmm/accelmap.h>
#include <map>
#include <string>

#include "gred/dthread_viewer.h"
#include "gred/rubber.h"
#include "img_io/gobj_comp.h"
#include "action_manager.h"
#include "geo_io/io.h"
#include "2d/rect.h"
#include "dialogs/ch_conf.h"
#include "dialogs/err.h"

#include "panels/wpts_panel.h"
#include "panels/trks_panel.h"
#include "panels/maps_panel.h"
#include "panels/srtm_panel.h"

#define ACCEL_FILE ".mapsoft/accel"

class Mapview : public Gtk::Window {
public:
    DThreadViewer viewer;
    Rubber        rubber;
    GObjComp      main_gobj;

    Gtk::Notebook * panels;
    PanelTRK  panel_trks;
    PanelWPT  panel_wpts;
    PanelMAP  panel_maps;
    PanelSRTM panel_srtm;

    Glib::RefPtr<Gtk::ActionGroup> actions;
    Glib::RefPtr<Gtk::UIManager> ui_manager;
    Gtk::Menu *popup_trks, *popup_wpts, *popup_maps, *popup_srtm;
    Gtk::Statusbar statusbar;
    Gtk::Image *busy_icon;

    bool have_reference;
    bool divert_refresh;

    DlgChConf dlg_ch_conf;
    DlgErr dlg_err;

private:
    boost::shared_ptr<ActionManager> action_manager;
    iPoint click_start;
    std::string filename; // project filename
    bool changed;         // true if project was changed since
                          // last saving/loading
public:

    Mapview ();

    void panel_edited (const Gtk::TreeModel::Path& path,
                       const Gtk::TreeModel::iterator& iter);

    void update_gobjs();

    void on_mode_change (int m);
    std::string get_filename() const;
    void set_filename(const std::string & f);

    bool get_changed() const;
    void set_changed(const bool c=true);
    void add_files(const std::list<std::string> & files);       // add data from files
    void load_file(const std::string & file, bool force=false); // load new data from file
    void new_file(bool force=false);                            // start new project

    void add_world(const geo_data & world, bool scroll=false);
    void clear_world();
    // build geo_data object with all/visible data
    geo_data get_world(bool visible=true);

    void set_ref(const g_map & ref);
    const convs::map2wgs* get_cnv() {return main_gobj.get_cnv();}
    void rescale(double k){ main_gobj.rescale(k);}

    void goto_wgs(dPoint p);
    void exit(bool force=false);
    void refresh();
    bool on_key_press(GdkEventKey * event);
    bool on_button_press (GdkEventButton * event);
    bool on_button_release (GdkEventButton * event);
    bool on_scroll (GdkEventScroll * event);
    bool on_panel_button_press (GdkEventButton * event);
    void show_busy_mark();
    void hide_busy_mark();
};


#endif /* MAPVIEW_H */
