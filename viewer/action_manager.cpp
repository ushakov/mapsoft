#include "action_manager.h"
#include "mapview.h"

#include "actions/action_mode.h"
#include "actions/am_new.h"
#include "actions/am_open.h"
#include "actions/am_add_file.h"
#include "actions/am_save.h"
#include "actions/am_save_all.h"
#include "actions/am_save_vis.h"
#include "actions/am_download.h"
#include "actions/am_quit.h"

#include "actions/am_add_wpt.h"
#include "actions/am_edit_wpt.h"
#include "actions/am_move_wpt.h"
#include "actions/am_delete_wpt.h"

#include "actions/am_add_track.h"
#include "actions/am_edit_track.h"
#include "actions/am_del_trk.h"
#include "actions/am_add_tpt.h"
#include "actions/am_edit_tpt.h"
#include "actions/am_move_tpt.h"
#include "actions/am_delete_tpt.h"
#include "actions/am_mark_trk.h"
#include "actions/am_trk_filter.h"
#include "actions/am_trk_opt.h"

#include "actions/am_edit_map.h"
#include "actions/am_setref.h"
#include "actions/am_llref.h"

#include "actions/srtm_riv.h"
#include "actions/srtm_area.h"
#include "actions/am_srtm_opt.h"
#include "actions/am_pano.h"

#include "actions/am_save_image.h"
#include "actions/am_show_pt.h"
#include "actions/am_nav.h"

#include "actions/am_fullscreen.h"
#include "actions/am_hide_panels.h"

#include "actions/panel_actions.h"

#define ADD_ACT(name, group) AddAction(new name(mapview),\
   std::string("Mode") + #name, group);


ActionManager::ActionManager (Mapview * mapview_)
    : mapview(mapview_)
{
    current_mode = 0;

    ADD_ACT(New,             "File")
    ADD_ACT(Open,            "File")
    ADD_ACT(AddFile,         "File")
    ADD_ACT(Download,        "File")
    AddSep("File");
    ADD_ACT(Save,            "File")
    ADD_ACT(SaveAll,         "File")
    ADD_ACT(SaveVis,         "File")
    AddSep("File");
    ADD_ACT(Quit,            "File")

    ADD_ACT(AddWaypoint,     "Waypoints")
    ADD_ACT(EditWaypoint,    "Waypoints")
    ADD_ACT(MoveWaypoint,    "Waypoints")
    ADD_ACT(DeleteWaypoint,  "Waypoints")

    ADD_ACT(AddTrack,        "Tracks")
    ADD_ACT(EditTrack,       "Tracks")
    ADD_ACT(DelTrk,          "Tracks")
    AddSep("Tracks");
    ADD_ACT(AddTrackpoint,   "Tracks")
    ADD_ACT(EditTrackpoint,  "Tracks")
    ADD_ACT(MoveTrackpoint,  "Tracks")
    ADD_ACT(DeleteTrackpoint,"Tracks")
    AddSep("Tracks");
    ADD_ACT(MarkTrack,       "Tracks")
    ADD_ACT(FilterTrack,     "Tracks")

    ADD_ACT(EditMap,         "Maps")
    ADD_ACT(SetRef,          "Maps")
    ADD_ACT(LLRef,           "Maps")

    ADD_ACT(SrtmRiv,         "SRTM")
    ADD_ACT(SrtmArea,         "SRTM")
    ADD_ACT(SrtmOpt,         "SRTM")
    ADD_ACT(Pano,            "SRTM")

    ADD_ACT(SaveImage,       "Misc")
    ADD_ACT(ShowPt,          "Misc")
    ADD_ACT(Nav,             "Misc")
    AddSep("Misc");
    ADD_ACT(FullScreen,      "Misc")
    ADD_ACT(HidePanels,      "Misc")

    /* Wpt panel menu*/
    ADD_ACT(PanelGoto,        "PopupWPTs")
    ADD_ACT(PanelSave,        "PopupWPTs")
    AddSep("PopupWPTs");
    ADD_ACT(PanelShowAll,     "PopupWPTs")
    ADD_ACT(PanelHideAll,     "PopupWPTs")
    ADD_ACT(PanelInvert,      "PopupWPTs")
    AddSep("PopupWPTs");
    ADD_ACT(PanelMoveTop,     "PopupWPTs")
    ADD_ACT(PanelMoveUp,      "PopupWPTs")
    ADD_ACT(PanelMoveDown,    "PopupWPTs")
    ADD_ACT(PanelMoveBottom,  "PopupWPTs")
    AddSep("PopupWPTs");
    ADD_ACT(PanelJoinVis,     "PopupWPTs")
    ADD_ACT(PanelJoinAll,     "PopupWPTs")
    AddSep("PopupWPTs");
    ADD_ACT(PanelDelSel,      "PopupWPTs")
    ADD_ACT(PanelDelAll,      "PopupWPTs")

    /* Trk panel menu*/
    ADD_ACT(PanelGoto,        "PopupTRKs")
    ADD_ACT(PanelSave,        "PopupTRKs")
    AddSep("PopupTRKs");
    ADD_ACT(PanelShowAll,     "PopupTRKs")
    ADD_ACT(PanelHideAll,     "PopupTRKs")
    ADD_ACT(PanelInvert,      "PopupTRKs")
    AddSep("PopupTRKs");
    ADD_ACT(PanelMoveTop,     "PopupTRKs")
    ADD_ACT(PanelMoveUp,      "PopupTRKs")
    ADD_ACT(PanelMoveDown,    "PopupTRKs")
    ADD_ACT(PanelMoveBottom,  "PopupTRKs")
    AddSep("PopupTRKs");
    ADD_ACT(PanelJoinVis,     "PopupTRKs")
    ADD_ACT(PanelJoinAll,     "PopupTRKs")
    AddSep("PopupTRKs");
    ADD_ACT(PanelDelSel,      "PopupTRKs")
    ADD_ACT(PanelDelAll,      "PopupTRKs")
    AddSep("PopupTRKs");
    ADD_ACT(DrawOpt,          "PopupTRKs")

    /* Map panel menu*/
    ADD_ACT(PanelGoto,        "PopupMAPs")
    ADD_ACT(PanelSave,        "PopupMAPs")
    AddSep("PopupMAPs");
    ADD_ACT(PanelShowAll,     "PopupMAPs")
    ADD_ACT(PanelHideAll,     "PopupMAPs")
    ADD_ACT(PanelInvert,      "PopupMAPs")
    AddSep("PopupMAPs");
    ADD_ACT(PanelMoveTop,     "PopupMAPs")
    ADD_ACT(PanelMoveUp,      "PopupMAPs")
    ADD_ACT(PanelMoveDown,    "PopupMAPs")
    ADD_ACT(PanelMoveBottom,  "PopupMAPs")
    AddSep("PopupMAPs");
    ADD_ACT(PanelJoinVis,     "PopupMAPs")
    ADD_ACT(PanelJoinAll,     "PopupMAPs")
    AddSep("PopupMAPs");
    ADD_ACT(PanelDelSel,      "PopupMAPs")
    ADD_ACT(PanelDelAll,      "PopupMAPs")


    /* */

    mapview->actions->add(Gtk::Action::create("MenuFile", "_File"));
    mapview->actions->add(Gtk::Action::create("MenuWaypoints", "_Waypoints"));
    mapview->actions->add(Gtk::Action::create("MenuTracks", "_Tracks"));
    mapview->actions->add(Gtk::Action::create("MenuMaps", "_Maps"));
    mapview->actions->add(Gtk::Action::create("MenuSRTM", "_SRTM"));
    mapview->actions->add(Gtk::Action::create("MenuMisc", "Mi_sc"));

    modes.push_back(boost::shared_ptr<ActionMode>(new ActionModeNone(mapview)));
}


