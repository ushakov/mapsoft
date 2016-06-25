#ifndef DIALOGS_PANO_H
#define DIALOGS_PANO_H

#include <gtkmm.h>
#include "gred/dthread_viewer.h"
#include "gred/rubber.h"
#include "srtm/srtm3.h"
#include "img_io/gobj_pano.h"
#include "../widgets/rainbow.h"

// dialog for show point action
class DlgPano : public Gtk::Dialog{
    GObjPano gobj_pano;
    DThreadViewer viewer;
    Rubber rubber;
    Rainbow *rb;
    Gtk::SpinButton *az;
    Gtk::SpinButton *dh;
    Gtk::SpinButton *mr;
    Gtk::Adjustment dh_adj, az_adj, mr_adj;

    void on_ch();
    bool on_key_press(GdkEventKey * event);
    void click (iPoint p, int button, const Gdk::ModifierType & state);

    sigc::signal<void, dPoint> signal_go_;
    sigc::signal<void, dPoint> signal_point_;

  public:
    DlgPano(SRTM3 * s);
    void set_origin(const dPoint & pt);
    void set_dir(const dPoint & pt);
    void set_az();
    void get_az(iPoint p); // update az value from viewer signal

    sigc::signal<void, dPoint> signal_go();
    sigc::signal<void, dPoint> signal_point();
};


#endif
