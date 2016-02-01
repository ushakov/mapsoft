#ifndef PANEL_ACTIONS_H
#define PANEL_ACTIONS_H

#include "action_mode.h"

class PanelDelSel : public ActionMode{
public:
    PanelDelSel (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Delete selected"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }
    bool is_radio() { return false; }
    void activate() {
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.remove_selected(); break;
        case 1: mapview->panel_trks.remove_selected(); break;
        case 2: mapview->panel_maps.remove_selected(); break;
      }
    }
};

class PanelDelAll : public ActionMode{
public:
    PanelDelAll (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Delete all"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::DELETE; }
    bool is_radio() { return false; }
    void activate() {
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.remove_all(); break;
        case 1: mapview->panel_trks.remove_all(); break;
        case 2: mapview->panel_maps.remove_all(); break;
      }
    }
};

class PanelMoveUp : public ActionMode{
public:
    PanelMoveUp (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Move up"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::GO_UP; }
    bool is_radio() { return false; }
    void activate() {
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.move(true,false); break;
        case 1: mapview->panel_trks.move(true,false); break;
        case 2: mapview->panel_maps.move(true,false); break;
      }
    }
};

class PanelMoveDown : public ActionMode{
public:
    PanelMoveDown (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Move down"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::GO_DOWN; }
    bool is_radio() { return false; }
    void activate() {
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.move(false,false); break;
        case 1: mapview->panel_trks.move(false,false); break;
        case 2: mapview->panel_maps.move(false,false); break;
      }
    }
};

class PanelMoveTop : public ActionMode{
public:
    PanelMoveTop (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Move to top"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::GOTO_TOP; }
    bool is_radio() { return false; }
    void activate() {
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.move(true,true); break;
        case 1: mapview->panel_trks.move(true,true); break;
        case 2: mapview->panel_maps.move(true,true); break;
      }
    }
};

class PanelMoveBottom : public ActionMode{
public:
    PanelMoveBottom (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Move to bottom"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::GOTO_BOTTOM; }
    bool is_radio() { return false; }
    void activate() {
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.move(false,true); break;
        case 1: mapview->panel_trks.move(false,true); break;
        case 2: mapview->panel_maps.move(false,true); break;
      }
    }
};

class PanelHideAll : public ActionMode{
public:
    PanelHideAll (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Hide All"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::REMOVE; }
    bool is_radio() { return false; }
    void activate() {
      mapview->viewer.start_waiting();
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.show_all(false); break;
        case 1: mapview->panel_trks.show_all(false); break;
        case 2: mapview->panel_maps.show_all(false); break;
      }
      mapview->viewer.stop_waiting();
    }
};

class PanelShowAll : public ActionMode{
public:
    PanelShowAll (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Show All"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::ADD; }
    bool is_radio() { return false; }
    void activate() {
      mapview->viewer.start_waiting();
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.show_all(); break;
        case 1: mapview->panel_trks.show_all(); break;
        case 2: mapview->panel_maps.show_all(); break;
      }
      mapview->viewer.stop_waiting();
    }
};

class PanelInvert : public ActionMode{
public:
    PanelInvert (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Invert visibility"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::REFRESH; }
    bool is_radio() { return false; }
    void activate() {
      mapview->viewer.start_waiting();
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.invert_all(); break;
        case 1: mapview->panel_trks.invert_all(); break;
        case 2: mapview->panel_maps.invert_all(); break;
      }
      mapview->viewer.stop_waiting();
    }
};

class PanelJoinVis : public ActionMode{
public:
    PanelJoinVis (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Join visible"; }
    bool is_radio() { return false; }
    void activate() {
      mapview->viewer.start_waiting();
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.join(true); break;
        case 1: mapview->panel_trks.join(true); break;
        case 2: mapview->panel_maps.join(true); break;
      }
      mapview->viewer.stop_waiting();
    }
};

class PanelJoinAll : public ActionMode{
public:
    PanelJoinAll (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Join all"; }
    bool is_radio() { return false; }
    void activate() {
      mapview->viewer.start_waiting();
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.join(false); break;
        case 1: mapview->panel_trks.join(false); break;
        case 2: mapview->panel_maps.join(false); break;
      }
      mapview->viewer.stop_waiting();
    }
};

class PanelGoto : public ActionMode{
public:
    PanelGoto (Mapview * mapview) : ActionMode(mapview){ }
    std::string get_name() { return "Goto"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::JUMP_TO; }
    bool is_radio() { return false; }
    void activate() {
      dPoint p;
      switch (mapview->panels->get_current_page()){
        case 0: p=mapview->panel_wpts.get_sel_point(); break;
        case 1: p=mapview->panel_trks.get_sel_point(); break;
        case 2: p=mapview->panel_maps.get_sel_point(); break;
      }
      if (!std::isnan(p.x)) mapview->goto_wgs(p);
    }
};

class PanelSave : public ActionMode, public Gtk::FileSelection{
public:
    PanelSave (Mapview * mapview) :
           ActionMode(mapview), Gtk::FileSelection(get_name()){
      set_transient_for(*mapview);
      get_ok_button()->signal_clicked().connect(
          sigc::mem_fun (this, &PanelSave::on_ok));
      get_cancel_button()->signal_clicked().connect(
          sigc::mem_fun(this, &Gtk::Window::hide));
    }

    std::string get_name() { return "Save selected"; }
    Gtk::StockID get_stockid() { return Gtk::Stock::SAVE_AS; }
    bool is_radio() { return false; }
    void activate() { show(); }

    void on_ok(){
      std::string f = get_filename();
      geo_data w;
      switch (mapview->panels->get_current_page()){
        case 0: mapview->panel_wpts.get_sel_data(w); break;
        case 1: mapview->panel_trks.get_sel_data(w); break;
        case 2: mapview->panel_maps.get_sel_data(w); break;
      }
      if (!w.empty()){
        try {io::out(f, w);}
        catch (MapsoftErr e) {mapview->dlg_err.call(e);}
      }
      hide();
    }
};

#endif
