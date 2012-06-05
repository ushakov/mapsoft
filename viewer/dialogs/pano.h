#ifndef DIALOGS_PANO_H
#define DIALOGS_PANO_H

#include <gtkmm.h>
#include "gred/dthread_viewer.h"
#include "srtm/srtm3.h"
#include "layers/layer_pano.h"
#include "../widgets/rainbow.h"

// dialog for show point action
class DlgPano : public Gtk::Dialog{
    LayerPano layer_pano;
    DThreadViewer *viewer;
    Rainbow *rb;
    Gtk::SpinButton *az;
    Gtk::SpinButton *dh;
    Gtk::Adjustment dh_adj, az_adj;

    void on_ch();
    bool on_key_press(GdkEventKey * event);
//    bool on_button_press (GdkEventButton * event);
//    bool on_scroll (GdkEventScroll * event);

  public:
    DlgPano(srtm3 * s);
    void set_origin(const dPoint & pt);
    void set_dir(const dPoint & pt);
    void set_az();
};

#endif
