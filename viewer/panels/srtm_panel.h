#ifndef VIEWER_SRTM_PANEL_H
#define VIEWER_SRTM_PANEL_H

#include "workplane.h"
#include "img_io/gobj_srtm.h"

class PanelSRTM: public Gtk::VBox, public Workplane{
public:
  PanelSRTM():
    srtm("",20),
    gobj_srtm(&srtm)
  {
    status = manage(new Gtk::CheckButton("Show SRTM data"));
    pack_start(*status, false, true, 0);
    status->signal_toggled().connect(
      sigc::mem_fun(this, &PanelSRTM::on_ch));
  }

  Options get_opt() const {
    return gobj_srtm.get_opt();}

  void set_opt(const Options & o){
    gobj_srtm.set_opt(o);
    if (exists(&gobj_srtm)) refresh_gobj(&gobj_srtm);
  }

  void on_ch(){
//    if (!mapview->have_reference) return;
    bool show = status->get_active();
    bool state = exists(&gobj_srtm);
    if (state && !show){
      remove_gobj(&gobj_srtm);
      Workplane::signal_redraw_me().emit(iRect());
    }
    else if (!state && show){
      add_gobj(&gobj_srtm, 0);
      refresh_gobj(&gobj_srtm);
    }
  }

  void show(bool show=true){
    status->set_active(show);
  }

  srtm3 srtm;
  GObjSRTM  gobj_srtm;
  Gtk::CheckButton * status;
};

#endif
