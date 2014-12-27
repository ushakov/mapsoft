#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <boost/shared_ptr.hpp>
#include <gtkmm.h>
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
#include "panels/status_panel.h"

//#define ACCEL_FILE ".mapsoft/accel"

class Mapview : public Gtk::Window {
public:
    /// Mapview components:
    DThreadViewer viewer;    // Viewer, gtk widget which shows main_gobj
    Rubber        rubber;    // Rubber lines
    GObjComp      main_gobj; // Main workplane

    // Right panel, a Gtk::Notebook with separate
    // panels for waypoints, tracks, maps ...
    Gtk::Notebook * panels;
    PanelTRK  panel_trks;
    PanelWPT  panel_wpts;
    PanelMAP  panel_maps;
    PanelSRTM panel_srtm;
    StatusPanel spanel; // status bar
    ActionManager action_manager; // menus and action handling

    bool have_reference;

    DlgChConf dlg_ch_conf; // "data was changed" dialog
    DlgErr dlg_err; // error dialog

private:
    std::string filename; // project filename
    bool changed;         // true if project was changed since
                          // last saving/loading
public:

    Mapview ();

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

    void goto_wgs(dPoint p);
    void exit(bool force=false);
};


#endif /* MAPVIEW_H */