void
ActionManager::AddSep(const std::string & menu){
  if (menu.substr(0,5) == "Popup"){
    mapview->ui_manager->add_ui_from_string(
      "<ui>"
      "  <popup name='" + menu + "'>"
      "    <separator/>"
      "  </popup>"
      "</ui>"
    );
  }
  else{
    mapview->ui_manager->add_ui_from_string(
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='Menu" + menu + "'>"
      "      <separator/>"
      "    </menu>"
      "  </menubar>"
      "</ui>"
    );
  }
}

void
ActionManager::AddAction(ActionMode *action,
                         const std::string & id, const std::string & menu){
  modes.push_back(boost::shared_ptr<ActionMode>(action));
  int m = modes.size()-1;
  std::string  mname   = action->get_name();
  Gtk::StockID stockid = action->get_stockid();
  Gtk::AccelKey acckey = action->get_acckey();

  if (!mapview->actions->get_action(id)){
    if (action->is_radio()) {
      // I do not know how to create empty editable AccelKey. So i use
      // these stupid ifs...
      if (acckey.is_null())
        mapview->actions->add(
          Gtk::Action::create(id, stockid, mname),
          sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));
      else
        mapview->actions->add(
          Gtk::Action::create(id, stockid, mname),
          acckey, sigc::bind (sigc::mem_fun(mapview, &Mapview::on_mode_change), m));
    }
    else {
      if (acckey.is_null())
        mapview->actions->add(
          Gtk::Action::create(id, stockid, mname),
          sigc::mem_fun(action, &ActionMode::activate));
      else
        mapview->actions->add(
          Gtk::Action::create(id, stockid, mname),
          acckey, sigc::mem_fun(action, &ActionMode::activate));
    }
  }

  if (menu.substr(0,5) == "Popup"){
    mapview->ui_manager->add_ui_from_string(
      "<ui>"
      "  <popup name='" + menu + "'>"
      "    <menuitem action='" + id + "'/>"
      "  </popup>"
      "</ui>"
    );
  }
  else{
    mapview->ui_manager->add_ui_from_string(
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='Menu" + menu + "'>"
      "      <menuitem action='" + id + "'/>"
      "    </menu>"
      "  </menubar>"
      "</ui>"
    );
  }
}